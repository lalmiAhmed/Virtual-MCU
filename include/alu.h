

#ifndef ALU_H
#define ALU_H

#include <stdio.h>
#include <stdint.h>
#include "cpu.h"



void ADD(CortexM0_CPU *cpu, uint8_t Rd, uint8_t Rn, uint8_t Rm);
void SUB(CortexM0_CPU *cpu, uint8_t Rd, uint8_t Rn, uint8_t Rm);
void CMP(CortexM0_CPU *cpu, uint8_t Rn, uint8_t Rm);
void AND(CortexM0_CPU *cpu, uint8_t Rn, uint8_t Rm, uint8_t Rd);
void ORR(CortexM0_CPU *cpu, uint8_t Rn, uint8_t Rm, uint8_t Rd);
void EOR(CortexM0_CPU *cpu, uint8_t Rn, uint8_t Rm, uint8_t Rd);
void TST(CortexM0_CPU *cpu, uint8_t Rn, uint8_t Rm);


#endif // ALU_H