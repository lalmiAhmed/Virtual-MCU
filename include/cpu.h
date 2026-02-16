#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "exception.h"

#define SP R[13]
#define LR R[14]
#define PC R[15]

typedef enum {
  EQ, NE, CS, CC, MI, PL, VS, VC,
  HI, LS, GE, LT, GT, LE
} Condition; 

#define N_MASK (1<<31)
#define Z_MASK (1<<30)
#define C_MASK (1<<29)
#define V_MASK (1<<28)


// Application Program Status Register (Flags)
typedef struct{
  uint32_t APSR_N:1; // Negative flag
  uint32_t APSR_Z:1; // Zero flag
  uint32_t APSR_C:1; // Carry flag
  uint32_t APSR_V:1; // Overflow flag
  uint32_t reserved:28;
} ASPR_bits;


typedef union {
  uint32_t all;
  ASPR_bits Bits;
} APSR_t;

// Cortex-M0 has 16 registers (R0-R15) + Status Register
typedef struct {
    uint32_t R[16];  // General-purpose registers (R0-R15)
    APSR_t APSR;     // Application Program Status Register (Flags)
} CortexM0_CPU;



/********************Functions Declaration************************ */



void init_cpu(CortexM0_CPU *cpu);
void print_cpu_state(CortexM0_CPU *cpu);
void update_flags(CortexM0_CPU *cpu, uint32_t result, _Bool carry, _Bool overflow);

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

void MOVS(CortexM0_CPU *cpu, uint8_t Rd, uint8_t imm8);
void MOVS_REG(CortexM0_CPU *cpu, uint8_t Rd, uint8_t Rm);

void LSL(CortexM0_CPU *cpu, uint8_t Rd, uint8_t Rm,uint32_t immediate);
void LSR(CortexM0_CPU *cpu, uint8_t Rd, uint8_t Rm,uint32_t immediate);

void raise_hardfault(CortexM0_CPU *cpu);
void check_Rt_validity(uint8_t Rt, const char *instruction_name);


#endif // CPU_H
