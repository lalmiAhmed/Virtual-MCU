#include "Bcond_test.h"

void test_Bcond_EQ(CortexM0_CPU *cpu) {
    uint32_t L_offset = 4;
    uint32_t* pc_reg = &(cpu->R[15]);

    // init_cpu(cpu);
    // cpu->APSR.Bits.APSR_Z = 1;  // Zero flag set
    // Bcond(cpu, L_offset, EQ);
    
    // assert(*pc_reg == L_offset);

    init_cpu(cpu);
    cpu->APSR.Bits.APSR_Z = 0;  // Zero flag clear
    Bcond(cpu, L_offset, EQ);
    assert(*pc_reg == L_offset);
}
