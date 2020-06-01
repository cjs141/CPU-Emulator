# CPU Emulator
> Creator: Craig Sandlin with skeleton provided by Professor Lee B. Hinkle

> Project Description: Functionality of a classic five-stage pipelined CPU with an L1 cache using write-back, write allocate with a round robin replacement policy. Full forwarding paths emulated with statistics on cycle count, bubbles, and flushes required due to data hazards. Binary is converted to assembly and control signals are set. 

## Technologies
* C++

## Code Examples
>Cache  Statistics:
```
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

```

## Features
* Converts Binary to Assembly
* Sets Control Signals
* Emulates stalls and flushes required with forwarding paths
* Emulates an L1 cache with using write-back, write allocate with a round robin replacement policy



## Status
Project is: Closed, no further development is planned.
