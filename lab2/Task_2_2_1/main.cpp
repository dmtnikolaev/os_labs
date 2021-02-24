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

void OptCreateFile();
void OptCreateFileMapping();
void OptMapViewOfFile();
void OptUnmapViewOfFile();
void OptWrite();

static const MenuOption MAIN_MENU[] =
        {
                {1, "CreateFile", OptCreateFile},
                {2, "CreateFileMapping", OptCreateFileMapping},
                {3, "MapViewOfFile", OptMapViewOfFile},
                {4, "UnmapViewOfFile", OptUnmapViewOfFile},
                {5, "Write", OptWrite},
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

void OptCreateFile() {
    file = CreateFile("C:\\test\\test.txt", GENERIC_READ | GENERIC_WRITE,
                                   FILE_SHARE_READ | FILE_SHARE_WRITE,
                                   nullptr, CREATE_NEW,
                                   FILE_ATTRIBUTE_NORMAL, nullptr);

    if (!file) {
        std::cout << "Error!" << std::endl;
    }
}

void OptCreateFileMapping() {
    map = CreateFileMapping(file, nullptr, PAGE_READWRITE, 0, sizeMax, "TESTPAGE");

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

void OptWrite() {
    getchar();

    char c;
    int i;
    for (i = 0; i < sizeMax - 1; i++) {
        c = (char)getchar();

        if (c == '\n') {
            break;
        }

        buffer[i] = c;
    }

    buffer[i] = '\0';
}
