#include <stdint.h>
#include <stdio.h>
#include "main.h"


/* Static function declaration */
static _Bool check_memory_bounds(uint32_t address, uint32_t size);



// Initialize CPU state
void init_cpu(CortexM0_CPU *cpu) {
    for (int i = 0; i < 16; i++) {
        cpu->R[i] = 0;  // Clear all registers
    }
    cpu->APSR.all = 0;      // Clear flags
}

// ADD Instruction
void ADD(CortexM0_CPU *cpu, uint8_t Rd, uint8_t Rn, uint8_t Rm) {
  uint32_t op1 = cpu->R[Rn];
  uint32_t op2 = cpu->R[Rm];
  uint64_t result = (uint64_t)op1 + (uint64_t)op2;
  // printf("here is the operation: %ld + %ld = %ld \n", (int64_t)op1, (int64_t)op2, result);
  // check for carry
  _Bool carry = (result >= UINT64_C(0x100000000));
  _Bool overflow = (((op1 & 0x80000000) == (op2 & 0x80000000)) && ((result&0x80000000) != (op1 & 0x80000000)));

  // cast the result to 32bit and check for overflow
  result = (uint32_t)result;
  cpu->R[Rd] = result;
  update_flags(cpu, result, carry, overflow);
}

// SUB Instruction
void SUB(CortexM0_CPU *cpu, uint8_t Rd, uint8_t Rn, uint8_t Rm) {
  uint32_t op1 = cpu->R[Rn];
  uint32_t op2 = cpu->R[Rm];
  uint32_t result = op1 - op2;
  printf("SUB operation: %0x - %0x = %0x \n", op1, op2, result);
  // Carry is inverted for SUB in ARM
  _Bool carry = !(op1 < op2);
  // An overflow is set if the ops have the same sign, that is different than the result sign
  _Bool overflow = (((op1 & 0x80000000) == (op2 & 0x80000000)) && ((result&0x80000000) != (op1 & 0x80000000)));
  cpu->R[Rd] = result;
  update_flags(cpu, result, carry, overflow);
}

// CMP Instruction (Compare: Rn - Rm)
void CMP(CortexM0_CPU *cpu, uint8_t Rn, uint8_t Rm) {
  uint32_t op1 = cpu->R[Rn];
  uint32_t op2 = cpu->R[Rm];
  uint32_t result = op1 - op2;
  printf("here is the operation: %d - %d = %d \n", op1, op2, result);
  // check for carry
  _Bool carry = !(op1 < op2);
  _Bool overflow = (((op1 & 0x80000000) == (op2 & 0x80000000)) && ((result&0x80000000) != (op1 & 0x80000000)));
  update_flags(cpu, result, carry, overflow);
}

// Logical AND
void AND(CortexM0_CPU *cpu, uint8_t Rn, uint8_t Rm, uint8_t Rd){
  uint32_t op1 = cpu->R[Rn];
  uint32_t op2 = cpu->R[Rm];
  uint32_t result = op1 & op2;
  _Bool carry = cpu->APSR.Bits.APSR_C;   // Keep previous carry unchanged
  _Bool overflow = cpu->APSR.Bits.APSR_V; // Keep previous overflow unchanged
  cpu->R[Rd] = result;
  update_flags(cpu, result, carry, overflow);
}

// Logical OR
void ORR(CortexM0_CPU *cpu, uint8_t Rn, uint8_t Rm, uint8_t Rd){
  uint32_t op1 = cpu->R[Rn];
  uint32_t op2 = cpu->R[Rm];
  uint32_t result = op1 | op2;
  _Bool carry = cpu->APSR.Bits.APSR_C;   // Keep previous carry unchanged
  _Bool overflow = cpu->APSR.Bits.APSR_V; // Keep previous overflow unchanged
  cpu->R[Rd] = result;
  update_flags(cpu, result, carry, overflow);
}

// Logical exclusive OR
void EOR(CortexM0_CPU *cpu, uint8_t Rn, uint8_t Rm, uint8_t Rd){
  uint32_t op1 = cpu->R[Rn];
  uint32_t op2 = cpu->R[Rm];
  uint32_t result = op1 ^ op2;
  _Bool carry = cpu->APSR.Bits.APSR_C;   // Keep previous carry unchanged
  _Bool overflow = cpu->APSR.Bits.APSR_V; // Keep previous overflow unchanged
  cpu->R[Rd] = result;
  update_flags(cpu, result, carry, overflow);
}

// Logical TST
void TST(CortexM0_CPU *cpu, uint8_t Rn, uint8_t Rm){
  uint32_t op1 = cpu->R[Rn];
  uint32_t op2 = cpu->R[Rm];
  uint32_t result = op1 & op2;
  _Bool carry = cpu->APSR.Bits.APSR_C;   // Keep previous carry unchanged
  _Bool overflow = cpu->APSR.Bits.APSR_V; // Keep previous overflow unchanged
  update_flags(cpu, result, carry, overflow);
}


