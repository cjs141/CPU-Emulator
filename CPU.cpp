/******************************
 * Submitted by: Craig Sandlin cjs141
 * CS 3339 - Spring 2020, Texas State University
 * Project 5 Cache
 * Copyright 2020, all rights reserved
 * Updated by Lee B. Hinkle based on prior work by Martin Burtscher and Molly O'Neil
 ******************************/

#include "CPU.h"

const string CPU::regNames[] = {"$zero","$at","$v0","$v1","$a0","$a1","$a2","$a3",
                                "$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7",
                                "$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7",
                                "$t8","$t9","$k0","$k1","$gp","$sp","$fp","$ra"};

CPU::CPU(uint32_t pc, Memory &iMem, Memory &dMem) : pc(pc), iMem(iMem), dMem(dMem) {
  for(int i = 0; i < NREGS; i++) {
    regFile[i] = 0;
  }
  hi = 0;
  lo = 0;
  regFile[28] = 0x10008000; // gp
  regFile[29] = 0x10000000 + dMem.getSize(); // sp

  instructions = 0;
  stop = false;
}

void CPU::run() {
  while(!stop) {
    instructions++;

    fetch();
    decode();
    execute();
    mem();
    writeback();
   // stats.showPipe();
    stats.clock();
   // D(printRegFile());
  }
}

void CPU::fetch() {
  instr = iMem.loadWord(pc);
  pc = pc + 4;
}

