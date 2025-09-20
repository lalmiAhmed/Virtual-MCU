#include "cpu.h"
#include "memory_file.h"

uint8_t Stack[Stack_size];

/**
 * @brief Initializes the Cortex-M0 CPU structure.
 *
 * This function sets up the initial state of the provided CortexM0_CPU structure,
 * preparing it for use in the virtual MCU environment. It typically initializes
 * registers, memory pointers, and any other necessary CPU state.
 *
 * @param cpu Pointer to the CortexM0_CPU structure to initialize.
 */
void init_cpu(CortexM0_CPU *cpu) {
    for (int i = 0; i < 16; i++) {
        cpu->R[i] = 0;  // Clear all registers
    }
    cpu->SP = Stack_size -1;
    cpu->APSR.all = 0;      // Clear flags
}

/**
 * @brief Resets the Cortex-M0 CPU structure to its default state.
 *
 * This function clears all registers and flags in the provided CortexM0_CPU structure,
 * effectively resetting the CPU state.
 *
 * @param cpu Pointer to the CortexM0_CPU structure to reset.
 */
void reset_cpu(CortexM0_CPU *cpu) {
    memset(cpu, 0, sizeof(CortexM0_CPU));
}

/**
 * @brief Updates the CPU flags based on the result of an operation.
 *
 * This function updates the Negative, Zero, Carry, and Overflow flags in the APSR
 * based on the result of an arithmetic operation.
 *
 * @param cpu Pointer to the CortexM0_CPU structure representing the CPU state.
 * @param result The result of the operation to evaluate flags for.
 * @param carry Indicates if there was a carry from the operation.
 * @param overflow Indicates if there was an overflow from the operation.
 */
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

/**
 * @brief Prints the current state of the Cortex-M0 CPU.
 *
 * This function outputs the values of the CPU registers and other relevant
 * state information for debugging or inspection purposes.
 *
 * @param cpu Pointer to a CortexM0_CPU structure representing the CPU whose state is to be printed.
 */
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
