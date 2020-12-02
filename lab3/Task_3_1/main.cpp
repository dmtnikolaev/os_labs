#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>
#include <atomic>
#include <windows.h>


#define N 100'000'000
#define BLOCK_SIZE 8'307'110

std::vector<int> threadsNum;
std::vector<DWORD> blocks;
std::vector<HANDLE> events;
bool terminate_flag;

std::atomic<double> pi;

typedef struct ThreadData {
    int block_start;
    HANDLE event;
} ThreadData, *pThreadData;

DWORD WINAPI ThreadFunction(LPVOID param) {
    auto data = static_cast<pThreadData>(param);
    int block_start;
    int block_end;
    double result;

    while (TRUE) {
        if (terminate_flag) {
            return 0;
        }
        block_start = data->block_start;
        block_end = block_start + BLOCK_SIZE;
        result = 0;

        for (auto i = block_start; i < block_end && i < N; i++) {
            result += 4 / (1 + (i + 0.5) / N * (i + 0.5) / N);
        }

        pi += result;

        SetEvent(data->event);
        SuspendThread(GetCurrentThread());
    }
}

int CreateThreads(int num, std::vector<HANDLE>& threads_hs,
                  std::vector<DWORD>& threads_ids, std::vector<pThreadData>& threads_data) {
    for (auto i = 0; i < num && i < blocks.size(); i++) {
        threads_data.push_back((pThreadData) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                                   sizeof(ThreadData)));
        threads_data[i]->block_start = blocks[i];
        threads_data[i]->event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        events.push_back(threads_data[i]->event);

        threads_ids.push_back(0);
        threads_hs.push_back(CreateThread(
                nullptr,
                0,
                ThreadFunction,
                threads_data[i],
                CREATE_SUSPENDED,
                &threads_ids[i]));
    }

    return 0;
}

int CalculatePi(int threads_num) {
    pi.store(0);
    terminate_flag = false;

    std::vector<HANDLE> threads_hs;
    std::vector<DWORD> threads_ids;
    std::vector<pThreadData> threads_data;

    CreateThreads(threads_num, threads_hs, threads_ids, threads_data);
    for (auto & threads_h : threads_hs) {
        ResumeThread(threads_h);
    }

    DWORD free_index;
    for (auto i = events.size(); i < blocks.size(); i++) {
        free_index = WaitForMultipleObjects(events.size(), events.data(), FALSE, INFINITE) - WAIT_OBJECT_0;
        threads_data[free_index]->block_start = blocks[i];
        while (ResumeThread(threads_hs[free_index]) == 0);
    }

    WaitForMultipleObjects(events.size(), events.data(), TRUE, INFINITE);

    terminate_flag = true;
    for (auto i = 0; i < events.size(); i++) {
        ResumeThread(threads_hs[i]);
        WaitForSingleObject(threads_hs[i], INFINITE);
        CloseHandle(events[i]);
        HeapFree(GetProcessHeap(), HEAP_ZERO_MEMORY, threads_data[i]);
    }

    events.clear();
    pi.store(pi.load() / N);
    return 0;
}

int main() {
    threadsNum = {1, 2, 4, 8, 12, 16};
    for (auto i = 0; i < N; i+=BLOCK_SIZE) {
        blocks.push_back(i);
    }

    double duration = 0;
    int n = 50;
    std::chrono::time_point<std::chrono::system_clock> start_time;
    std::chrono::time_point<std::chrono::system_clock> stop_time;

    for (int num : threadsNum) {
        std::cout << " --- " << num << " thread(s) --- " << std::endl;
        for (auto i = 0; i < n; i++) {
            start_time = std::chrono::system_clock::now();

            CalculatePi(num);

            stop_time = std::chrono::system_clock::now();

            if (i == 0) {
                std::cout << "pi = " << std::setprecision(13) << pi.load() << std::endl;
            }
            duration += std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count() / 1000.0;
        }

        duration /= n;
        std::cout << "time = " << duration << std::endl;
        std::cout << std::endl;
    }
    return 0;
}