#pragma once
#include <windows.h>
#include <fstream>
#include <vector>

#include "vm.h"

struct ManualInject {
    uintptr_t ImageBase;
    IMAGE_NT_HEADERS* NtHeaders;
    IMAGE_DATA_DIRECTORY BaseRelocation;
    IMAGE_DATA_DIRECTORY ImportDirectory;
    IMAGE_DATA_DIRECTORY TLSDirectory;
    IMAGE_DATA_DIRECTORY ExceptionDirectory;
    uintptr_t fnLoadLibraryA;
    uintptr_t fnGetProcAddress;
    uintptr_t fnRtlAddFunctionTable;
};

BOOL WINAPI DllMainDefinition(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpvReserved);

DWORD load_dll(ManualInject* manual_inject, VirtualMachine* vm) {
    if (manual_inject->BaseRelocation.Size != 0) {
        PIMAGE_BASE_RELOCATION pIBR = (PIMAGE_BASE_RELOCATION)(manual_inject->ImageBase + manual_inject->BaseRelocation.VirtualAddress);

        auto delta = (manual_inject->ImageBase - manual_inject->NtHeaders->OptionalHeader.ImageBase);
        auto reloc_end = pIBR + pIBR->SizeOfBlock;
        size_t cur_size = 0;

        // Relocate the image
        while (cur_size < manual_inject->BaseRelocation.Size) {
            auto count = (pIBR->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
            WORD* list = (WORD*)(pIBR + 1);

            for (int i = 0; i < count; i++) {
                auto reloc_entry = *(list + i);

                if ((reloc_entry >> 0x0C) == IMAGE_REL_BASED_DIR64) {
                    auto ptr = (uintptr_t*)(manual_inject->ImageBase
                        + pIBR->VirtualAddress
                        + ((uintptr_t)reloc_entry & 0xFFF));

                    *ptr = (*ptr + delta);
                }
            }

            cur_size += pIBR->SizeOfBlock;
            pIBR = (PIMAGE_BASE_RELOCATION)((uintptr_t)pIBR + pIBR->SizeOfBlock);
        }
    }

    if (manual_inject->ImportDirectory.Size != 0) {
        auto pIID = (PIMAGE_IMPORT_DESCRIPTOR)(manual_inject->ImageBase + manual_inject->ImportDirectory.VirtualAddress);

        // Resolve DLL imports
        while (pIID->Name != 0) {
            auto OrigFirstThunk = (uintptr_t*)(manual_inject->ImageBase + pIID->OriginalFirstThunk);
            auto FirstThunk = (uintptr_t*)(manual_inject->ImageBase + pIID->FirstThunk);

            auto load_library = (decltype(LoadLibraryA)*)(manual_inject->fnLoadLibraryA);
            auto hModule = load_library((LPCSTR)(manual_inject->ImageBase + pIID->Name));

            if (!hModule) {
                return 0;
            }

            auto OrigFirstThunk_iter = OrigFirstThunk;
            auto FirstThunk_iter = FirstThunk;

            if (!OrigFirstThunk_iter) {
                OrigFirstThunk_iter = FirstThunk_iter;
            }

            auto get_proc_address = (decltype(GetProcAddress)*)(manual_inject->fnGetProcAddress);

            while (*OrigFirstThunk_iter != 0) {
                if ((*OrigFirstThunk_iter & IMAGE_ORDINAL_FLAG64) != 0) {
                    // Import by ordinal
                    auto Function = get_proc_address(hModule, (LPCSTR)(*OrigFirstThunk_iter & 0xFFFF));

                    if (!Function) {
                        return 0;
                    }

                    *FirstThunk_iter = (uintptr_t)Function;
                }
                else {
                    // Import by name
                    auto pIBN = (IMAGE_IMPORT_BY_NAME*)(manual_inject->ImageBase + *OrigFirstThunk_iter);
                    auto name = pIBN->Name;
                    FARPROC Function;
                    __asm {
                        mov rcx, hModule
                        mov rdx, name
                        call get_proc_address
                        mov Function, rax
                    }
                    
                    //get_proc_address(hModule, pIBN->Name);

                    if (!Function) {
                        return 0;
                    }

                    *FirstThunk_iter = (uintptr_t)Function;
                }

                OrigFirstThunk_iter = OrigFirstThunk_iter + 1;
                FirstThunk_iter = FirstThunk_iter + 1;
            }

            pIID += 1;
        }
    }

    if (manual_inject->TLSDirectory.Size != 0) {
        auto pTLS = (PIMAGE_TLS_DIRECTORY)(manual_inject->ImageBase + manual_inject->TLSDirectory.VirtualAddress);
        auto callback = (PIMAGE_TLS_CALLBACK*)(pTLS->AddressOfCallBacks);

        while (callback && *callback) {
            vm->execute((uintptr_t)*callback, manual_inject->ImageBase, DLL_PROCESS_ATTACH, 0, 0);
            //(*callback)((void*)manual_inject->ImageBase, DLL_PROCESS_ATTACH, nullptr);
            callback += 1;
        }
    }

    /*if (manual_inject->ExceptionDirectory.Size != 0) {
        auto fnRtlAddFunctionTable = (decltype(RtlAddFunctionTable)*)(manual_inject->fnRtlAddFunctionTable);

        fnRtlAddFunctionTable(
            (PRUNTIME_FUNCTION)(manual_inject->ImageBase + manual_inject->ExceptionDirectory.VirtualAddress),
            manual_inject->ExceptionDirectory.Size / sizeof(IMAGE_RUNTIME_FUNCTION_ENTRY),
            manual_inject->ImageBase
        );
    }*/

    if (manual_inject->NtHeaders->OptionalHeader.AddressOfEntryPoint != 0) {
        return vm->execute(manual_inject->ImageBase + manual_inject->NtHeaders->OptionalHeader.AddressOfEntryPoint, manual_inject->ImageBase, DLL_PROCESS_ATTACH, 0, 0);
        //auto EntryPoint = (decltype(DllMainDefinition)*)(manual_inject->ImageBase + manual_inject->NtHeaders->OptionalHeader.AddressOfEntryPoint);
        //return EntryPoint((HINSTANCE)manual_inject->ImageBase, DLL_PROCESS_ATTACH, nullptr);
    }

    return 1;
}

class Loader {
public:
	static void load(const char* filepath) {
        auto image_stream = std::ifstream(filepath, std::ios::in | std::ios::binary);
        
        if (!image_stream.is_open()) {
            printf("failed to open image\n");
            return;
        }
        
        image_stream.seekg(0, std::ios::end);
        auto image_size = image_stream.tellg();
        image_stream.seekg(0, std::ios::beg);
        image_size -= image_stream.tellg();
        
        std::vector<uint8_t> image_data(image_size);
        image_stream.read((char*)image_data.data(), image_size);
        
        image_stream.close();
        
        auto dos = PIMAGE_DOS_HEADER(image_data.data());
        auto nt = PIMAGE_NT_HEADERS(image_data.data() + dos->e_lfanew);
        
        uint8_t* image_base = (uint8_t*)VirtualAlloc(0, nt->OptionalHeader.SizeOfImage, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        
        if (!image_base) {
            printf("failed to allocate virtual page\n");
            return;
        }
        
        // write headers
        memcpy(image_base, image_data.data(), nt->OptionalHeader.SizeOfHeaders);
        
        uintptr_t vm_start = 0, vm_end = 0;

        // write sections
        auto section = IMAGE_FIRST_SECTION(nt);
        for (int i = 0; i < nt->FileHeader.NumberOfSections; i++, section++) {
            memcpy(image_base + section->VirtualAddress, image_data.data() + section->PointerToRawData, section->SizeOfRawData);

            if (*(uint32_t*)section->Name == *(uint32_t*)".pvm") {
                if (!vm_start)
                    vm_start = (uintptr_t)image_base + section->VirtualAddress;
                
                vm_end = (uintptr_t)image_base + section->VirtualAddress + section->Misc.VirtualSize;
            }

            if (section->Characteristics & IMAGE_SCN_MEM_EXECUTE)
                VirtualProtect(image_base + section->VirtualAddress, section->Misc.VirtualSize, PAGE_EXECUTE_READ, nullptr);
        }

        // create vm instance
        VirtualMachine vm(vm_start, vm_end);
        
        // setup inject data
        ManualInject inject_data;
        inject_data.ImageBase = (uintptr_t)image_base;
        inject_data.NtHeaders = PIMAGE_NT_HEADERS(image_base + dos->e_lfanew);
        inject_data.BaseRelocation = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
        inject_data.ImportDirectory = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
        inject_data.TLSDirectory = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS];
        inject_data.ExceptionDirectory = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXCEPTION];
        inject_data.fnLoadLibraryA = (uintptr_t)GetProcAddress(LoadLibrary(L"kernel32.dll"), "LoadLibraryA");
        inject_data.fnGetProcAddress = (uintptr_t)GetProcAddress(LoadLibrary(L"kernel32.dll"), "GetProcAddress");
        inject_data.fnRtlAddFunctionTable = (uintptr_t)GetProcAddress(LoadLibrary(L"kernel32.dll"), "RtlAddFunctionTable");
        
        // load dll
        auto result = load_dll(&inject_data, &vm);

        printf("result: %p\n", result);
	}
};