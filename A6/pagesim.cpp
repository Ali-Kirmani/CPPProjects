/// =========================================================================
/// Copyright (C) 2024 Jonathan Hudson (jwhudson@ucalgary.ca)
/// All Rights Reserved. Do not distribute this file.
/// =========================================================================
/// this is the only file you should modify and submit for grading

//Ali Kirmani 30115539
//CPSC 457 A6


#include "pagesim.h"
#include <unordered_map>
#include <limits>
#include <algorithm>
#include <list>
#include <iostream>
#include <vector>
using namespace std;

int optimal(int frame_count, std::vector<int> &ref, std::vector<int> &frames) {
    int page_faults = 0;
    std::unordered_map<int, int> next_use; // Map to store the next use index of each page

    // Initialize next_use map with -1 indicating page never used again
    for (int i = 0; i < frame_count; ++i) {
        next_use[frames[i]] = std::numeric_limits<int>::max();
    }

    for (int i = 0; i < ref.size(); ++i) {
        int page = ref[i];

        // Check if page is already in frames
        auto it = find(frames.begin(), frames.end(), page);

        // If page is not in frames, it's a page fault
        if (it == frames.end()) {
            ++page_faults;

            // If there is an empty frame, place the page in it
            auto empty_frame = find(frames.begin(), frames.end(), -1);
            if (empty_frame != frames.end()) {
                *empty_frame = page;
            } else {
                // Find the page that will not be used for the longest period in the future
                int farthest_index = -1;
                int farthest_page = -1;
                for (int j = 0; j < frame_count; ++j) {
                    int next_use_index = next_use[frames[j]];
                    if (next_use_index == std::numeric_limits<int>::max()) {
                        // Page never used again, so it will not be used for the longest period
                        farthest_page = frames[j];
                        break;
                    }
                    if (next_use_index > farthest_index) {
                        farthest_index = next_use_index;
                        farthest_page = frames[j];
                    }
                }
                // Replace the farthest page with the current page
                replace(frames.begin(), frames.end(), farthest_page, page);
            }
        }

        // Update the next use index of the current page
        next_use[page] = std::numeric_limits<int>::max();
        for (int j = i + 1; j < ref.size(); ++j) {
            if (ref[j] == page) {
                next_use[page] = j;
                break;
            }
        }
    }

    return page_faults;
}

int lru(int frame_count, std::vector<int> &ref, std::vector<int> &frames) {
    unordered_map<int, int> frame_positions;
    list<int> lru_order;
    int page_faults = 0;

    for (int page : ref) {
        auto it = find(frames.begin(), frames.end(), page);

        if (it != frames.end()) {
            lru_order.remove(page);
            lru_order.push_front(page);
        } else {
            page_faults++;

            if (lru_order.size() == frame_count) {
                auto lru_page_it = prev(lru_order.end());
                int lru_page = *lru_page_it;
                lru_order.erase(lru_page_it);

                auto frame_iter = find(frames.begin(), frames.end(), lru_page);
                *frame_iter = page;

                frame_positions.erase(lru_page);
                frame_positions.emplace(page, distance(frames.begin(), frame_iter));
            } else {
                auto pos = find(frames.begin(), frames.end(), -1);
                *pos = page;
                frame_positions.emplace(page, distance(frames.begin(), pos));
            }

            lru_order.push_front(page);
        }
    }

    return page_faults;
}

int clock(int frame_count, std::vector<int> &ref, std::vector<int> &frames) {
    int page_faults = 0;
    int hand = 0; // Clock hand pointing to the frame to be replaced
    std::vector<bool> referenced(frame_count, false); // Reference bit for each frame

    for (int i = 0; i < ref.size(); ++i) {
        int page = ref[i];

        // Check if page is already in frames
        auto it = std::find(frames.begin(), frames.end(), page);

        // If page is not in frames, it's a page fault
        if (it == frames.end()) {
            ++page_faults;

            // If there is an empty frame, place the page in it
            if (frames.size() < frame_count) {
                frames.push_back(page);
                referenced.push_back(true); // New page is referenced
            } else {
                // Find the first frame with reference bit 0
                while (referenced[hand]) {
                    referenced[hand] = false; // Reset reference bit
                    hand = (hand + 1) % frame_count; // Move hand
                }
                // Replace the frame pointed by the hand with the current page
                frames[hand] = page;
                referenced[hand] = true; // New page is referenced
                hand = (hand + 1) % frame_count; // Move hand
            }
        } else {
            // If page is already in frames, set its reference bit
            int index = std::distance(frames.begin(), it);
            referenced[index] = true;
        }
    }

    return page_faults;
}