static _Bool check_memory_bounds(uint32_t address, uint32_t size) {
  return (address + size - 1 < MEMORY_SIZE);
}

/*
Store the value in Rt to the momory address specified by Rm+Rn
*/
void STR(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm) {
  // Ensure register is valid (not SP, LR, or PC)
  if (Rt >= 13) {
      printf("Invalid register for STR\n");
      return;
  }

  // Compute memory address
  uint32_t addr = cpu->R[Rm] + cpu->R[Rn];

  // Check for alignment (Cortex-M0 usually requires word alignment for 32-bit stores)
  if (addr % WORD_SIZE != 0) {
      printf("Unaligned memory access at 0x%08X\n", addr);
      return;
  }

  // Check for memory boundry
  if (!check_memory_bounds(addr, WORD_SIZE)) {
    printf("Memory access out of bounds at 0x%08X\n", addr);
    return;
  }

  // Store the 32-bit register value in memory
  Memory[addr] = cpu->R[Rt] & 0xFF;
  Memory[addr + 1] = (cpu->R[Rt] >> 8) & 0xFF;
  Memory[addr + 2] = (cpu->R[Rt] >> 16) & 0xFF;
  Memory[addr + 3] = (cpu->R[Rt] >> 24) & 0xFF;
}


/*
Store the half word 16-bit value in Rt to the momory address specified by Rm+Rn
*/
void STRH(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm) {
  if (Rt >= 13) {
      printf("Invalid register for STRH\n");
      return;
  }

  uint32_t addr = cpu->R[Rn] + cpu->R[Rm];

  if (addr % HALFWORD_SIZE != 0) {
      printf("Unaligned STRH access at 0x%08X\n", addr);
      return;
  }

    // Check for memory boundry
    if (!check_memory_bounds(addr, HALFWORD_SIZE)) {
      printf("Memory access out of bounds at 0x%08X\n", addr);
      return;
    }

  uint16_t halfword = cpu->R[Rt] & 0xFFFF;

  Memory[addr]     = (uint8_t)(halfword & 0xFF);
  Memory[addr + 1] = (uint8_t)(halfword >> 8);
}

/*
Store 8-bit value in Rt to the momory address specified by Rm+Rn
*/
void STRB(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm) {
  if (Rt >= 13) {
      printf("Invalid register for STRB\n");
      return;
  }

  uint32_t addr = cpu->R[Rn] + cpu->R[Rm];

  // Check for memory boundry
  if (!check_memory_bounds(addr, BYTE_SIZE)) {
    printf("Memory access out of bounds at 0x%08X\n", addr);
    return;
  }

  Memory[addr]= (uint8_t)(cpu->R[Rt] & 0xFF);
}


/*
Load the value in the momory address specified by Rm+Rn to Rt
*/
void LDR(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm) {
  // Ensure register is valid (not SP, LR, or PC)
  if (Rt >= 13) {
      printf("Invalid register for LDR\n");
      return;
  }

  // Compute memory address
  uint32_t addr = cpu->R[Rm] + cpu->R[Rn];

  // Check for alignment (Cortex-M0 usually requires word alignment for 32-bit stores)
  if (addr % WORD_SIZE != 0) {
    printf("Unaligned memory access at 0x%08X\n", addr);
    return;
  }

  // Check for memory boundry
  if (!check_memory_bounds(addr, WORD_SIZE)) {
    printf("Memory access out of bounds at 0x%08X\n", addr);
    return;
  }

  // Load the 32-bit value memory to the register
  cpu->R[Rt] = Memory[addr] |
                 (Memory[addr + 1] << 8) |
                 (Memory[addr + 2] << 16) |
                 (Memory[addr + 3] << 24);

}

/*
Load the 16-bit unsigned halfword value in the momory address specified by Rm+Rn to Rt
*/
void LDRH(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm) {
  // Ensure register is valid (not SP, LR, or PC)
  if (Rt >= 13) {
      printf("Invalid register for LDR\n");
      return;
  }

  // Compute memory address
  uint32_t addr = cpu->R[Rm] + cpu->R[Rn];

  // Check for alignment (Cortex-M0 usually requires word alignment for 32-bit stores)
  if (addr % HALFWORD_SIZE != 0) {
    printf("Unaligned memory access at 0x%08X\n", addr);
    return;
  }

  // Check for memory boundry
  if (!check_memory_bounds(addr, HALFWORD_SIZE)) {
    printf("Memory access out of bounds at 0x%08X\n", addr);
    return;
  }

  // Load the 16-bit value memory to the register
  cpu->R[Rt] = (uint16_t)Memory[addr] |
                 (Memory[addr + 1] << 8);
}

