#include <iostream>
#include <windows.h>
#include <vector>
#include <chrono>

OVERLAPPED* overlapsRead;
OVERLAPPED* overlapsWrite;
CHAR** buffers;

LARGE_INTEGER fileSize;
HANDLE original, copy;

const auto iterations = 1;
const auto blockSize = 4096;
const int opsCount[] = {1, 2, 4, 8, 12, 16};
auto ops_count_index = 0;

std::atomic<int> doneCount;

VOID WINAPI ReadRoutine(DWORD, DWORD, LPOVERLAPPED);
VOID WINAPI WriteRoutine(DWORD, DWORD, LPOVERLAPPED);

int main() {
    while (ops_count_index < sizeof(opsCount) / sizeof(*opsCount)) {
        std::chrono::time_point<std::chrono::system_clock> start_time;
        std::chrono::time_point<std::chrono::system_clock> stop_time;
        double duration = 0;

        for (auto it = 0; it < iterations; it++) {
            original = CreateFile("C:\\test\\test.txt", GENERIC_READ, 0, nullptr, OPEN_EXISTING,
                                  FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, nullptr);

            std::string target = "C:\\test\\test_copy";
            target.push_back(ops_count_index + '0');
            target += ".txt";

            copy = CreateFile(target.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                              FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, nullptr);

            LARGE_INTEGER cur_pos_read;
            cur_pos_read.QuadPart = 0;

            GetFileSizeEx(original, &fileSize);

            long long blocks_num = (fileSize.QuadPart + blockSize - 1) / blockSize;
            overlapsRead = new OVERLAPPED[opsCount[ops_count_index]];
            overlapsWrite = new OVERLAPPED[opsCount[ops_count_index]];

            buffers = new CHAR * [opsCount[ops_count_index]];
            for (int i = 0; i < opsCount[ops_count_index]; i++) {
                buffers[i] = new CHAR[blockSize];
            }

            doneCount.store(0);

            start_time = std::chrono::system_clock::now();

            for (auto i = 0; i < opsCount[ops_count_index]; i++) {
                overlapsRead[i].hEvent = (HANDLE) i;
                overlapsWrite[i].hEvent = (HANDLE) i;

                overlapsRead[i].Offset = cur_pos_read.LowPart;
                overlapsRead[i].OffsetHigh = cur_pos_read.HighPart;

                if (cur_pos_read.QuadPart < fileSize.QuadPart) {
                    ReadFileEx(original, buffers[i], blockSize, &overlapsRead[i], ReadRoutine);
                }

                cur_pos_read.QuadPart += (LONGLONG) blockSize;
            }

            while (doneCount.load() < 2 * blocks_num) {
                SleepEx(INFINITE, true);
            }

            stop_time = std::chrono::system_clock::now();
            duration += std::chrono::duration_cast<std::chrono::milliseconds>(stop_time -
                    start_time).count() / 1000.0;

            LARGE_INTEGER eof;
            eof.QuadPart = fileSize.QuadPart;
            SetFilePointerEx(copy, eof, nullptr, FILE_BEGIN);
            SetEndOfFile(copy);

            CloseHandle(copy);
            CloseHandle(original);
            //DeleteFile(target.c_str());

            delete[] overlapsWrite;
            delete[] overlapsRead;
            for (int i = 0; i < opsCount[ops_count_index]; i++) {
                delete[] buffers[i];
            }
            delete[] buffers;
        }
        std::cout << duration / iterations << " s" << std::endl;
        ops_count_index++;
    }
    return 0;
}

VOID WINAPI ReadRoutine(DWORD dw_err, DWORD bytes_read, LPOVERLAPPED lp_overlap) {
    LARGE_INTEGER cur_pos_read, cur_pos_write;

    cur_pos_read.LowPart = lp_overlap->Offset;
    cur_pos_read.HighPart = lp_overlap->OffsetHigh;

    cur_pos_write.QuadPart = cur_pos_read.QuadPart;

    auto i = (DWORD)(lp_overlap->hEvent);
    overlapsWrite[i].Offset = cur_pos_write.LowPart;
    overlapsWrite[i].OffsetHigh = cur_pos_write.HighPart;

    WriteFileEx(copy, buffers[i], blockSize, &overlapsWrite[i], WriteRoutine);

    cur_pos_read.QuadPart += blockSize * (LONGLONG)opsCount[ops_count_index];
    overlapsRead[i].Offset = cur_pos_read.LowPart;
    overlapsRead[i].OffsetHigh = cur_pos_read.HighPart;

    doneCount.operator++();
}

VOID WINAPI WriteRoutine(DWORD dw_err, DWORD bytes_read, LPOVERLAPPED lp_overlap) {
    LARGE_INTEGER cur_pos_read;

    auto i = (DWORD)(lp_overlap->hEvent);

    cur_pos_read.LowPart = overlapsRead[i].Offset;
    cur_pos_read.HighPart = overlapsRead[i].OffsetHigh;

    if (cur_pos_read.QuadPart < fileSize.QuadPart) {
        ReadFileEx(original, buffers[i], blockSize, &overlapsRead[i], ReadRoutine);
    }

    doneCount.operator++();
}
