#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include "main.h"

int branch_taken = 0;

void B(CortexM0_CPU *cpu, int32_t offset) {
    branch_taken = 1;
    cpu->R[15] += offset; // Simple simulation for testing
}

void reset_cpu(CortexM0_CPU *cpu) {
    memset(cpu, 0, sizeof(CortexM0_CPU));
    branch_taken = 0;
}

void test_Bcond_EQ() {
    CortexM0_CPU cpu;
    reset_cpu(&cpu);

    cpu.APSR.Bits.APSR_Z = 1;  // Zero flag set
    Bcond(&cpu, 4, EQ);
    assert(branch_taken == 1);

    reset_cpu(&cpu);
    cpu.APSR.Bits.APSR_Z = 0;  // Zero flag clear
    Bcond(&cpu, 4, EQ);
    assert(branch_taken == 0);
}

void test_Bcond_NE() {
    CortexM0_CPU cpu;
    reset_cpu(&cpu);

    cpu.APSR.Bits.APSR_Z = 0;
    Bcond(&cpu, 4, NE);
    assert(branch_taken == 1);

    reset_cpu(&cpu);
    cpu.APSR.Bits.APSR_Z = 1;
    Bcond(&cpu, 4, NE);
    assert(branch_taken == 0);
}

void test_Bcond_GE_LT_GT_LE() {
    CortexM0_CPU cpu;

    // GE: N == V
    reset_cpu(&cpu);
    cpu.APSR.Bits.APSR_N = 0;
    cpu.APSR.Bits.APSR_V = 0;
    Bcond(&cpu, 4, GE);
    assert(branch_taken == 1);

    reset_cpu(&cpu);
    cpu.APSR.Bits.APSR_N = 1;
    cpu.APSR.Bits.APSR_V = 0;
    Bcond(&cpu, 4, GE);
    assert(branch_taken == 0);

    // LT: N != V
    reset_cpu(&cpu);
    cpu.APSR.Bits.APSR_N = 1;
    cpu.APSR.Bits.APSR_V = 0;
    Bcond(&cpu, 4, LT);
    assert(branch_taken == 1);

    reset_cpu(&cpu);
    cpu.APSR.Bits.APSR_N = 1;
    cpu.APSR.Bits.APSR_V = 1;
    Bcond(&cpu, 4, LT);
    assert(branch_taken == 0);

    // GT: Z == 0 && N == V
    reset_cpu(&cpu);
    cpu.APSR.Bits.APSR_Z = 0;
    cpu.APSR.Bits.APSR_N = 0;
    cpu.APSR.Bits.APSR_V = 0;
    Bcond(&cpu, 4, GT);
    assert(branch_taken == 1);

    // LE: Z == 1 || N != V
    reset_cpu(&cpu);
    cpu.APSR.Bits.APSR_Z = 1;
    Bcond(&cpu, 4, LE);
    assert(branch_taken == 1);

    reset_cpu(&cpu);
    cpu.APSR.Bits.APSR_N = 1;
    cpu.APSR.Bits.APSR_V = 0;
    Bcond(&cpu, 4, LE);
    assert(branch_taken == 1);
}

int main() {
    test_Bcond_EQ();
    test_Bcond_NE();
    test_Bcond_GE_LT_GT_LE();

    printf("All conditional branch tests passed.\n");
    return 0;
}
