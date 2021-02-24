#include <iostream>
#include <windows.h>

const int sizeMax = 256;

typedef void (*MenuProcessingFunctionPointer)();

struct MenuOption
{
    int choice;
    char const * p_selection_text;
    MenuProcessingFunctionPointer p_processing_function;
};

void OptOpenFileMapping();
void OptMapViewOfFile();
void OptUnmapViewOfFile();
void OptRead();

static const MenuOption MAIN_MENU[] =
        {
                {1, "OpenFileMapping", OptOpenFileMapping},
                {2, "MapViewOfFile", OptMapViewOfFile},
                {3, "UnmapViewOfFile", OptUnmapViewOfFile},
                {4, "Read", OptRead},
        };

HANDLE file;
HANDLE map;
PCHAR buffer;

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

void OptOpenFileMapping() {
    map = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "TESTPAGE");

    if (!map) {
        std::cout << "Error!" << std::endl;
    }
}

void OptMapViewOfFile() {
    buffer = (PCHAR)MapViewOfFile(map, FILE_MAP_ALL_ACCESS, 0,
                                  0, sizeMax);

    if (!buffer) {
        std::cout << "Error!" << std::endl;
    }
}

void OptUnmapViewOfFile() {
    auto res = UnmapViewOfFile(buffer);

    if (!res) {
        std::cout << "Error!" << std::endl;
    }
}

void OptRead() {
    std::cout << buffer << std::endl;
}
