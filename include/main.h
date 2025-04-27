
#define N_MASK (1<<31)
#define Z_MASK (1<<30)
#define C_MASK (1<<29)
#define V_MASK (1<<28)

#define MEMORY_SIZE 56

#define WORD_SIZE 4
#define HALFWORD_SIZE 2
#define BYTE_SIZE 1

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


uint8_t Memory[MEMORY_SIZE];

void init_cpu(CortexM0_CPU *cpu);
void print_cpu_state(CortexM0_CPU *cpu);
void update_flags(CortexM0_CPU *cpu, uint32_t result, _Bool carry, _Bool overflow);
void ADD(CortexM0_CPU *cpu, uint8_t Rd, uint8_t Rn, uint8_t Rm);
void SUB(CortexM0_CPU *cpu, uint8_t Rd, uint8_t Rn, uint8_t Rm);
void CMP(CortexM0_CPU *cpu, uint8_t Rn, uint8_t Rm);
void AND(CortexM0_CPU *cpu, uint8_t Rn, uint8_t Rm, uint8_t Rd);
void ORR(CortexM0_CPU *cpu, uint8_t Rn, uint8_t Rm, uint8_t Rd);
void EOR(CortexM0_CPU *cpu, uint8_t Rn, uint8_t Rm, uint8_t Rd);
void TST(CortexM0_CPU *cpu, uint8_t Rn, uint8_t Rm);
void STR(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm);
void STRH(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm);
void STRB(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm);
void LDR(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm);
void LDRH(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm);
void LDRSH(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm);
void LDRB(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm);
void LDRSB(CortexM0_CPU *cpu, uint8_t Rt, uint8_t Rn, uint8_t Rm);
void B(CortexM0_CPU *cpu, int32_t signed_immediate);
static _Bool check_memory_bounds(uint32_t address, uint32_t size);
void print_memory();
