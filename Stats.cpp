/******************************
 * Submitted by: Craig Sandlin cjs141
 * CS 3339 - Spring 2020, Texas State University
 * Project 3 Pipelining
 * Copyright 2020, all rights reserved
 * Updated by Lee B. Hinkle based on prior work by Martin Burtscher and Molly O'Neil
 ******************************/

#include "Stats.h"

Stats::Stats() {
  cycles = PIPESTAGES - 1; // pipeline startup cost
  flushes = 0;
  bubbles = 0;
  stalls = 0;

  memops = 0;
  branches = 0;
  taken = 0;

  for(int i = IF1; i < PIPESTAGES; i++) {
    resultReg[i] = -1;
  }
}

void Stats::clock() {
  cycles++;

  // advance all pipeline flip-flops
  for(int i = WB; i > IF1; i--) {
    resultReg[i] = resultReg[i-1];
    resultStage[i] = resultStage[i-1];
  }
  // inject a no-op in IF1 for resultReg and resultStage
  resultReg[IF1] = -1;
  resultStage[IF1] = 0;
  //showPipe();
}
//Check for how many bubbles to insert based on position of conflict
void Stats::registerSrc(int r, int p) {
    int numBubbles;
    int cyclesUntilNeeded;
    int cyclesUntilValid;
    if(r == resultReg[EXE1])
    {
        cyclesUntilNeeded = p-ID;
        cyclesUntilValid = resultStage[EXE1]-EXE1;
        hazard++;
        exe1Hazard++;
        numBubbles = cyclesUntilValid-cyclesUntilNeeded;
        for(int i = 0; i < numBubbles; i++)
        {
            bubble();
        }
    }
    else if(r == resultReg[EXE2])
    {
        cyclesUntilNeeded = p-ID;
        cyclesUntilValid = resultStage[EXE2]-EXE2;
        hazard++;
        exe2Hazard++;
        numBubbles = cyclesUntilValid-cyclesUntilNeeded;
        for(int i = 0; i < numBubbles; i++)
        {
            bubble();
        }
    }
    else if(r == resultReg[MEM1])
    {
        cyclesUntilNeeded = p-ID;
        cyclesUntilValid = resultStage[MEM1]-MEM1;
        hazard++;
        mem1Hazard++;
        numBubbles = cyclesUntilValid-cyclesUntilNeeded;
        for(int i = 0; i < numBubbles; i++)
        {
            bubble();
        }
    }
    else if(r == resultReg[MEM2])
    {
        cyclesUntilNeeded = p-ID;
        cyclesUntilValid = resultStage[MEM2]-MEM2;
        hazard++;
        mem2Hazard++;
        numBubbles = cyclesUntilValid-cyclesUntilNeeded;
        for(int i = 0; i < numBubbles; i++)
        {
            bubble();
        }
    }
}
//Insert destination register into ID so that it can traverse the pipe
//Insert when data is valid into resultStage for later comparison
void Stats::registerDest(int r, int p) {
    resultReg[ID] = r;
    resultStage[ID] = p;
}

void Stats::stall(int count)
{
    stalls = stalls + count;
    cycles = cycles + count;
}
//2 flushes each time flush is called on a branch taken
void Stats::flush(int count) {

  for(int i = 0; i < count; i++)
  {
    cycles++;
    flushes++;
    // advance all pipeline flip-flops
    for(int i = WB; i > IF1; i--) {
       resultReg[i] = resultReg[i-1];
    }
    // inject a no-op in IF1
    resultReg[IF1] = -1;
    resultReg[IF2] = -1;
    //showPipe();
  }
}

//Advance pipeline after ID once and insert -1 in EXE1
void Stats::bubble() {
    bubbles++;
    cycles++;
    for(int i = WB; i > EXE1; i--) {
      resultReg[i] = resultReg[i-1];
      resultStage[i] = resultStage[i-1];
    }
    resultReg[EXE1] = -1;
    resultStage[EXE1] = 0;
    //showPipe();
}

void Stats::showPipe() {
  // this method is to assist testing and debug, please do not delete or edit
  // you are welcome to use it but remove any debug outputs before you submit
  cout << "              IF1  IF2 *ID* EXE1 EXE2 MEM1 MEM2 WB         #C      #B      #F" << endl;
  cout << "  resultReg ";
  for(int i = 0; i < PIPESTAGES; i++) {
    cout << "  " << dec << setw(2) << resultReg[i] << " ";
  }
  cout << endl << " resultStage";
  for(int i = 0; i < PIPESTAGES; i++) {
    cout << "  " << dec << setw(2) << resultStage[i] << " ";
  }
  cout << "   " << setw(7) << cycles << " " << setw(7) << bubbles << " " << setw(7) << flushes;
  cout << endl;
}