/////////////////////////////////////////
// ALL YOUR CHANGES GO IN THIS FUNCTION
/////////////////////////////////////////
void CPU::decode() {
  uint32_t opcode;      // opcode field
  uint32_t rs, rt, rd;  // register specifiers
  uint32_t shamt;       // shift amount (R-type)
  uint32_t funct;       // funct field (R-type)
  uint32_t uimm;        // unsigned version of immediate (I-type)
  int32_t simm;         // signed version of immediate (I-type)
  uint32_t addr;        // jump address offset field (J-type)
  int32_t temp;
  //int stallAmount;

  opcode = (instr & 0xfc000000) >> 26;
  rs = (instr & 0x03e00000) >> 21;
  rt = (instr & 0x001f0000) >> 16;
  rd = (instr & 0x0000f800) >> 11;
  shamt = (instr >> 6) & 0x1f;
  funct = instr & 0x0000003f;
  uimm = instr & 0x0000ffff;
  //Determine if immediate value is negative
  temp = (instr & 0x0000ffff) >> 15;
  if(temp == 0x1)
  {
      //if negative mask then pad with ones
      simm = (instr & 0xffff) | 0xffff0000;
  }
  else
  {
      //if positive just mask
      simm = instr & 0x0000ffff;
  }

  addr = instr & 0x03ffffff;

//Control signal declarations, reinitialized on each loop
    opIsLoad = false;
    opIsStore = false;
    opIsMultDiv = false;
    aluOp = ADD;
    writeDest = false;
    destReg = REG_ZERO;
    aluSrc1 = regFile[REG_ZERO];
    aluSrc2 = regFile[REG_ZERO];

    //Wires between stages
    uint32_t aluOut;
    uint32_t writeData;
  //stats.clock();
  D(cout << "  " << hex << setw(8) << pc - 4 << ": ");
  switch(opcode) {
    case 0x00:
      switch(funct) {
        case 0x00: D(cout << "sll " << regNames[rd] << ", " << regNames[rs] << ", " << dec << shamt);
                   writeDest = true; destReg = rd; stats.registerDest(rd, MEM1);//Writing to a register[rd] = true
                   aluOp = SHF_L;
                   aluSrc1 = regFile[rs]; stats.registerSrc(rs, EXE1);// rs is moved left logical by shamt
                   aluSrc2 = shamt;
                   break;
        case 0x03: D(cout << "sra " << regNames[rd] << ", " << regNames[rs] << ", " << dec << shamt);
                   writeDest = true; destReg = rd; stats.registerDest(rd, MEM1);//Writing to a register[rd] = true
                   aluOp = SHF_R;
                   aluSrc1 = regFile[rs]; stats.registerSrc(rs, EXE1);// rs is moved right arithmatic by shamt
                   aluSrc2 = shamt;
                   break;
        case 0x08: D(cout << "jr " << regNames[rs]);
                   aluOp = ADD;
                   stats.flush(2);
                   aluSrc1 = pc;
                   aluSrc2 = regFile[REG_ZERO];
                   pc = regFile[rs];stats.registerSrc(rs, ID);// jumps to this value
                   break;
        case 0x10: D(cout << "mfhi " << regNames[rd]);
                   writeDest = true; destReg = rd; stats.registerDest(rd, MEM1);
                   aluOp = ADD;
                   aluSrc1 = alu.getUpper(); stats.registerSrc(REG_HILO, EXE1);
                   aluSrc2 = regFile[REG_ZERO];
                   break;
        case 0x12: D(cout << "mflo " << regNames[rd]);
                   writeDest = true; destReg = rd; stats.registerDest(rd, MEM1);
                   aluOp = ADD;
                   aluSrc1 = alu.getLower(); stats.registerSrc(REG_HILO, EXE1);
                   aluSrc2 = regFile[REG_ZERO];
                   break;
        case 0x18: D(cout << "mult " << regNames[rs] << ", " << regNames[rt]);
                   opIsMultDiv = true; stats.registerDest(REG_HILO, WB);
                   aluOp = MUL;
                   aluSrc1 = regFile[rs]; stats.registerSrc(rs, EXE1);
                   aluSrc2 = regFile[rt]; stats.registerSrc(rt, EXE1);
                   break;
        case 0x1a: D(cout << "div " << regNames[rs] << ", " << regNames[rt]);
                   opIsMultDiv = true; stats.registerDest(REG_HILO, WB);
                   aluOp = DIV;
                   aluSrc1 = regFile[rs]; stats.registerSrc(rs, EXE1);
                   aluSrc2 = regFile[rt]; stats.registerSrc(rt, EXE1);
                   break;
        case 0x21: D(cout << "addu " << regNames[rd] << ", " << regNames[rs] << ", " << regNames[rt]);
                   writeDest = true; destReg = rd; stats.registerDest(rd, MEM1);
                   aluOp = ADD;
                   aluSrc1 = regFile[rs]; stats.registerSrc(rs, EXE1);
                   aluSrc2 = regFile[rt]; stats.registerSrc(rt, EXE1);
                   break;
        case 0x23: D(cout << "subu " << regNames[rd] << ", " << regNames[rs] << ", " << regNames[rt]);
                   writeDest = true; destReg = rd; stats.registerDest(rd, MEM1);
                   aluOp = ADD;
                   aluSrc1 = regFile[rs]; stats.registerSrc(rs, EXE1);
                   aluSrc2 = 0-(regFile[rt]); stats.registerSrc(rt, EXE1);//0-rt = -rt
                   break;
        case 0x2a: D(cout << "slt " << regNames[rd] << ", " << regNames[rs] << ", " << regNames[rt]);
                   writeDest = true; destReg = rd; stats.registerDest(rd, MEM1);// sets rd if comparison is true
                   aluOp = CMP_LT;
                   aluSrc1 = regFile[rs]; stats.registerSrc(rs, EXE1);
                   aluSrc2 = regFile[rt]; stats.registerSrc(rt, EXE1);
                   break;
        default: cerr << "unimplemented instruction: pc = 0x" << hex << pc - 4 << endl;
      }
      break;
    case 0x02: D(cout << "j " << hex << ((pc & 0xf0000000) | addr << 2)); // P1: pc + 4
               stats.flush(2);
               writeDest = false;
               pc = (pc & 0xf0000000) | addr << 2;
               break;
    case 0x03: D(cout << "jal " << hex << ((pc & 0xf0000000) | addr << 2)); // P1: pc + 4
               writeDest = true; destReg = REG_RA; stats.registerDest(REG_RA, EXE1); // writes PC+4 to $ra
               stats.flush(2);
               aluOp = ADD; // ALU should pass pc thru unchanged
               aluSrc1 = pc;
               aluSrc2 = regFile[REG_ZERO]; // always reads zero
               pc = (pc & 0xf0000000) | addr << 2;
               break;
    case 0x04: D(cout << "beq " << regNames[rs] << ", " << regNames[rt] << ", " << pc + (simm << 2));
               stats.countBranch();
               stats.registerSrc(rs, ID);
               stats.registerSrc(rt, ID);
               if (regFile[rs] == regFile[rt]) {//branch comparison
                   stats.flush(2);
                   stats.countTaken();
                   aluOp = ADD;
                   aluSrc1 = pc;
                   aluSrc2 = regFile[REG_ZERO];
                   pc = pc + (simm << 2);// if true set pc
               }
               break;  // read the handout carefully, update PC directly here as in jal example
    case 0x05: D(cout << "bne " << regNames[rs] << ", " << regNames[rt] << ", " << pc + (simm << 2));
               stats.countBranch();
               stats.registerSrc(rs, ID);
               stats.registerSrc(rt, ID);
               if (regFile[rs] != regFile[rt]) {//branch comparison
                   stats.flush(2);
                   stats.countTaken();
                   aluOp = ADD;
                   aluSrc1 = pc;
                   aluSrc2 = regFile[REG_ZERO];
                   pc = pc + (simm << 2);//if true set pc
               }
               break;  // same comment as beq
    case 0x09: D(cout << "addiu " << regNames[rt] << ", " << regNames[rs] << ", " << dec << simm);
               writeDest = true; destReg = rt; stats.registerDest(rt, MEM1);
               aluOp = ADD;
               aluSrc1 = regFile[rs]; stats.registerSrc(rs, EXE1);
               aluSrc2 = simm;
               break;
    case 0x0c: D(cout << "andi " << regNames[rt] << ", " << regNames[rs] << ", " << dec << uimm);
               writeDest = true; destReg = rt;  stats.registerDest(rt, MEM1);
               aluOp = AND;
               aluSrc1 = regFile[rs]; stats.registerSrc(rs, EXE1);// and valur in rs and uimm
               aluSrc2 = uimm;
               break;
    case 0x0f: D(cout << "lui " << regNames[rt] << ", " << dec << simm);
               writeDest = true; destReg = rt; stats.registerDest(rt, MEM1);
               aluOp = ADD;
               aluSrc1 = regFile[REG_ZERO];
               aluSrc2 = simm << 16;// found that this shift was necessary through testing
               break;
    case 0x1a: D(cout << "trap " << hex << addr);
               switch(addr & 0xf) {
                 case 0x0: cout << endl; break;
                 case 0x1: cout << " " << (signed)regFile[rs]; stats.registerSrc(rs, EXE1);
                           break;
                 case 0x5: cout << endl << "? "; cin >> regFile[rt]; stats.registerDest(rt, MEM1);
                           break;
                 case 0xa: stop = true; break;
                 default: cerr << "unimplemented trap: pc = 0x" << hex << pc - 4 << endl;
                          stop = true;
               }
               break;
    case 0x23: D(cout << "lw " << regNames[rt] << ", " << dec << simm << "(" << regNames[rs] << ")");
               stats.countMemOp();
               writeDest = true; destReg = rt; stats.registerDest(rt, WB);
               opIsLoad = true;
               opIsStore = false;
               aluOp = ADD;
               aluSrc1 = regFile[rs]; stats.registerSrc(rs, EXE1);
               aluSrc2 = simm;
               //stallAmount = cache.access(aluOut, LOAD);
               //stats.stall(stallAmount);
               break;
    case 0x2b: D(cout << "sw " << regNames[rt] << ", " << dec << simm << "(" << regNames[rs] << ")");
               stats.countMemOp();
               writeDest = false;
               opIsStore = true;
               aluOp = ADD;
               aluSrc1 = regFile[rs]; stats.registerSrc(rs, EXE1);
               aluSrc2 = simm;
               storeData = regFile[rt]; stats.registerSrc(rt, MEM1);// data to be stored
              // stallAmount = cache.access(aluOut, STORE);
               //stats.stall(stallAmount);
               break;  // same comment as lw
    default: cerr << "unimplemented instruction: pc = 0x" << hex << pc - 4 << endl;
  }
  D(cout << endl);
}

