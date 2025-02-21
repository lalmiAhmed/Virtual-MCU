#include <stdint.h>
#include <stdio.h>
#include "main.h"




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
  int64_t result = (int64_t)op1 + (int64_t)op2;
  printf("here is the operation: %ld + %ld = %ld \n", (int64_t)op1, (int64_t)op2, result);
  // check for carry
  _Bool carry = (result > 0xFFFFFFFF);
  if (carry){printf("a carry is yeild \n");}
  
  // cast the result to 32bit and check for overflow
  result = (uint32_t)result;
  printf("the result after the casting: %ld\n",result);
  _Bool overflow = (((op1 & 0x80000000) == (op2 & 0x80000000)) && ((result&0x80000000) != (op1 & 0x80000000)));
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
    cpu.R[2] = 0x7FFFFFFF;
    cpu.R[1] = 0X1;
    SUB(&cpu, 0, 1, 2);
    print_cpu_state(&cpu);

    return 0;
}
