#include <iostream>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <chrono>

const int pageSize = 4096;
const int pageNum = 12;
const int blockSize = pageNum * pageSize;

int main() {
    HANDLE map = OpenFileMapping(FILE_MAP_READ, false, "map");
    LPVOID map_buffer = MapViewOfFile(map, FILE_MAP_READ, 0, 0, 0);

    HANDLE sem_page_empty[pageNum];
    HANDLE sem_page_full[pageNum];
    for (auto i = 0; i < pageNum; i++) {
        sem_page_empty[i] = OpenSemaphore(SEMAPHORE_ALL_ACCESS, false, ("sem_page" +
                                            std::to_string(i) + "_empty").c_str());

        sem_page_full[i] = OpenSemaphore(SEMAPHORE_ALL_ACCESS, false, ("sem_page" +
                                            std::to_string(i) + "_full").c_str());
    }

    HANDLE sem_empty = OpenSemaphore(SEMAPHORE_ALL_ACCESS, true, "sem_empty");
    HANDLE sem_full = OpenSemaphore(SEMAPHORE_ALL_ACCESS, true, "sem_full");
    HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, "mutex");

    VirtualLock(map_buffer, blockSize);

    int writer_id;
    std::cout << "Enter the reader id: " << std::endl;
    std::cin >> writer_id;

    std::ofstream log;
    log.open("reader" + std::to_string(writer_id) + "log.txt");

    std::chrono::time_point<std::chrono::system_clock> start_time;
    std::chrono::time_point<std::chrono::system_clock> stop_time;
    int page_pos;

    char text_buffer[pageSize];

    while (true) {
        start_time = std::chrono::system_clock::now();

        WaitForSingleObject(sem_full, INFINITE);
        page_pos = WaitForMultipleObjects(pageNum, sem_page_full, false, INFINITE);

        WaitForSingleObject(mutex, INFINITE);

        stop_time = std::chrono::system_clock::now();
        log << "Waiting: " <<
            std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count();
        log << " | ";

        start_time = std::chrono::system_clock::now();
        CopyMemory(text_buffer, (char*)map_buffer + page_pos * (pageSize + 4), pageSize);
        std::cout << text_buffer << std::endl;

        Sleep((rand() % 1000) + 500);
        stop_time = std::chrono::system_clock::now();
        log << "Reading " << page_pos + 1 << ": " <<
            std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count();
        log << " | ";

        start_time = std::chrono::system_clock::now();
        ReleaseMutex(mutex);
        ReleaseSemaphore(sem_page_empty[page_pos], 1, nullptr);
        ReleaseSemaphore(sem_empty, 1, nullptr);
        log << "Releasing: " <<
            std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count();
        log << std::endl;
    }

    return 0;
}
