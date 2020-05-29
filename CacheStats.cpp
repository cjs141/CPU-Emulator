/******************************
 * Submitted by: enter your first and last name and net ID
 * CS 3339 - Spring 2020, Texas State University
 * Project 5 Data Cache
 * Copyright 2020, all rights reserved
 * Updated by Lee B. Hinkle based on prior work by Martin Burtscher and Molly O'Neil
 ******************************/
#include <iostream>
#include <cstdlib>
#include <iomanip>
#include "CacheStats.h"
using namespace std;

CacheStats::CacheStats() {
  cout << "Cache Config: ";
  if(!CACHE_EN) {
    cout << "cache disabled" << endl;
  } else {
    cout << (SETS * WAYS * BLOCKSIZE) << " B (";
    cout << BLOCKSIZE << " bytes/block, " << SETS << " sets, " << WAYS << " ways)" << endl;
    cout << "  Latencies: Lookup = " << LOOKUP_LATENCY << " cycles, ";
    cout << "Read = " << READ_LATENCY << " cycles, ";
    cout << "Write = " << WRITE_LATENCY << " cycles" << endl;
  }

  loads = 0;
  stores = 0;
  load_misses = 0;
  store_misses = 0;
  writebacks = 0;
}

int CacheStats::access(uint32_t addr, ACCESS_TYPE type) {
  if(!CACHE_EN) { // cache is disabled
    return (type == LOAD) ? READ_LATENCY : WRITE_LATENCY;
  }

  uint32_t tag = addr >> 8;
  uint32_t index = addr >> 5;
  index = index & 0x7;

  if(type == LOAD)
  {
    loads = loads + 1;
    for(int i = 0; i < 4; i++)
    {
        if(tagBits[index][i] == tag)
        {
            if(validFlag[index][i])
            {//hit
            return LOOKUP_LATENCY;
            }
        }
    }
    //miss
    //miss checklist
    //check round robin
    //zero out the appropriate tag/dirty bits
    //set valid to 1
    //load in new tag
    //increase cycles
    load_misses = load_misses + 1;
    if(dirtyFlag[index][roundRobinCounter[index]])
    {
        writebacks = writebacks + 1;
        tagBits[index][roundRobinCounter[index]] = tag;
        validFlag[index][roundRobinCounter[index]] = true;
        dirtyFlag[index][roundRobinCounter[index]] = false;
        if(roundRobinCounter[index] == 3)
        {
            roundRobinCounter[index] = 0;
        }
        else
        {
            roundRobinCounter[index] = roundRobinCounter[index] + 1;
        }
    return WRITE_LATENCY + READ_LATENCY;
    }
    if(!dirtyFlag[index][roundRobinCounter[index]])
    {
        tagBits[index][roundRobinCounter[index]] = tag;
        validFlag[index][roundRobinCounter[index]] = true;
        dirtyFlag[index][roundRobinCounter[index]] = false;
        if(roundRobinCounter[index] == 3)
        {
            roundRobinCounter[index] = 0;
        }
        else
        {
            roundRobinCounter[index] = roundRobinCounter[index] + 1;
        }
    return READ_LATENCY;
    }
  }
  if(type == STORE)
  {
    stores = stores + 1;
    //check for hit
    for(int i = 0; i < 4; i++)
    {
        if(tagBits[index][i] == tag)
        {   //hit
            if(validFlag[index][i])
            {
            dirtyFlag[index][i] = true;
            return LOOKUP_LATENCY;
            }
        }
    }
    //miss
    store_misses = store_misses + 1;
    //find max round robin for eviction

    //check dirty bit on all evicts
    //round robin policy handle that
    //write always dirty flag to true

    if(dirtyFlag[index][roundRobinCounter[index]])
    {
    writebacks = writebacks + 1;
    dirtyFlag[index][roundRobinCounter[index]] = true;
    tagBits[index][roundRobinCounter[index]] = tag;
    validFlag[index][roundRobinCounter[index]] = true;

        if(roundRobinCounter[index] == 3)
        {
            roundRobinCounter[index] = 0;
        }
        else
        {
            roundRobinCounter[index] = roundRobinCounter[index] + 1;
        }
    return READ_LATENCY + WRITE_LATENCY;
    }

    if(!dirtyFlag[index][roundRobinCounter[index]])
    {
    dirtyFlag[index][roundRobinCounter[index]] = true;
    tagBits[index][roundRobinCounter[index]] = tag;
    validFlag[index][roundRobinCounter[index]] = true;

        if(roundRobinCounter[index] == 3)
        {
            roundRobinCounter[index] = 0;
        }
        else
        {
            roundRobinCounter[index] = roundRobinCounter[index] + 1;
        }
    return READ_LATENCY;
    }
  }
}

void CacheStats::printFinalStats() {

  for(int i = 0; i < 8; i++)
  {
      for(int j = 0; j < 4; j++)
      {
          if(dirtyFlag[i][j])
          {
              writebacks = writebacks + 1;
          }
      }
  }

  int accesses = loads + stores;
  int misses = load_misses + store_misses;
  cout << "Accesses: " << accesses << endl;
  cout << "  Loads: " << loads << endl;
  cout << "  Stores: " << stores << endl;
  cout << "Misses: " << misses << endl;
  cout << "  Load misses: " << load_misses << endl;
  cout << "  Store misses: " << store_misses << endl;
  cout << "Writebacks: " << writebacks << endl;
  cout << "Hit Ratio: " << fixed << setprecision(1) << 100.0 * (accesses - misses) / accesses;
  cout << "%" << endl;
}
