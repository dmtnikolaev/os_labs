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

void OptCreateNamedPipe();
void OptConnectNamedPipe();
void OptDisconnectNamedPipe();
void OptCreateEvent();
void OptWriteFile();
void OptWaitForSingleObject();

HANDLE namedPipe;
HANDLE event;

static const MenuOption MAIN_MENU[] =
        {
                {1, "CreateNamedPipe", OptCreateNamedPipe},
                {2, "ConnectNamedPipe", OptConnectNamedPipe},
                {3, "DisconnectNamedPipe", OptDisconnectNamedPipe},
                {4, "CreateEvent", OptCreateEvent},
                {5, "WriteFile", OptWriteFile},
                {6, "WaitForSingleObject", OptWaitForSingleObject},
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

void OptCreateNamedPipe() {
    namedPipe = CreateNamedPipe("\\\\.\\pipe\\LOCAL\\named", PIPE_ACCESS_OUTBOUND,
                                PIPE_TYPE_MESSAGE |
                                PIPE_WAIT | PIPE_READMODE_MESSAGE, PIPE_UNLIMITED_INSTANCES,
                                blockSize, blockSize, 0, nullptr);
    if (!namedPipe) {
        std::cout << "Error " << GetLastError() << std::endl;
    }
}

void OptConnectNamedPipe() {
    auto res = ConnectNamedPipe(namedPipe, nullptr);

    if (!res) {
        std::cout << "Error " << GetLastError() << std::endl;
    }
}

void OptDisconnectNamedPipe() {
    auto res = DisconnectNamedPipe(namedPipe);

    if (!res) {
        std::cout << "Error!" << std::endl;
        return;
    }

    CloseHandle(namedPipe);
}

void OptCreateEvent() {
    event = CreateEvent(nullptr, false, false, "event");

    if (!event) {
        std::cout << "Error!" << std::endl;
    }
}

void OptWriteFile() {
    OVERLAPPED overlap{.Internal = 0, .InternalHigh = 0, .Offset = 0,
            .OffsetHigh = 0, .hEvent = event};

    char buffer[blockSize];
    std::cout << "Input data: " << std::endl;

    int i;
    getchar();
    for (i = 0; i < blockSize - 1; i++) {
        buffer[i] = (char) getchar();

        if (buffer[i] == '\n') {
            break;
        }
    }
    buffer[i] = '\0';

    WriteFile(namedPipe, buffer, blockSize, nullptr, &overlap);
}

void OptWaitForSingleObject() {
    WaitForSingleObject(event, INFINITE);
    std::cout << "Successful writing" << std::endl;
    CloseHandle(event);
}