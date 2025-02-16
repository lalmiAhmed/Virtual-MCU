#include <stdint.h>
#include <stdio.h>

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


// Initialize CPU state
void init_cpu(CortexM0_CPU *cpu) {
    for (int i = 0; i < 16; i++) {
        cpu->R[i] = 0;  // Clear all registers
    }
    cpu->APSR.all = 0;      // Clear flags
}

// ADD Instruction
void ADD(CortexM0_CPU *cpu, uint8_t Rd, uint8_t Rn, uint8_t Rm) {
  uint64_t result = cpu->R[n] + cpu->R[m];
  // check for carry
  cpu->R[0] = Rn;
  cpu->R[1] = Rm;
  cpu->R[0] = Rn + Rm;
  update_flags(cpu, Rd);
}

// SUB Instruction
void SUB(CortexM0_CPU *cpu, uint8_t Rd, uint8_t Rn, uint8_t Rm) {
  Rd = Rn - Rm;
  update_flags(cpu, Rd);
}

// // CMP Instruction (Compare: Rn - Rm)
// void CMP(CortexM0_CPU *cpu, uint8_t Rn, uint8_t Rm) {
//   Rd = Rn + Rm;
//   update_flags(cpu, Rd);
// }

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
void update_flags(CortexM0_CPU *cpu, uint32_t result) {
  // Clear previous flags  
  cpu->APSR.all = 0;
  // Set Zero flag if result is 0
  if (result == 0){
    cpu->APSR.Bits.APSR_Z = 1;
  };  
  // Negative 
  if ((result & (1<<31))){
    cpu->APSR.Bits.APSR_N = 1;
  }
}



int main() {
    CortexM0_CPU cpu;
    init_cpu(&cpu);
    printf("CPU init: \n");
    print_cpu_state(&cpu);

    
    cpu.R[0] = 0x7;  // Load a negative number
    update_flags(&cpu, cpu.R[0]);
    printf("cpu status after operation: \n");
    print_cpu_state(&cpu);

    return 0;
}
