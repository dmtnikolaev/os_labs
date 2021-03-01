#include <iostream>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <chrono>

const int pageSize = 4096;
const int pageNum = 12;
const int blockSize = pageNum * pageSize;

int main() {


    HANDLE file = CreateFile("out.txt", GENERIC_READ | GENERIC_WRITE,
                             FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_ALWAYS,
                             FILE_ATTRIBUTE_NORMAL, nullptr);
    HANDLE map = CreateFileMapping(file, nullptr, PAGE_READWRITE, 0, blockSize, "map");
    LPVOID map_buffer = MapViewOfFile(map, FILE_MAP_WRITE, 0, 0, blockSize);

    HANDLE sem_page_empty[pageNum];
    HANDLE sem_page_full[pageNum];
    for (auto i = 0; i < pageNum; i++) {
        sem_page_empty[i] = CreateSemaphore(nullptr, 1, 1, ("sem_page"+ std::to_string(i) +
                                                            "_empty").c_str());

        sem_page_full[i] = CreateSemaphore(nullptr, 0, 1, ("sem_page"+ std::to_string(i) +
                                                            "_full").c_str());
    }

    HANDLE sem_empty = CreateSemaphore(nullptr, pageNum, pageNum, "sem_empty");
    HANDLE sem_full = CreateSemaphore(nullptr, 0, pageNum, "sem_full");
    HANDLE mutex = CreateMutex(nullptr, FALSE, "mutex");

    VirtualLock(map_buffer, blockSize);

    int writer_id;
    std::cout << "Enter the writer id: " << std::endl;
    std::cin >> writer_id;

    std::ofstream log;
    log.open("writer" + std::to_string(writer_id) + "log.txt");

    std::chrono::time_point<std::chrono::system_clock> start_time;
    std::chrono::time_point<std::chrono::system_clock> stop_time;
    int page_pos;

    std::ifstream text;
    text.open("in.txt");
    char text_buffer[pageSize];

    while (true) {
        start_time = std::chrono::system_clock::now();

        WaitForSingleObject(sem_empty, INFINITE);
        page_pos = WaitForMultipleObjects(pageNum, sem_page_empty, false, INFINITE);

        WaitForSingleObject(mutex, INFINITE);

        stop_time = std::chrono::system_clock::now();
        log << "Waiting: " <<
            std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count();
        log << " | ";

        start_time = std::chrono::system_clock::now();
        text.getline((char*)map_buffer + page_pos * (pageSize + 4), pageSize).eof();

        Sleep((rand() % 1000) + 500);
        stop_time = std::chrono::system_clock::now();
        log << "Writing " << page_pos + 1 << ": " <<
            std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count();
        log << " | ";

        start_time = std::chrono::system_clock::now();
        ReleaseMutex(mutex);
        ReleaseSemaphore(sem_page_full[page_pos], 1, nullptr);
        ReleaseSemaphore(sem_full, 1, nullptr);
        log << "Releasing: " <<
            std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count();
        log << std::endl;
    }

    return 0;
}
