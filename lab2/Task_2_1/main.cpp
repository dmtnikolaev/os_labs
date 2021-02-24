#include <iostream>
#include <windows.h>

const int blockSize = 4096;

typedef void (*MenuProcessingFunctionPointer)();

struct MenuOption
{
    int choice;
    char const * p_selection_text;
    MenuProcessingFunctionPointer p_processing_function;
};

void OptGetSystemInfo();
void OptGlobalMemoryStatus();
void OptVirtualQuery();
void OptVirtualAlloc();
void OptVirtualAllocAuto();
void OptWrite();
void OptVirtualProtect();
void OptVirtualFree();

static const MenuOption MAIN_MENU[] =
        {
                {1, "GetSystemInfo", OptGetSystemInfo},
                {2, "GlobalMemoryStatus", OptGlobalMemoryStatus},
                {3, "VirtualQuery", OptVirtualQuery},
                {4, "VirtualAlloc", OptVirtualAlloc},
                {5, "VirtualAllocAuto", OptVirtualAllocAuto},
                {6, "Write", OptWrite},
                {7, "VirtualProtect", OptVirtualProtect},
                {8, "VirtualFree", OptVirtualFree},
        };

int main()
{
    static const char *const menu_title =
            "\n"
            "------------------------------\n"
            "         Main Menu\n"
            "------------------------------\n"
    ;

    while(true) {
        std::cout.write(menu_title, strlen(menu_title));
        for (const auto &opt : MAIN_MENU) {
            std::cout << opt.choice << " - "<< opt.p_selection_text << "\n";
        }
        std::cout << "Enter selection, 0 to quit: ";
        int choice;
        std::cin >> choice;
        if (choice == 0) break;
        for (const auto &opt : MAIN_MENU) {
            if (choice == opt.choice) {
                auto p_function = opt.p_processing_function;
                (p_function)();
                break;
            }
        }
    }
    return EXIT_SUCCESS;
}

void OptGetSystemInfo() {
    SYSTEM_INFO system_info;
    GetSystemInfo(&system_info);

    std::cout << "Architecture: ";
    WORD arch = system_info.wProcessorArchitecture;
    switch (arch) {
        case PROCESSOR_ARCHITECTURE_AMD64:
            std::cout << "x86 (AMD or Intel)";
            break;
        case PROCESSOR_ARCHITECTURE_ARM:
            std::cout << "ARM";
            break;
        case PROCESSOR_ARCHITECTURE_ARM64:
            std::cout << "ARM64";
            break;
        case PROCESSOR_ARCHITECTURE_IA64:
            std::cout << "Intel Itanium-based";
            break;
        case PROCESSOR_ARCHITECTURE_INTEL:
            std::cout << "x86";
            break;
        default:
            std::cout << "unknown";
            break;
    }

    std::cout << std::endl;
    std::cout << "Page Size: " << system_info.dwPageSize << std::endl;
    std::cout << "Lowest Address: " << system_info.lpMinimumApplicationAddress << std::endl;
    std::cout << "Highest Address: " << system_info.lpMaximumApplicationAddress << std::endl;

    std::cout << "Active Processors: ";
    WORD mask = system_info.dwActiveProcessorMask;
    WORD res = 1;
    for (auto i = 0; i < 32; i++) {
        if (res & mask) {
            std::cout << i << " ";
        }

        res <<= 1;
    }

    std::cout << std::endl;
    std::cout << "Number Of Processors: " << system_info.dwNumberOfProcessors << std::endl;
    std::cout << "Processor Type: " << system_info.dwProcessorType << std::endl;
    std::cout << "Alloc Granularity: " << system_info.dwAllocationGranularity << std::endl;
    std::cout << "Processor Level: " << system_info.wProcessorLevel << std::endl;
    std::cout << "Processor Revision: " << system_info.wProcessorRevision << std::endl;
}

void OptGlobalMemoryStatus() {
    MEMORYSTATUS mem_status;
    GlobalMemoryStatus(&mem_status);

    std::cout << "Size Of Structure: " << mem_status.dwLength << std::endl;
    std::cout << "Memory Load: " << mem_status.dwMemoryLoad << std::endl;
    std::cout << "Total Phys: " << mem_status.dwTotalPhys << std::endl;
    std::cout << "Avail Phys: " << mem_status.dwAvailPhys << std::endl;
    std::cout << "Total Page: " << mem_status.dwTotalPageFile << std::endl;
    std::cout << "Avail Page: " << mem_status.dwAvailPageFile << std::endl;
    std::cout << "Total Virtual: " << mem_status.dwTotalVirtual << std::endl;
    std::cout << "Avail Virtual: " << mem_status.dwAvailVirtual << std::endl;
}

