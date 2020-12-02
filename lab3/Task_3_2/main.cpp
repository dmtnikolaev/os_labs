#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>
#include <omp.h>

#define N 100'000'000
#define BLOCK_SIZE 8'307'110

std::vector<int> threadsNum;

double CalculatePiIteration(int i) {
    return 4 / (1 + (i + 0.5) / N * (i + 0.5) / N) ;
}

double CalculatePi() {
    int i;
    double pi = 0;

#pragma omp parallel for private(i) schedule(dynamic, BLOCK_SIZE) reduction(+:pi)
    for (i = 0; i < N; i++) {
        pi += CalculatePiIteration(i);
    }

    pi /= N;

    return pi;
}

int main() {
    threadsNum = {1, 2, 4, 8, 12, 16};
    double duration = 0;
    int n = 50;
    std::chrono::time_point<std::chrono::system_clock> start_time;
    std::chrono::time_point<std::chrono::system_clock> stop_time;

    for (int num : threadsNum) {
        std::cout << " --- " << num << " thread(s) --- " << std::endl;
        omp_set_num_threads(num);
        for (auto i = 0; i < n; i++) {
            start_time = std::chrono::system_clock::now();

            CalculatePi();

            stop_time = std::chrono::system_clock::now();

            if (i == 0) {
                std::cout << "pi = " << std::setprecision(13) << CalculatePi() << std::endl;
            }
            duration += std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count() / 1000.0;
        }

        duration /= n;
        std::cout << "time = " << duration << std::endl;
        std::cout << std::endl;
    }
    return 0;
}