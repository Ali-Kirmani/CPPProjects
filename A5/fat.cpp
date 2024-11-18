/// =========================================================================
/// Copyright (C) 2024 Jonathan Hudson (jwhudson@ucalgary.ca)
/// All Rights Reserved. Do not distribute this file.
/// =========================================================================
/// this is the only file you should modify and submit for grading

//Ali Kirmani 30115539
//CPSC 457 A5
//fat.cpp

#include "fat.h"
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <string>
#include <stdio.h>
#include <set>
#include <vector>
#include <iostream>

using namespace std;

std::vector<std::string> stringVector;

int checkConsistency( int blockSize, std::vector<DEntry> & files, std::vector<int> & fat)
{
	vector<int> blocks;
    for (const auto& file : files) {
    int blocksReq = 0;
    int remainingSize = file.size;

    while (remainingSize > 0) {
        blocksReq++;
        remainingSize -= blockSize;
    }

    blocks.push_back(blocksReq);
}
	
	int blocksAvailable = fat.size();
	vector<vector<int>> numUsing;
    bool cycle;
    set<int> indS;
	for (int i = 0; i < files.size(); i++)
	{
		int ind = files[i].ind;
        //Vector for pages used
		vector<int> used;
		//Add pages until -1
		while(ind != -1)
		{	
			//Check for cycle using pages used
			cycle = false;
            for (const auto& u : used) {
                if (u == ind) {
                    cycle = true;
                    break;
                }
            }
			//If cycle, set flag
			if(cycle)
			{
				files[i].hasCycle = true;
				break;
			}	
			for (int a = 0; a < numUsing.size(); a++) {
                if (a == i) continue; // Skip checking the current file against itself
                for (int block : numUsing[a]) {
                    if (ind == block) {
                        files[i].sharesBlocks = true;
                        files[a].sharesBlocks = true;
                        break; // No need to continue if a shared block is found
                    }
                }
            }
            //Insert index to list of indices
			indS.insert(ind);
			//Push index to used indices
			used.push_back(ind);
			int foo = ind;
			ind = fat[foo];	
		}

		//checks for blocks allocated
		files[i].tooFewBlocks = (blocks[i] > used.size());
        files[i].tooManyBlocks = (blocks[i] < used.size());
		numUsing.push_back(used);	
	}
	
	//Calc blocksAvailable
    blocksAvailable = blocksAvailable - indS.size();
    return blocksAvailable;
}
