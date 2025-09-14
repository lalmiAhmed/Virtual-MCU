


#ifndef MEMORY_FILE_H
#define MEMORY_FILE_H

#include <stdio.h>
#include <stdint.h>
#include "cpu.h"


#define MEMORY_SIZE 56
#define WORD_SIZE 4
#define HALFWORD_SIZE 2
#define BYTE_SIZE 1

extern uint8_t Memory[MEMORY_SIZE];

_Bool check_memory_bounds(uint32_t address, uint32_t size);
void print_memory();

void STR(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm);
void STRH(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm);
void STRB(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm);
void LDR(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm);
void LDRH(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm);
void LDRSH(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm);
void LDRB(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm);
void LDRSB(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm);



#endif // MEMORY_FILE_H