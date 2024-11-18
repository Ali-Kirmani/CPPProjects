// ======================================================================
// You must modify this file and then submit it for grading to D2L.
// ======================================================================
//
// count_pixels() counts the number of pixels that fall into a circle
// using the algorithm explained here:
//
// https://en.wikipedia.org/wiki/Approximations_of_%CF%80
//
// count_pixels() takes 2 paramters:
//  r         =  the radius of the circle
//  n_threads =  the number of threads you should create
//
// Currently the function ignores the n_threads parameter. Your job is to
// parallelize the function so that it uses n_threads threads to do
// the computation.

//Ali Kirmani 30115549
//CPSC 457
//A3 Q1
//Dr Hudson

#include "calcpi.h"
#include <thread>
#include <vector>
#include <cmath>
using namespace std;

uint64_t count_pixels(int r, int n_threads) {
    // Vector to hold the threads
    std::vector<std::thread> threads;

    // Vector to hold the count of pixels within the circle for each thread
    std::vector<uint64_t> thread_counts(n_threads);

    // Function to count pixels within the circle for a given range of pixels
    auto count_pixels_in_range = [&](double start_x, double end_x, int thread_id) {
        uint64_t count = 0;
        // Iterate over the assigned range of pixels
        for (double x = start_x; x < end_x; ++x) {
            for (double y = 1; y <= r; ++y) {
                if (x * x + y * y <= r * r) {
                    count++;
                }
            }
        }
        // Store the count in the corresponding index of the thread_counts vector
        thread_counts[thread_id] = count;
    };

    // Create and start threads
    for (int i = 0; i < n_threads; ++i) {
        // Calculate the range of x-values for this thread
        double start_x = i * ceil((r + 1)/ n_threads);
        double end_x = min((i + 1) * ceil((r + 1)/ n_threads), double(r + 1)); // Adjust end_x to ensure correct distribution
        threads.emplace_back(count_pixels_in_range, start_x, end_x, i);
    }

    // Join threads
    for (auto& thread : threads) {
        thread.join();
    }

    // Sum up the counts obtained by each thread
    uint64_t total_count = 0;
    for (auto count : thread_counts) {
        total_count += count;
    }
    // Multiply the total count by 4 to get the total number of pixels within the circle
    return total_count * 4 + 1;
}
