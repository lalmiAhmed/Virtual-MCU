#ifndef EXCEPTION_H
#define EXCEPTION_H
#include <stdint.h>

#define VECTOR_TABLE_SIZE 48
extern uint32_t vector_table[];


void load_vector_table(uint32_t *memory);


#endif // EXCEPTION_H