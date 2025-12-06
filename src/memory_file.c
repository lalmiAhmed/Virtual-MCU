

#include "memory_file.h"

uint8_t Memory[MEMORY_SIZE];

_Bool check_memory_bounds(uint32_t address, uint32_t size) {
  return (address + size - 1 < MEMORY_SIZE);
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

void print_stack(){
  for(int i = 0; i < Stack_size; i++){
    printf("[%d] 0x%02X ",i, Stack[i]);
    if(i % 8 == 0) {
      printf("\n");
    }
  }
  printf("\n");
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

void PUSH(CortexM0_CPU *cpu, uint32_t value)
{
  
  cpu->SP -= 4;
  assert((cpu->SP & 0x3) == 0); // word alignment
  assert(cpu->SP <= Stack_size - 4);
  // Store the 32-bit register value in stack
  Stack[cpu->SP ] = value & 0xFF;
  Stack[cpu->SP + 1] = (value >> 8) & 0xFF;
  Stack[cpu->SP + 2] = (value >> 16) & 0xFF;
  Stack[cpu->SP + 3] = (value >> 24) & 0xFF;
  
}

uint32_t POP(CortexM0_CPU *cpu)
{
  assert((cpu->SP & 0x3) == 0); // word alignment
  assert(cpu->SP <= Stack_size - 4);
  // Load 32-bit value from the stack into the target register
  uint32_t value = 0; 
  value = Stack[cpu->SP] & 0xFF;
  value |= (Stack[cpu->SP + 1] & 0xFF) << 8;
  value |= (Stack[cpu->SP + 2] & 0xFF) << 16;
  value |= (Stack[cpu->SP + 3] & 0xFF) << 24;

  cpu->SP += 4;
  return value;
}

/**
* @brief Move (immediate) writes an immediate value to the destination register. The condition flags are updated based on
* the result.
*/

void MOVS(CortexM0_CPU *cpu, uint8_t Rd, uint8_t imm8){
  assert(Rd <= 7 && "MOVS #imm8 only supports R0-R7");
  
  cpu->R[Rd] = imm8 & (0xFF);

  update_flags(cpu, (uint32_t)(imm8 & (0xFF)), 0, 0);

  return;
}

/**
 * @brief Move (register) copies the value from the source register to the destination register. The condition flags are updated based on
 * the result.
 */
void MOVS_REG(CortexM0_CPU *cpu, uint8_t Rd, uint8_t Rm){
  assert(Rd <= 7 && "MOVS_REG only supports R0-R7");
  assert(Rm <= 7 && "MOVS_REG only supports R0-R7");  
  cpu->R[Rd] = cpu->R[Rm];
  update_flags(cpu, cpu->R[Rd], 0, 0);
  return;

}

/**
* @brief Logical Shift Left (immediate) shifts the value in the source register left by the specified immediate amount
* and stores the result in the destination register. The condition flags are updated  based on the result.
*/  

void LSL(CortexM0_CPU *cpu, uint8_t Rd, uint8_t Rm,uint32_t immediate){

  assert(Rm < 13 && "LSL: Source only supports R0-R12");
  assert(Rd < 13 && "LSL: Source only supports R0-R12");

  uint32_t shift = immediate & 0x1F; // Only lower 5 bits are valid for shift amount
  uint8_t carry_out = 0;
  
  if (shift == 0) {
    // Shift of zero: carry does NOT change
    carry_out = cpu->APSR.Bits.APSR_C;  // unchanged
    cpu->R[Rd] = cpu->R[Rm];
  }
  else if (shift < 32) {
    carry_out = (cpu->R[Rm] >> (32 - shift)) & 1;  // last bit shifted out
    cpu->R[Rd] = (uint32_t)(cpu->R[Rm] << shift);
  }
  else { 
    // Shift >= 32 → result = 0, carry = 0
    carry_out = 0;
    cpu->R[Rd] = 0;
}

  update_flags(cpu, cpu->R[Rd], carry_out, 0);
  return;
}


/**
* @brief Logical Shift Right (immediate) shifts the value in the source register right by the specified immediate amount
* and stores the result in the destination register. The condition flags are updated  based on the result.
*/  

void LSR(CortexM0_CPU *cpu, uint8_t Rd, uint8_t Rm,uint32_t immediate){

  assert(Rm < 13 && "LSR: Source only supports R0-R12");
  assert(Rd < 13 && "LSR: Source only supports R0-R12");

  uint32_t shift = immediate & 0x1F; // Only lower 5 bits are valid for shift amount
  uint8_t carry_out = 0;
  
  if (shift == 0) {
    //  ARMv6-M semantics: LSR #0 means LSR #32
    carry_out = (cpu->R[Rm] >> 31) & 1;  //  bit 31 becomes carry
    cpu->R[Rd] = 0; // shifting 32 bits → result = 0
  }
  else{
    carry_out = (cpu->R[Rm] >> (shift - 1)) & 1;  // last bit shifted out
    cpu->R[Rd] = (uint32_t)(cpu->R[Rm] >> shift);
  }

  update_flags(cpu, cpu->R[Rd], carry_out, 0);
  return;
}