/*
Load the 16-bit signed halfword value in the momory address specified by Rm+Rn to Rt
*/

void LDRSH(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm) {
  if (Rt >= 13) {
      printf("Invalid register for LDRSH\n");
      return;
  }

  uint32_t addr = cpu->R[Rn] + cpu->R[Rm];

  if (addr % 2 != 0) {
      printf("Unaligned LDRSH access at 0x%08X\n", addr);
      return;
  }

  int16_t halfword = (int16_t)(Memory[addr] |
                               (Memory[addr + 1] << 8));

  cpu->R[Rt] = (int32_t)halfword;  // Sign-extend to 32-bit
}

/*
Load the 8-bit unsigned value in the momory address specified by Rm+Rn to Rt
*/
void LDRB(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm) {
  // Ensure register is valid (not SP, LR, or PC)
  if (Rt >= 13) {
      printf("Invalid register for LDRB\n");
      return;
  }

  // Compute memory address
  uint32_t addr = cpu->R[Rm] + cpu->R[Rn];

  // Check for memory boundry
  if (!check_memory_bounds(addr, BYTE_SIZE)) {
    printf("Memory access out of bounds at 0x%08X\n", addr);
    return;
  }

  // Load the 8-bit value memory to the register
  cpu->R[Rt] = (uint8_t)Memory[addr];
}

/*
Load the 8-bit signed value in the momory address specified by Rm+Rn to Rt
*/
void LDRSB(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm) {
  // Ensure register is valid (not SP, LR, or PC)
  if (Rt >= 13) {
      printf("Invalid register for LDRSB\n");
      return;
  }

  // Compute memory address
  uint32_t addr = cpu->R[Rm] + cpu->R[Rn];

  // Check for memory boundry
  if (!check_memory_bounds(addr, BYTE_SIZE)) {
    printf("Memory access out of bounds at 0x%08X\n", addr);
    return;
  }


  // Load signed 8-bit and sign-extend to 32-bit
  cpu->R[Rt] = (int32_t)((int8_t)Memory[addr]);
}


/* 
Branch instruction (B) 
*/
void B(CortexM0_CPU *cpu, int32_t signed_immediate) {
  printf("Branching by offset: %d\n", signed_immediate);
  cpu->R[15] += signed_immediate;
}

// Print CPU state (for debugging)
void print_cpu_state(CortexM0_CPU *cpu) {
    printf("Registers:\n");
    for (int i = 0; i < 16; i++) {
        printf("R%d: 0x%08X\n", i, cpu->R[i]);
    }
    printf("APSR: N=%d Z=%d C=%d V=%d\n",
        cpu->APSR.Bits.APSR_N,
        cpu->APSR.Bits.APSR_Z,
        cpu->APSR.Bits.APSR_C,
        cpu->APSR.Bits.APSR_V);
}

// Print Memory
void print_memory(){
  for(int i = 0; i < MEMORY_SIZE; i++){
    printf("[%d] 0x%02X ",i, Memory[i]);
    if(i % 8 == 0) {
      printf("\n");
    }
  }
  printf("\n");
}

// Example: Set flags based on a computation result
void update_flags(CortexM0_CPU *cpu, uint32_t result, _Bool carry, _Bool overflow) {
  // Clear previous flags
  cpu->APSR.all = 0;

  // Negative
  if ((result & (0x80000000))){
    cpu->APSR.Bits.APSR_N = 1;
    printf("a negative result \n");
  }
  // Zero
  if (result == 0){
    cpu->APSR.Bits.APSR_Z = 1;
    printf("a zero result \n");
  };
  // Carry
  if (carry){
    cpu->APSR.Bits.APSR_C = 1;
    printf("a carry result \n");
  }
  // Overflow
  if (overflow){
    cpu->APSR.Bits.APSR_V = 1;
    printf("an overflowed result \n");
  }
}



int main() {
    CortexM0_CPU cpu;
    init_cpu(&cpu);
    printf("CPU init: \n");
    print_cpu_state(&cpu);


    // cpu.R[0] = 0x7;  // Load a negative number
    // update_flags(&cpu, cpu.R[0], 0, 0);
    // printf("cpu status after operation: \n");

    cpu.R[1] = 0x4000003C;
    cpu.R[2] = 0x10;
    cpu.R[3] = 0X0;
    STRB(&cpu, 1, 2, 3);


    // Memory[20] = 0x3C;
    // Memory[21] = 0xFD;
    // Memory[22] = 0xAA;
    // Memory[23] = 0xBB;

    // cpu.R[2] = 0x14;
    // LDR(&cpu, 4, 2, 3);
    // LDRH(&cpu, 5, 2, 3);

    print_cpu_state(&cpu);
    print_memory();

    return 0;
}
