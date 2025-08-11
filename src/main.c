#include <stdint.h>
#include <stdio.h>
#include "main.h"


/* Static function declaration */
static _Bool check_memory_bounds(uint32_t address, uint32_t size);




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
    cpu->APSR.all = 0;      // Clear flags
}

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


static _Bool check_memory_bounds(uint32_t address, uint32_t size) {
  return (address + size - 1 < MEMORY_SIZE);
}


/**
 * @brief Stores the value of a register into memory.
 *
 * This function emulates the STR (Store Register) instruction for a Cortex-M0 CPU.
 * It stores the value from register Rt into the memory address computed by adding
 * the values of registers Rn and Rm.
 *
 * @param cpu Pointer to the CortexM0_CPU structure representing the CPU state.
 * @param Rt  Index of the source register whose value will be stored.
 * @param Rn  Index of the base register used for address calculation.
 * @param Rm  Index of the offset register used for address calculation.
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



/**
 * @brief Stores the halfword from register Rt into memory.
 *
 * This function implements the STRH (Store Register Halfword) instruction for the Cortex-M0 CPU.
 * It stores the lower 16 bits of the value in register Rt into the memory address computed by adding
 * the values of registers Rn and Rm.
 *
 * @param cpu Pointer to the CortexM0_CPU structure representing the CPU state.
 * @param Rt  Index of the source register whose halfword value will be stored.
 * @param Rn  Index of the base register used for address calculation.
 * @param Rm  Index of the offset register used for address calculation.
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


/**
 * @brief Store Byte (STRB) instruction implementation for Cortex-M0 CPU emulator.
 *
 * This function emulates the STRB instruction, which stores the least significant byte
 * from register Rt into the memory address computed by adding the values of registers
 * Rn and Rm.
 *
 * @param cpu Pointer to the CortexM0_CPU structure representing the CPU state.
 * @param Rt  Index of the source register whose least significant byte will be stored.
 * @param Rn  Index of the base register used for address calculation.
 * @param Rm  Index of the register whose value is added to Rn for address calculation.
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


/**
 * @brief Loads a value from memory into a register.
 *
 * This function emulates the LDR (Load Register) instruction for a Cortex-M0 CPU.
 * It loads a value from the memory address computed using the values of registers Rn and Rm,
 * and stores the result into register Rt.
 *
 * @param cpu Pointer to the CortexM0_CPU structure representing the CPU state.
 * @param Rt  Destination register index where the loaded value will be stored.
 * @param Rn  Base register index used to compute the memory address.
 * @param Rm  Offset register index used to compute the memory address.
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


/**
 * @brief Executes the LDRH (Load Register Halfword) instruction for the Cortex-M0 CPU.
 *
 * This function loads a 16-bit halfword from memory into the specified register (Rt).
 * The memory address is calculated using the base register (Rn) and an offset (Rm).
 *
 * @param cpu Pointer to the CortexM0_CPU structure representing the CPU state.
 * @param Rt  Destination register index where the loaded halfword will be stored.
 * @param Rn  Base register index used to calculate the memory address.
 * @param Rm  Offset register index used to calculate the memory address.
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


/**
 * @brief Executes the LDRSH (Load Register Signed Halfword) instruction for the Cortex-M0 CPU.
 *
 * This function loads a 16-bit halfword from memory, sign-extends it to 32 bits,
 * and stores the result in the destination register Rt. The address from which the
 * halfword is loaded is calculated using the base register Rn and the offset register Rm.
 *
 * @param cpu Pointer to the CortexM0_CPU structure representing the CPU state.
 * @param Rt  Destination register index where the loaded value will be stored.
 * @param Rn  Base register index used to calculate the memory address.
 * @param Rm  Offset register index used to calculate the memory address.
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


/**
 * @brief Executes the LDRB (Load Register Byte) instruction for the Cortex-M0 CPU emulator.
 *
 * This function loads a byte from memory, addressed by the sum of registers Rn and Rm,
 * and stores the result into register Rt. The loaded byte is zero-extended to 32 bits.
 *
 * @param cpu Pointer to the CortexM0_CPU structure representing the CPU state.
 * @param Rt  Destination register index where the loaded byte will be stored.
 * @param Rn  Base register index used for memory address calculation.
 * @param Rm  Offset register index added to the base register for address calculation.
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


/**
 * @brief Executes the LDRSB (Load Register Signed Byte) instruction for the Cortex-M0 CPU.
 *
 * This function loads a signed byte from memory, sign-extends it to 32 bits, and stores the result in the destination register (Rt).
 * The memory address is calculated using the base register (Rn) and the offset register (Rm).
 *
 * @param cpu Pointer to the CortexM0_CPU structure representing the CPU state.
 * @param Rt  Destination register index where the loaded value will be stored.
 * @param Rn  Base register index used to calculate the memory address.
 * @param Rm  Offset register index used to calculate the memory address.
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


/**
 * @brief Branches to a specified offset from the current instruction address.
 *
 * This function simulates the B (Branch) instruction for a Cortex-M0 CPU.
 * It updates the program counter (R15) by adding the given signed immediate offset.
 *
 * @param cpu Pointer to the CortexM0_CPU structure representing the CPU state.
 * @param signed_immediate The signed offset to branch to.
 */