void OptVirtualQuery() {
    std::cout << "Enter the address: " << std::endl;

    size_t address;
    std::cin >> std::hex >> address;

    MEMORY_BASIC_INFORMATION mem_info;

    VirtualQuery((LPVOID)address, &mem_info, sizeof(mem_info));

    std::cout << "Base Address: " << mem_info.BaseAddress << std::endl;
    std::cout << "Alloc Base: " << mem_info.AllocationBase << std::endl;
    std::cout << "Alloc Protect: " << mem_info.AllocationProtect << std::endl;
    std::cout << "Region Size: " << mem_info.RegionSize << std::endl;

    std::cout << "State: ";
    switch (mem_info.State) {
        case MEM_COMMIT:
            std::cout << "MEM_COMMIT";
            break;
        case MEM_FREE:
            std::cout << "MEM_FREE";
            break;
        case MEM_RESERVE:
            std::cout << "MEM_RESERVE";
            break;
        default:
            std::cout << "unknown";
            break;
    }
    std::cout << std::endl;

    std::cout << "Protect: ";
    switch (mem_info.Protect) {
        case PAGE_NOACCESS:
            std::cout << "PAGE_NOACCESS";
            break;
        case PAGE_READONLY:
            std::cout << "PAGE_READONLY";
            break;
        case PAGE_READWRITE:
            std::cout << "PAGE_READWRITE";
            break;
        case PAGE_EXECUTE:
            std::cout << "PAGE_EXECUTE";
            break;
        case PAGE_GUARD:
            std::cout << "PAGE_GUARD";
            break;
        case PAGE_NOCACHE:
            std::cout << "PAGE_NOCACHE";
            break;
        case PAGE_WRITECOMBINE:
            std::cout << "PAGE_WRITECOMBINE";
            break;
        default:
            std::cout << "unknown";
            break;
    }
    std::cout << std::endl;

    std::cout << "Type: ";
    switch (mem_info.Type) {
        case MEM_IMAGE:
            std::cout << "MEM_IMAGE";
            break;
        case MEM_MAPPED:
            std::cout << "MEM_MAPPED";
            break;
        case MEM_PRIVATE:
            std::cout << "MEM_PRIVATE";
            break;
        default:
            std::cout << "unknown";
            break;
    }
    std::cout << std::endl;
}

void OptVirtualAlloc() {
    std::cout << "Enter the address: " << std::endl;

    size_t address;
    std::cin >> std::hex >> address;

    auto res = VirtualAlloc((LPVOID)address, blockSize,
                            MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!res) {
        std::cout << "Alloc Error!"<< std::endl;
    }
}

void OptVirtualAllocAuto() {
    auto res = VirtualAlloc(nullptr, blockSize,
                            MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!res) {
        std::cout << "Alloc Error!" << std::endl;
        return;
    }

    std::cout << "Address: " << res << std::endl;
}

void OptWrite() {
    std::cout << "Enter the address: " << std::endl;

    size_t address;
    int data;
    std::cin >> std::hex >> address;

    auto alloc = VirtualAlloc((LPVOID)address, sizeof(int),
                            MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!alloc) {
        std::cout << "Alloc Error!"<< std::endl;
        return;
    }

    std::cout << "Enter data to write (number): " << std::endl;
    std::cin >> data;
    alloc = (void*)data;
    std::cout << "Success!" << std::endl;
    std::cout << "Data written: " << (int*)alloc << std::endl;
}

void OptVirtualProtect() {
    std::cout << "Enter the address: " << std::endl;

    size_t address;
    std::cin >> std::hex >> address;

    DWORD old_protect;
    auto res = VirtualProtect((LPVOID)address, blockSize, PAGE_READONLY, &old_protect);

    if (!res) {
        std::cout << "Protect Error!" << std::endl;
        return;
    }

    std::cout << "Your data is protected" << std::endl;

    MEMORY_BASIC_INFORMATION mem_info;
    VirtualQuery((LPVOID)address, &mem_info, sizeof(mem_info));

    std::cout << "Protect: ";
    switch (mem_info.Protect) {
        case PAGE_NOACCESS:
            std::cout << "PAGE_NOACCESS";
            break;
        case PAGE_READONLY:
            std::cout << "PAGE_READONLY";
            break;
        case PAGE_READWRITE:
            std::cout << "PAGE_READWRITE";
            break;
        case PAGE_EXECUTE:
            std::cout << "PAGE_EXECUTE";
            break;
        case PAGE_GUARD:
            std::cout << "PAGE_GUARD";
            break;
        case PAGE_NOCACHE:
            std::cout << "PAGE_NOCACHE";
            break;
        case PAGE_WRITECOMBINE:
            std::cout << "PAGE_WRITECOMBINE";
            break;
        default:
            std::cout << "unknown";
            break;
    }
}

void OptVirtualFree() {
    std::cout << "Enter the address: " << std::endl;

    size_t address;
    std::cin >> std::hex >> address;

    auto res = VirtualFree((LPVOID)address, 0, MEM_RELEASE);
    if (!res) {
        std::cout << "Free Error!";
    }
}
