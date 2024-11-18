//Ali Kirmani
//CPSC 457
//A3 Q3
//Dr Hudson

#include "detectPrimes.h"
#include <pthread.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <atomic>
#include <unordered_map>
#include <vector>

using namespace std;

// =============================================================================
// Global Variables
// =============================================================================
pthread_barrier_t barrier;
int64_t n_threads;
atomic<bool> is_prime;
std::vector<int64_t> numbers;
std::vector<int64_t> result;
int64_t bottom;
int64_t top;
int64_t partition;
bool finished;
int num_index;
int64_t n;
bool threadBool;
unordered_map<int64_t, bool> cache;
// =============================================================================
// Function Declarations
// =============================================================================
static bool is_prime_serial(int64_t n);
void threadSkip(bool is);
void* primeCheck(void* id);
std::vector<int64_t> detect_primes(const std::vector<int64_t>& numbers, int n_threads);

// =============================================================================
// Function Definitions
// =============================================================================
static bool is_prime_serial(int64_t n) {
    // handle trivial cases
    if (n < 2) return false;
    if (n <= 3) return true; // 2 and 3 are primes
    if (n % 2 == 0) return false; // handle multiples of 2
    if (n % 3 == 0) return false; // handle multiples of 3
    // try to divide n by every number 5 .. sqrt(n)
    int64_t i = 5;
    int64_t max = sqrt(n);
    while (i <= max) {
        if (n % i == 0) return false;
        if (n % (i + 2) == 0) return false;
        i += 6;
    }
    // didn't find any divisors, so it must be a prime
    return true;
}

void threadSkip(bool is) {
    if (!threadBool) return;
    
    threadBool = false;
    is_prime.store(is, memory_order_release);
}

void* primeCheck(void* id) {
    int thread_id = long(id);

    while (true) {
        // Serial #1
        int firstReturn = pthread_barrier_wait(&barrier);
        if (firstReturn == PTHREAD_BARRIER_SERIAL_THREAD) {
            if (num_index >= (int)numbers.size()) {
                finished = true;
            } else {
                n = numbers[num_index];
                threadBool = true;
                is_prime.store(true, memory_order_release);

                if (threadBool) {
                    if (n < 2) threadSkip(false);
                    if (n <= 3) threadSkip(true);
                    if (n % 2 == 0) threadSkip(false);
                    if (n % 3 == 0) threadSkip(false);
                }

                if (threadBool) {
                    auto search = cache.find(n);
                    if (search != cache.end()) {
                        threadSkip(cache[n]);
                    }
                }

                if (threadBool) {
                    bottom = 5;
                    top = sqrt(n);
                    partition = ceil(double(top - bottom) / n_threads);

                    if (partition <= 6) {
                        bool is_prime = is_prime_serial(n);
                        threadSkip(is_prime);
                    } else {
                        partition = partition - (partition % 6) + 6;
                    }
                }

                if (threadBool) {
                    if (top < bottom) {
                        threadSkip(true); // Prime, but small numbers, like 13, 17, 19
                    } else if (top == bottom || n % top == 0 || n % (top + 2) == 0) {
                        threadSkip(false); // It's 25, not prime or top bound is the divisor
                    }
                }

                num_index++;
            }
        }

        pthread_barrier_wait(&barrier);
        if (finished) {
            break;
        } else if (threadBool) {
            int64_t start = bottom + partition * thread_id;
            int64_t end = start + partition;

            start = (start > top) ? top : start;
            start = (start < bottom) ? bottom : start;
            end = (end > top) ? top : end;

            int64_t t = start;
            while (t <= end && is_prime.load()) {
                if (n % t == 0 || n % (t + 2) == 0) {
                    is_prime.store(false, memory_order_release);
                    break;
                }
                t += 6;
            }
        }

        pthread_barrier_wait(&barrier);
        if (thread_id == 0) {
            bool is_prime_value = is_prime.load();
            if (is_prime_value) {
                result.push_back(n);
            }
            cache[n] = is_prime_value;
        }
    }

    pthread_exit(0);
}

std::vector<int64_t> detect_primes(const std::vector<int64_t>& numbers, int n_threads) {
    if (n_threads == 1) { 
        for (auto num : numbers) {
            if (is_prime_serial(num)) {
                result.push_back(num);
            }
        }
    } else { 
        ::numbers = numbers;
        num_index = 0;
        ::n_threads = n_threads;
        finished = false;

        pthread_barrier_init(&barrier, nullptr, n_threads);
        pthread_t threads[n_threads];

        // Init threads
        for (int t = 0; t < n_threads; t++) {
            pthread_create(&threads[t], nullptr, primeCheck, reinterpret_cast<void*>(static_cast<long>(t)));
        }

        // Join threads
        for (int t = 0; t < n_threads; t++) {
            pthread_join(threads[t], nullptr);
        }

        // Get rid of barrier
        pthread_barrier_destroy(&barrier);
    }

    return result;
}
