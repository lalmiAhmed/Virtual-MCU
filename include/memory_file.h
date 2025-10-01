


#ifndef MEMORY_FILE_H
#define MEMORY_FILE_H

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "cpu.h"


#define MEMORY_SIZE 56 // to be changed to 5KB
#define Stack_size 12 // to be changed to 1KB
#define WORD_SIZE 4
#define HALFWORD_SIZE 2
#define BYTE_SIZE 1

extern uint8_t Memory[MEMORY_SIZE];
extern uint8_t Stack[Stack_size];

_Bool check_memory_bounds(uint32_t address, uint32_t size);
void print_memory();
void print_stack();

void STR(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm);
void STRH(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm);
void STRB(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm);
void LDR(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm);
void LDRH(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm);
void LDRSH(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm);
void LDRB(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm);
void LDRSB(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm);

void PUSH(CortexM0_CPU *cpu, uint32_t value);
uint32_t POP(CortexM0_CPU *cpu);



#endif // MEMORY_FILE_H