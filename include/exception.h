#ifndef EXCEPTION_H
#define EXCEPTION_H
#include <stdint.h>

uint32_t vector_table[48];


void load_vector_table(uint32_t *memory);


#endif // EXCEPTION_H