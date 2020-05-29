#ifndef __CACHE_STATS_H
#define __CACHE_STATS_H

#include <cstdint>
#include "Debug.h"
using namespace std;

#ifndef CACHE_EN
#define CACHE_EN 1
#endif

#ifndef BLOCKSIZE
#define BLOCKSIZE 32
#endif

#ifndef SETS
#define SETS 8
#endif

#ifndef WAYS
#define WAYS 4
#endif

#ifndef LOOKUP_LATENCY
#define LOOKUP_LATENCY 0
#endif

#ifndef READ_LATENCY
#define READ_LATENCY 30
#endif

#ifndef WRITE_LATENCY
#define WRITE_LATENCY 10
#endif

enum ACCESS_TYPE { LOAD, STORE };

class CacheStats {
  private:
   /* bool validBit = 1;
    bool dirtyBit = 1;
    int indexBits = 3;//Used to select the block pg.460 (1024/32)/4=8 sets
    int offsetBits = 3;//Each cache block has 8 words = 3 bits
    int tagBits = 24;
    int roundRobin = 0;
*/
    bool dirtyFlag[8][4]{
        {false, false, false, false},
        {false, false, false, false},
        {false, false, false, false},
        {false, false, false, false},
        {false, false, false, false},
        {false, false, false, false},
        {false, false, false, false},
        {false, false, false, false}
    };
    bool validFlag[8][4]{
        {false, false, false, false},
        {false, false, false, false},
        {false, false, false, false},
        {false, false, false, false},
        {false, false, false, false},
        {false, false, false, false},
        {false, false, false, false},
        {false, false, false, false}
    };
    uint32_t tagBits[8][4]{
        {0x00000000, 0x00000000, 0x00000000, 0x00000000},
        {0x00000000, 0x00000000, 0x00000000, 0x00000000},
        {0x00000000, 0x00000000, 0x00000000, 0x00000000},
        {0x00000000, 0x00000000, 0x00000000, 0x00000000},
        {0x00000000, 0x00000000, 0x00000000, 0x00000000},
        {0x00000000, 0x00000000, 0x00000000, 0x00000000},
        {0x00000000, 0x00000000, 0x00000000, 0x00000000},
        {0x00000000, 0x00000000, 0x00000000, 0x00000000}
    };


    int loads;
    int stores;
    int load_misses;
    int store_misses;
    int writebacks;
    int roundRobinCounter[8] = {0,0,0,0,0,0,0,0};



  public:
    CacheStats();
    int access(uint32_t, ACCESS_TYPE);
    void printFinalStats();
};

#endif
