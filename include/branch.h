
#ifndef BRANCH_H
#define BRANCH_H

#include <stdio.h>
#include <stdint.h>
#include "cpu.h"


void B(CortexM0_CPU *cpu, int32_t signed_immediate);
void Bcond(CortexM0_CPU *cpu, int32_t offset, Condition cond);

#endif // BRANCH_H