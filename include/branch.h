
#ifndef BRANCH_H
#define BRANCH_H

#include <stdio.h>
#include <stdint.h>
#include "cpu.h"


void B(CortexM0_CPU *cpu, int32_t signed_immediate);
void Bcond(CortexM0_CPU *cpu, int32_t offset, Condition cond);
void BLX(CortexM0_CPU *cpu, uint8_t Rm, uint8_t inst_size);
void BX(CortexM0_CPU *cpu, uint8_t Rm);


#endif // BRANCH_H