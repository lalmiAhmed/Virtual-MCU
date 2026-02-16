#include "cpu.h"
#include "memory_file.h"
#include "exception.h"

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
void init_cpu(CortexM0_CPU *cpu)
{
  for (int i = 0; i < 16; i++)
  {
    cpu->R[i] = 0; // Clear all registers
  }
  cpu->SP = Stack_size - 4;
  cpu->APSR.all = 0; // Clear flags
}

/**
 * @brief Resets the Cortex-M0 CPU structure to its default state.
 *
 * This function clears all registers and flags in the provided CortexM0_CPU structure,
 * effectively resetting the CPU state.
 *
 * @param cpu Pointer to the CortexM0_CPU structure to reset.
 */
void reset_cpu(CortexM0_CPU *cpu)
{
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
void update_flags(CortexM0_CPU *cpu, uint32_t result, _Bool carry, _Bool overflow)
{
  // Clear previous flags
  cpu->APSR.all = 0;

  // Negative
  if ((result & (0x80000000)))
  {
    cpu->APSR.Bits.APSR_N = 1;
    printf("a negative result \n");
  }
  // Zero
  if (result == 0)
  {
    cpu->APSR.Bits.APSR_Z = 1;
    printf("a zero result \n");
  };
  // Carry
  if (carry)
  {
    cpu->APSR.Bits.APSR_C = 1;
    printf("a carry result \n");
  }
  // Overflow
  if (overflow)
  {
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
void print_cpu_state(CortexM0_CPU *cpu)
{
  printf("Registers:\n");
  for (int i = 0; i < 16; i++)
  {
    printf("R%d: 0x%08X\n", i, cpu->R[i]);
  }
  printf("APSR: N=%d Z=%d C=%d V=%d\n",
         cpu->APSR.Bits.APSR_N,
         cpu->APSR.Bits.APSR_Z,
         cpu->APSR.Bits.APSR_C,
         cpu->APSR.Bits.APSR_V);
}

void cpu_reset(CortexM0_CPU *cpu)
{
  cpu->SP = vector_table[0];      // initilize stack pointer
  cpu->PC = vector_table[1] & ~1; // Reset handler (bit0=0 for Thumb)
  cpu->APSR.all = 0;
}

void exception_entry(CortexM0_CPU *cpu, uint8_t exception_number)
{
  // Push registers (simplified)
  PUSH(cpu, cpu->APSR.all);
  PUSH(cpu, cpu->PC);
  PUSH(cpu, cpu->LR);
  PUSH(cpu, cpu->R[12]);
  PUSH(cpu, cpu->R[3]);
  PUSH(cpu, cpu->R[2]);
  PUSH(cpu, cpu->R[1]);
  PUSH(cpu, cpu->R[0]);

  // cpu->LR = 0xFFFFFFF9; // Return to Thread mode using MSP
  cpu->PC = vector_table[exception_number] & ~1; // Jump to handler
}

void exception_return(CortexM0_CPU *cpu)
{
  cpu->R[0] = POP(cpu);
  cpu->R[1] = POP(cpu);
  cpu->R[2] = POP(cpu);
  cpu->R[3] = POP(cpu);
  cpu->R[12] = POP(cpu);
  cpu->LR = POP(cpu);
  cpu->PC = POP(cpu);
  cpu->APSR.all = POP(cpu);
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
void STR(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm)
{
  // Ensure register is valid (not SP, LR, or PC)
  if (Rt >= 13)
  {
    printf("Invalid register for STR\n");
    return;
  }

  // Compute memory address
  uint32_t addr = cpu->R[Rm] + cpu->R[Rn];
  uint32_t value = cpu->R[Rt];

  if (!mem_write32(addr, value))
  {
    raise_hardfault(cpu);
    return;
  }
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
void STRH(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm)
{
  if (Rt >= 13)
  {
    printf("Invalid register for STRH\n");
    return;
  }

  uint32_t addr = cpu->R[Rn] + cpu->R[Rm];
  uint16_t value = cpu->R[Rt] & 0xFFFF;

  if (!mem_write16(addr, value))
  {
    raise_hardfault(cpu);
    return;
  }
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
void STRB(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm)
{
  if (Rt >= 13)
  {
    printf("Invalid register for STRB\n");
    return;
  }

  uint32_t addr = cpu->R[Rn] + cpu->R[Rm];
  uint8_t value = cpu->R[Rt] & 0xFFU;

  if (!mem_write8(addr, value))
  {
    raise_hardfault(cpu);
    return;
  }
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
void LDR(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm)
{
  // Ensure register is valid (not SP, LR, or PC)
  check_Rt_validity(Rt, "LDR");

  // Compute memory address
  uint32_t addr = cpu->R[Rm] + cpu->R[Rn];

  uint32_t value;
  if (!mem_read32(addr, &value))
  {
    raise_hardfault(cpu);
    return;
  }
  cpu->R[Rt] = value;
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
void LDRB(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm)
{
  // Ensure register is valid (not SP, LR, or PC)
  check_Rt_validity(Rt, "LDRB");

  // Compute memory address
  uint32_t addr = cpu->R[Rm] + cpu->R[Rn];

  int32_t value;
  if (!mem_read8(addr, &value))
  {
    raise_hardfault(cpu);
    return;
  }
  cpu->R[Rt] = value;
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
void LDRH(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm)
{
  // Ensure register is valid (not SP, LR, or PC)
  check_Rt_validity(Rt, "LDRH");

  // Compute memory address
  uint32_t addr = cpu->R[Rm] + cpu->R[Rn];

  int32_t value;
  if (!mem_read16(addr, &value))
  {
    raise_hardfault(cpu);
    return;
  }
  cpu->R[Rt] = value;
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
void LDRSH(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm)
{
 check_Rt_validity(Rt, "LDRSH");

  uint32_t addr = cpu->R[Rn] + cpu->R[Rm];
  int16_t halfword;
  
  if (!mem_read16(addr, &halfword))
  {
    raise_hardfault(cpu);
    return;
  }
  
  cpu->R[Rt] = (int32_t)halfword; // Sign-extend to 32-bit
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
void LDRSB(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm)
{
  // Ensure register is valid (not SP, LR, or PC)
  check_Rt_validity(Rt, "LDRSB");

  // Compute memory address
  uint32_t addr = cpu->R[Rm] + cpu->R[Rn];

  int8_t byte;
  if (!mem_read16(addr, &byte))
  {
    raise_hardfault(cpu);
    return;
  }
  // Load signed 8-bit and sign-extend to 32-bit
  cpu->R[Rt] = (int32_t)byte;
}


/**
 * @brief Move (immediate) writes an immediate value to the destination register. The condition flags are updated based on
 * the result.
 */

void MOVS(CortexM0_CPU *cpu, uint8_t Rd, uint8_t imm8)
{
  assert(Rd <= 7 && "MOVS #imm8 only supports R0-R7");

  cpu->R[Rd] = imm8 & (0xFF);

  update_flags(cpu, (uint32_t)(imm8 & (0xFF)), 0, 0);

  return;
}

/**
 * @brief Move (register) copies the value from the source register to the destination register. The condition flags are updated based on
 * the result.
 */
void MOVS_REG(CortexM0_CPU *cpu, uint8_t Rd, uint8_t Rm)
{
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

void LSL(CortexM0_CPU *cpu, uint8_t Rd, uint8_t Rm, uint32_t immediate)
{

  assert(Rm < 13 && "LSL: Source only supports R0-R12");
  assert(Rd < 13 && "LSL: Source only supports R0-R12");

  uint32_t shift = immediate & 0x1F; // Only lower 5 bits are valid for shift amount
  uint8_t carry_out = 0;

  if (shift == 0)
  {
    // Shift of zero: carry does NOT change
    carry_out = cpu->APSR.Bits.APSR_C; // unchanged
    cpu->R[Rd] = cpu->R[Rm];
  }
  else if (shift < 32)
  {
    carry_out = (cpu->R[Rm] >> (32 - shift)) & 1; // last bit shifted out
    cpu->R[Rd] = (uint32_t)(cpu->R[Rm] << shift);
  }
  else
  {
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
void LSR(CortexM0_CPU *cpu, uint8_t Rd, uint8_t Rm, uint32_t immediate)
{

  assert(Rm < 13 && "LSR: Rm supports R0-R12");
  assert(Rd < 13 && "LSR: Rd supports R0-R12");

  uint32_t shift = immediate & 0x1F; // Only lower 5 bits are valid for shift amount
  uint8_t carry_out = 0;

  if (shift == 0)
  {
    //  ARMv6-M semantics: LSR #0 means LSR #32
    carry_out = (cpu->R[Rm] >> 31) & 1; //  bit 31 becomes carry
    cpu->R[Rd] = 0;                     // shifting 32 bits → result = 0
  }
  else
  {
    carry_out = (cpu->R[Rm] >> (shift - 1)) & 1; // last bit shifted out
    cpu->R[Rd] = (uint32_t)(cpu->R[Rm] >> shift);
  }

  update_flags(cpu, cpu->R[Rd], carry_out, 0);
  return;
}

void raise_hardfault(CortexM0_CPU *cpu){
  // cpu->exception_pending = HARDFAULT;
  printf("HardFault raised due to invalid memory access or unaligned access.\n");
  return;
}

void check_Rt_validity(uint8_t Rt, const char *instruction_name)
{
  if (Rt >= 13)
  {
    printf("Invalid register for %s\n", instruction_name);
    return;
  }
}