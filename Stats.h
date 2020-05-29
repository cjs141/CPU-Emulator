#ifndef __STATS_H
#define __STATS_H
#include <iostream>
#include <iomanip>
#include "Debug.h"
using namespace std;

enum PIPESTAGE { IF1 = 0, IF2 = 1, ID = 2, EXE1 = 3, EXE2 = 4, MEM1 = 5,
                 MEM2 = 6, WB = 7, PIPESTAGES = 8 };

class Stats {
  private:
    long long cycles;
    int flushes;
    int bubbles;
    int stalls;

    int memops;
    int branches;
    int taken;
    int hazard;
    int exe1Hazard;
    int exe2Hazard;
    int mem1Hazard;
    int mem2Hazard;

    int resultReg[PIPESTAGES];
    int resultStage[PIPESTAGES];

  public:
    Stats();

    void clock();

    void flush(int count);
    void stall(int count);

    void registerSrc(int r, int p);
    void registerDest(int r, int p);

    void countMemOp() { memops++; }
    void countBranch() { branches++; }
    void countTaken() { taken++; }

    void showPipe();

    // getters
    long long getCycles() { return cycles; }
    int getStalls() { return stalls; }
    int getFlushes() { return flushes; }
    int getBubbles() { return bubbles; }
    int getMemOps() { return memops; }
    int getBranches() { return branches; }
    int getTaken() { return taken; }
    int getHazard() { return hazard; }
    int getExe1Hazard() { return exe1Hazard; }
    int getExe2Hazard() { return exe2Hazard; }
    int getMem1Hazard() { return mem1Hazard; }
    int getMem2Hazard() { return mem2Hazard; }


  private:
    void bubble();
};

#endif
