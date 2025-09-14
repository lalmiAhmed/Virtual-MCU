#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "cpu.h"
#include "alu.h"
#include "memory_file.h"
#include "test_mod.h"



int main() {
    CortexM0_CPU cpu;
    init_cpu(&cpu);
    // printf("CPU init: \n");
    // print_cpu_state(&cpu);

    test_Bcond_EQ(&cpu);

    print_cpu_state(&cpu);
    print_memory();

    return 0;
}