void B(CortexM0_CPU *cpu, int32_t signed_immediate) {
  printf("Branching by offset: %d\n", signed_immediate);
  cpu->R[15] += signed_immediate;
}


/**
 * @brief Executes a conditional branch instruction.
 *
 * This function performs a branch operation based on the specified condition.
 * If the condition is met, the program counter (PC) is updated by the given offset.
 *
 * @param cpu Pointer to the CortexM0_CPU structure representing the CPU state.
 * @param offset The signed offset to add to the PC if the condition is true.
 * @param cond The condition code that determines whether the branch is taken.
 */
void Bcond(CortexM0_CPU *cpu, int32_t offset, Condition cond)
{
  switch (cond)
  {
  case EQ: // Equal (Z == 1)
    if (!cpu->APSR.Bits.APSR_Z)
      return;
    break;

  case NE: // Not Equal (Z == 0)
    if (cpu->APSR.Bits.APSR_Z)
      return;
    break;

  case CS: // Carry Set / Unsigned higher or same (C == 1)
    if (!cpu->APSR.Bits.APSR_C)
      return;
    break;

  case CC: // Carry Clear / Unsigned lower (C == 0)
    if (cpu->APSR.Bits.APSR_C)
      return;
    break;

  case MI: // Minus / Negative (N == 1)
    if (!cpu->APSR.Bits.APSR_N)
      return;
    break;

  case PL: // Plus / Positive or Zero (N == 0)
    if (cpu->APSR.Bits.APSR_N)
      return;
    break;

  case VS: // Overflow (V == 1)
    if (!cpu->APSR.Bits.APSR_V)
      return;
    break;

  case VC: // No Overflow (V == 0)
    if (cpu->APSR.Bits.APSR_V)
      return;
    break;

  case HI: // Unsigned higher (C == 1 && Z == 0)
    if (!cpu->APSR.Bits.APSR_C || cpu->APSR.Bits.APSR_Z)
      return;
    break;

  case LS: // Unsigned lower or same (C == 0 || Z == 1)
    if (cpu->APSR.Bits.APSR_C && !cpu->APSR.Bits.APSR_Z)
      return;
    break;

  case GE: // Signed greater than or equal (N == V)
    if (cpu->APSR.Bits.APSR_N != cpu->APSR.Bits.APSR_V)
      return;
    break;

  case LT: // Signed less than (N != V)
    if (cpu->APSR.Bits.APSR_N == cpu->APSR.Bits.APSR_V)
      return;
    break;

  case GT: // Signed greater than (Z == 0 && N == V)
    if (cpu->APSR.Bits.APSR_Z || (cpu->APSR.Bits.APSR_N != cpu->APSR.Bits.APSR_V))
      return;
    break;

  case LE: // Signed less than or equal (Z == 1 || N != V)
    if (!cpu->APSR.Bits.APSR_Z && (cpu->APSR.Bits.APSR_N == cpu->APSR.Bits.APSR_V))
      return;
    break;
  }

  // Branch taken if condition passed
  B(cpu, offset);
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

/********************************************************************
/************************ Debug functions ***************************
*********************************************************************/


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

/**
 * @brief Prints the contents of the memory array.
 *
 * This function iterates through the global Memory array and prints its contents
 * in a formatted manner, displaying each byte in hexadecimal format.
 */
void print_memory(){
  for(int i = 0; i < MEMORY_SIZE; i++){
    printf("[%d] 0x%02X ",i, Memory[i]);
    if(i % 8 == 0) {
      printf("\n");
    }
  }
  printf("\n");
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
