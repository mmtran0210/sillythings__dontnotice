/**
 * @file sloppySim.cc
 * 
 * Minh My Tran - CSCE 311 002 - Sloppy Counter Simulator
 * Program 1
 */

#include <iostream>
#include <pthread.h>
#include <vector>
#include <chrono>
#include <random>
#include <atomic>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

typedef struct __shared {
    std::atomic<int> global_counter = 0;
    std::vector<int> local_counters;
    int sloppiness = 10;
    int work_time = 10;
    int work_iterations= 100;
    bool cpu_bound = false;
    bool do_logging = false;
} shared;

void* work(void* arg) {
    shared* sh = (shared*) arg;
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(sh->work_time*0.5, sh->work_time*1.5);

    int local_counter = 0;
    for (int i = 0; i < sh->work_iterations; ++i) {
        int random_work_time = distribution(generator);
        if (sh->cpu_bound) {
            long increments = sh->work_time * 1e6; // Total number of increments
            // start timing
            auto start = std::chrono::high_resolution_clock::now();

            for (long j = 0; j < increments; ++j) {
                // Perform some work that takes approximately work_time milliseconds
            }
            // end timing
            auto finish = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = finish - start;
            std::cout << "Time spent on doing 1 billion increments: " << elapsed.count() << " seconds" << std::endl;
        } else {
            #ifdef _WIN32
        Sleep(random_work_time * 1000);
            #else
        usleep(random_work_time * 1000 * 1000); // usleep works with microseconds
    #endif

        }
        if (++local_counter % sh->sloppiness == 0) {
            sh->global_counter += sh->sloppiness;
            local_counter = 0;
        }
    }
    return NULL;
}

    void createThreads(std::vector<pthread_t>& threads, shared& sh) {
    for (int i = 0; i < threads.size(); ++i) {
        pthread_create(&threads[i], NULL, work, &sh);
    }
}

void joinThreads(std::vector<pthread_t>& threads) {
    for (auto& t : threads) {
        pthread_join(t, NULL);
    }
}

void Do_Logging(shared& sh, int n_threads) {
    std::cout << "N_Threads = " << n_threads << std::endl;
    std::cout << "Sloppiness = " << sh.sloppiness << std::endl;
    std::cout << "Work Time = " << sh.work_time << std::endl;
    std::cout << "Work Iterations =  " << sh.work_iterations << std::endl;
    std::cout << "CPU Bound =  " << (sh.cpu_bound ? "true" : "false") << std::endl;
    std::cout << "Do Logging =  " << (sh.do_logging ? "true" : "false") << std::endl;

    for (int i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(sh.work_time * sh.work_iterations / 10));
        std::cout << "Global counter = " << sh.global_counter << std::endl;
        std::cout << "Local counters = [";
        for (int j = 0; j < sh.local_counters.size(); ++j) {
            std::cout << sh.local_counters[j];
            if (j != sh.local_counters.size() - 1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
    }

    std::cout << "Final global counter = " << sh.global_counter << std::endl;
} 

int main(int argc, char* argv[]) {
    shared sh;

    int n_threads = 2; // Default number of threads
    if (argc > 1) n_threads = std::stoi(argv[1]); // If provided, update the number of threads
    if (argc > 2) sh.sloppiness = std::stoi(argv[2]); // If provided, update the sloppiness
    if (argc > 3) {
        sh.work_time = std::stoi(argv[3]); // If provided, update the work time
        if (argc > 4) {
            sh.work_iterations = std::stoi(argv[4]); // If provided, update the work iterations
            if (argc > 5) {
                sh.cpu_bound = std::string(argv[5]) == "true"; // If provided, update the CPU bound
                if (argc > 6) sh.do_logging = std::string(argv[6]) == "true"; // If provided, update the logging
            }
        }
    }

    std::vector<pthread_t> threads(n_threads);
    createThreads(threads, sh);

    if (sh.do_logging) {
        log(sh, n_threads);
    }

    joinThreads(threads);

    return 0;

}
