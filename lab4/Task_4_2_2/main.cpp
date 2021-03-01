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

void OptCreateFile();
void OptReadFileEx();
VOID CALLBACK ReadRoutine(DWORD, DWORD, LPOVERLAPPED);

HANDLE namedPipe;

static const MenuOption MAIN_MENU[] =
        {
                {1, "CreateFile", OptCreateFile},
                {2, "ReadFileEx", OptReadFileEx},
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
    CloseHandle(namedPipe);
    return EXIT_SUCCESS;
}

void OptCreateFile() {
    namedPipe = CreateFile("\\\\.\\pipe\\LOCAL\\named", GENERIC_READ, 0, nullptr, OPEN_EXISTING,
                           FILE_FLAG_OVERLAPPED, nullptr);
    if (!namedPipe) {
        std::cout << "Error " << GetLastError() << std::endl;
    }
}

void OptReadFileEx() {
    OVERLAPPED overlap{.Internal = 0, .InternalHigh = 0, .Offset = 0, .OffsetHigh = 0};
    char buffer[blockSize];

    auto res = ReadFileEx(namedPipe, buffer, blockSize, &overlap, ReadRoutine);

    if (!res) {
        std::cout << "Error " << GetLastError() << std::endl;
        return;
    }

    SleepEx(INFINITE, true);
    std::cout << "Data: " << buffer << std::endl;
}

VOID CALLBACK ReadRoutine(DWORD, DWORD, LPOVERLAPPED) {

}