void CPU::execute() {
  aluOut = alu.op(aluOp, aluSrc1, aluSrc2);
}

void CPU::mem() {
  int stallAmount;
  if(opIsLoad)
  {
    writeData = dMem.loadWord(aluOut);
    stallAmount = cache.access(aluOut, LOAD);
    stats.stall(stallAmount);
  }

  else
    writeData = aluOut;

  if(opIsStore)
  {
    dMem.storeWord(storeData, aluOut);
    stallAmount = cache.access(aluOut, STORE);
    stats.stall(stallAmount);
  }
}

void CPU::writeback() {
  if(writeDest && destReg > 0) // skip when write is to zero_register
    regFile[destReg] = writeData;

  if(opIsMultDiv) {
    hi = alu.getUpper();
    lo = alu.getLower();
  }
}

void CPU::printRegFile() {
  cout << hex;
  for(int i = 0; i < NREGS; i++) {
    cout << "    " << regNames[i];
    if(i > 0) cout << "  ";
    cout << ": " << setfill('0') << setw(8) << regFile[i];
    if( i == (NREGS - 1) || (i + 1) % 4 == 0 )
      cout << endl;
  }
  cout << "    hi   : " << setfill('0') << setw(8) << hi;
  cout << "    lo   : " << setfill('0') << setw(8) << lo;
  cout << dec << endl;
}

void CPU::printFinalStats() {
  double memory = stats.getMemOps();
  double branches = stats.getBranches();
  double taken = stats.getTaken();
  double cycle = stats.getCycles();
  double hazard = stats.getHazard();
  double exe1Hazard = stats.getExe1Hazard();
  double exe2Hazard = stats.getExe2Hazard();
  double mem1Hazard = stats.getMem1Hazard();
  double mem2Hazard = stats.getMem2Hazard();

  cout << "Program finished at pc = 0x" << hex << pc << "  (" << dec << instructions << " instructions executed)" << endl << endl
       << "Cycles: " << stats.getCycles() << endl
       << "CPI: " << fixed << setprecision(2) << (cycle/instructions) << endl << endl
       << "Bubbles: " << stats.getBubbles() << endl
       << "Flushes: " << stats.getFlushes() << endl
       << "Stalls: " << stats.getStalls() << endl << endl;
       cache.printFinalStats();

}
