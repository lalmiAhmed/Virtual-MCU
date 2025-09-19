#include "branch.h"


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


void BLX(CortexM0_CPU *cpu, uint8_t Rm, uint8_t inst_size)
{
  assert((inst_size == 2) || (inst_size == 4));

  cpu->R[14] = cpu->R[15]+ inst_size; 
  uint32_t target = cpu->R[Rm];

  // Check Thumb bit (bit0 must be 1)
  assert((target & 0x1) == 1);

  // Force PC = target with bit0 cleared
  cpu->R[15] = target & ~1U;
}


void BX(CortexM0_CPU *cpu, uint8_t Rm)
{
    uint32_t target = cpu->R[Rm];

    // Thumb bit must be set
    assert((target & 0x1) == 1);

    // Update PC with bit0 cleared
    cpu->R[15] = target & ~1U;
}

