#include "alu.h"


/**
 * @brief Performs the ADD operation for the CortexM0 CPU.
 *
 * This function adds the values of registers Rn and Rm, and stores the result in register Rd.
 * It also updates the relevant CPU flags as per the ARM Cortex-M0 architecture specification.
 *
 * @param cpu Pointer to the CortexM0_CPU structure representing the CPU state.
 * @param Rd  Destination register index where the result will be stored.
 * @param Rn  First operand register index.
 * @param Rm  Second operand register index.
 */
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

/**
 * @brief Performs the SUB (subtract) operation on the CortexM0 CPU registers.
 *
 * This function subtracts the value in register Rm from the value in register Rn,
 * and stores the result in register Rd. It also updates the relevant CPU flags
 * according to the result of the subtraction, following the ARM Cortex-M0 instruction set.
 *
 * @param cpu Pointer to the CortexM0_CPU structure representing the CPU state.
 * @param Rd  Destination register index where the result will be stored.
 * @param Rn  First operand register index (minuend).
 * @param Rm  Second operand register index (subtrahend).
 */
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

/**
 * @brief Compares the values of two CPU registers and updates condition flags.
 *
 * This function performs a comparison between the values stored in registers Rn and Rm
 * of the provided CortexM0_CPU structure. The result of the comparison is not stored,
 * but the CPU's condition flags (such as Zero, Negative, Carry, and Overflow) are updated
 * to reflect the outcome of the subtraction (Rn - Rm).
 *
 * @param cpu Pointer to the CortexM0_CPU structure representing the CPU state.
 * @param Rn Index of the first register to compare.
 * @param Rm Index of the second register to compare.
 */
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

/**
 * @brief Performs a bitwise AND operation between two registers and stores the result.
 *
 * This function takes the values from registers Rn and Rm, performs a bitwise AND operation,
 * and stores the result in register Rd within the provided CortexM0_CPU structure.
 *
 * @param cpu Pointer to the CortexM0_CPU structure representing the CPU state.
 * @param Rn Index of the first source register.
 * @param Rm Index of the second source register.
 * @param Rd Index of the destination register where the result will be stored.
 */
void AND(CortexM0_CPU *cpu, uint8_t Rn, uint8_t Rm, uint8_t Rd){
  uint32_t op1 = cpu->R[Rn];
  uint32_t op2 = cpu->R[Rm];
  uint32_t result = op1 & op2;
  _Bool carry = cpu->APSR.Bits.APSR_C;   // Keep previous carry unchanged
  _Bool overflow = cpu->APSR.Bits.APSR_V; // Keep previous overflow unchanged
  cpu->R[Rd] = result;
  update_flags(cpu, result, carry, overflow);
}

/**
 * @brief Performs a bitwise OR operation between two registers and stores the result in a destination register.
 *
 * This function simulates the ORR (bitwise OR) instruction for a Cortex-M0 CPU.
 *
 * @param cpu Pointer to the CortexM0_CPU structure representing the CPU state.
 * @param Rn Index of the first source register.
 * @param Rm Index of the second source register.
 * @param Rd Index of the destination register where the result will be stored.
 */
void ORR(CortexM0_CPU *cpu, uint8_t Rn, uint8_t Rm, uint8_t Rd){
  uint32_t op1 = cpu->R[Rn];
  uint32_t op2 = cpu->R[Rm];
  uint32_t result = op1 | op2;
  _Bool carry = cpu->APSR.Bits.APSR_C;   // Keep previous carry unchanged
  _Bool overflow = cpu->APSR.Bits.APSR_V; // Keep previous overflow unchanged
  cpu->R[Rd] = result;
  update_flags(cpu, result, carry, overflow);
}

/**
 * @brief Performs a bitwise exclusive OR operation between two registers and stores the result in a destination register.
 *
 * This function simulates the EOR (exclusive OR) instruction for a Cortex-M0 CPU.
 *
 * @param cpu Pointer to the CortexM0_CPU structure representing the CPU state.
 * @param Rn Index of the first source register.
 * @param Rm Index of the second source register.
 * @param Rd Index of the destination register where the result will be stored.
 */
void EOR(CortexM0_CPU *cpu, uint8_t Rn, uint8_t Rm, uint8_t Rd){
  uint32_t op1 = cpu->R[Rn];
  uint32_t op2 = cpu->R[Rm];
  uint32_t result = op1 ^ op2;
  _Bool carry = cpu->APSR.Bits.APSR_C;   // Keep previous carry unchanged
  _Bool overflow = cpu->APSR.Bits.APSR_V; // Keep previous overflow unchanged
  cpu->R[Rd] = result;
  update_flags(cpu, result, carry, overflow);
}

/**
 * @brief Performs a bitwise test operation between two registers.
 *
 * This function performs a bitwise AND operation between the values in registers Rn and Rm,
 * but does not store the result. Instead, it updates the CPU flags based on the result of the operation.
 *
 * @param cpu Pointer to the CortexM0_CPU structure representing the CPU state.
 * @param Rn Index of the first source register.
 * @param Rm Index of the second source register.
 */
void TST(CortexM0_CPU *cpu, uint8_t Rn, uint8_t Rm){
  uint32_t op1 = cpu->R[Rn];
  uint32_t op2 = cpu->R[Rm];
  uint32_t result = op1 & op2;
  _Bool carry = cpu->APSR.Bits.APSR_C;   // Keep previous carry unchanged
  _Bool overflow = cpu->APSR.Bits.APSR_V; // Keep previous overflow unchanged
  update_flags(cpu, result, carry, overflow);
}

