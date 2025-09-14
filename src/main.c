#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "cpu.h"
#include "alu.h"
#include "memory_file.h"
#include "Bcond_test.h"



int main() {
    CortexM0_CPU cpu;
    init_cpu(&cpu);
    // printf("CPU init: \n");
    // print_cpu_state(&cpu);


    // cpu.R[0] = 0x7;  // Load a negative number
    // update_flags(&cpu, cpu.R[0], 0, 0);
    // printf("cpu status after operation: \n");

    // cpu.R[1] = 0x4000003C;
    // cpu.R[2] = 0x10;
    // cpu.R[3] = 0X0;
    // STRB(&cpu, 1, 2, 3);


    // Memory[20] = 0x3C;
    // Memory[21] = 0xFD;
    // Memory[22] = 0xAA;
    // Memory[23] = 0xBB;

    // cpu.R[2] = 0x14;
    // LDR(&cpu, 4, 2, 3);
    // LDRH(&cpu, 5, 2, 3);

    test_Bcond_EQ(&cpu);

    print_cpu_state(&cpu);
    print_memory();

    return 0;
}
