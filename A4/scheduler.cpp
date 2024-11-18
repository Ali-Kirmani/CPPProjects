//CPSC 457
//Ali Kirmani
//Assignment 4 Question 2 Scheudling
//I used code from the TA Amir that he posted to D2L and based my code off of it

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include "scheduler.h"
#include <limits>

using namespace std;

// Function to simulate Round Robin scheduling
void simulate_rr(int64_t quantum, int64_t max_seq_len, std::vector<Process>& processes, std::vector<int>& seq) {
    seq.clear();
    int n = processes.size();
    queue<int> q; // Queue to store the process indexes
    vector<int> arrival_time(n);
    for (int i = 0; i < n; ++i) {
        arrival_time[i] = processes[i].arrival;
    }
    int64_t current_time = 0;
    int completed = 0;
    vector<int> order; // To store the order of execution
    vector<int> waiting_time(n, 0); // To store waiting time for each process
    int total_waiting_time = 0; // Total waiting time
    vector<int> remaining_burst(processes.size());

    // Initialize remaining burst times
    for (int i = 0; i < n; ++i) {
        remaining_burst[i] = processes[i].burst;
    }

    while (completed != n) {
        for (int i = 0; i < n; i++) {
            if (processes[i].arrival <= current_time && find(order.begin(), order.end(), i) == order.end()) {
                q.push(i);
                order.push_back(i);
            }
        }

        if (!q.empty()) {//Non Idle CPU
            int idx = q.front();
            q.pop();

            seq.push_back(processes[idx].id);
            if (processes[idx].start_time == -1) {
                processes[idx].start_time = current_time;
                // Push the process ID into seq when it starts execution
                //seq.push_back(processes[idx].id);
                // Calculate waiting time for the process
                waiting_time[idx] = current_time - processes[idx].arrival;
            }

            if (remaining_burst[idx] > 0) {
                if (remaining_burst[idx] > quantum) {
                    current_time += quantum;
                    remaining_burst[idx] -= quantum;
                    // Update waiting time for processes in the queue
                    for (int i = 0; i < n; i++) {
                        if (i != idx && processes[i].arrival <= current_time && remaining_burst[i] > 0 && find(order.begin(), order.end(), i) != order.end()) {
                            waiting_time[i] += quantum;
                        }
                    }
                } else {
                    current_time += remaining_burst[idx];
                    processes[idx].finish_time = current_time;
                    // Update waiting time for processes in the queue
                    for (int i = 0; i < n; i++) {
                        if (i != idx && processes[i].arrival <= current_time && remaining_burst[i] > 0 && find(order.begin(), order.end(), i) != order.end()) {
                            waiting_time[i] += remaining_burst[idx];
                        }
                    }
                    remaining_burst[idx] = 0;
                    completed++;
                }
            }

            for (int i = 0; i < n; i++) {
                if (processes[i].arrival < current_time && remaining_burst[i] > 0 && find(order.begin(), order.end(), i) == order.end()) {
                    q.push(i);
                    order.push_back(i);
                }
            }

            if (remaining_burst[idx] > 0) {
                q.push(idx); // Re-add the process to the queue if it's not completed
            }
        } else {
            seq.push_back(-1);
            current_time++;
        }
    }

    // Remove duplicates from the order vector
        auto last = std::unique(seq.begin(), seq.end());
        seq.erase(last, seq.end());

    seq.resize(std::min(max_seq_len, int64_t(seq.size())));

    // Calculate and print waiting time for each process
    for (int i = 0; i < n; i++) {
        cout << "P" << i << " waited " << waiting_time[i] << " secs" << endl;
        total_waiting_time += waiting_time[i];
    }

    // Print average waiting time
    double avg_waiting_time = static_cast<double>(total_waiting_time) / n;
    cout << "Average waiting time " << avg_waiting_time << " secs" << endl;
}




void simulate_sjf(int64_t max_seq_len, std::vector<Process>& processes, std::vector<int>& seq) {
    seq.clear();
    int n = processes.size();
    int currentTime = 0;
    int completed = 0;
    vector<int> order; // To store the order of execution

    vector<bool> completedList(n);
    vector<int> waiting_time(n);
    vector<int> turnAround_time(n);
    // Initialize remaining burst times
    for (int i = 0; i < n; ++i) {
        completedList[i] = false;
    }

    while(completed != n) {
        int idx = -1;
        int minBt = numeric_limits<int>::max();

        for(int i = 0; i < n; i++) {
            if(processes[i].arrival <= currentTime && !completedList[i]) {
                if(processes[i].burst < minBt) {
                    minBt = processes[i].burst;
                    idx = i;
                }
                if(processes[i].burst == minBt) {
                    if(processes[i].arrival < processes[idx].arrival) {
                        idx = i;
                    }
                }
            }
        }

        if(idx != -1) {
            seq.push_back(processes[idx].id);
            processes[idx].start_time = currentTime; // Move this line here to avoid updating start time if process not chosen
            currentTime += processes[idx].burst;
            processes[idx].finish_time = currentTime;
            turnAround_time[idx] = processes[idx].finish_time - processes[idx].arrival;
            waiting_time[idx] = turnAround_time[idx] - processes[idx].burst;
            completedList[idx] = true;
            completed++;
            order.push_back(processes[idx].id);
        } else {
            seq.push_back(-1);
            currentTime++;
        }
    }

    // Print waiting times for each process
    int totalWaitingTime = 0;
    for (int i = 0; i < n; i++) {
        totalWaitingTime += waiting_time[i];
        cout << "P" << i << " waited " << waiting_time[i] << " secs" << endl;
    }

    // Calculate and print average waiting time
    double avgWaitingTime = static_cast<double>(totalWaitingTime) / n;
    cout << "Average waiting time: " << avgWaitingTime << " secs" << endl;
}