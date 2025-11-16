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

    // test_Bcond_EQ(&cpu);
    MOVS(&cpu, 8, 0x12);
    

    // uint32_t *tmp_sp = cpu.SP;
    // printf("Stack ends at :%p\n", (uint32_t *)&(Stack[Stack_size-1]));
    print_cpu_state(&cpu);
    

    PUSH(&cpu, 1);
    print_cpu_state(&cpu);
    // print_memory();
    print_stack();

    printf("Poping back the value to R2 \n");
    cpu.R[2] = POP(&cpu);
    STR(&cpu, 2, 15, 0);
    print_cpu_state(&cpu);
    print_stack();
    print_memory();


    return 0;
}
