


#ifndef MEMORY_FILE_H
#define MEMORY_FILE_H

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include "cpu.h"


#define MEMORY_SIZE 56 // to be changed to 5KB
#define Stack_size 12 // to be changed to 1KB

#define FLASH_SIZE 1024
#define SRAM_SIZE 2048

#define WORD_SIZE 4
#define HALFWORD_SIZE 2
#define BYTE_SIZE 1

extern uint8_t Memory[MEMORY_SIZE];
extern uint8_t Stack[Stack_size];
extern uint8_t Flash[FLASH_SIZE];
extern uint8_t SRAM[SRAM_SIZE];


_Bool check_memory_bounds(uint32_t address, uint32_t size);
void print_memory();
void print_stack();

bool mem_read8(uint32_t addr, uint8_t  *value);
bool mem_read16(uint32_t addr, uint16_t *value);
bool mem_read32(uint32_t addr, uint32_t *value);

bool mem_write8 (uint32_t addr, uint8_t  value);
bool mem_write16(uint32_t addr, uint16_t value);
bool mem_write32(uint32_t addr, uint32_t value);

uint8_t* translate_address(uint32_t addr);




#endif // MEMORY_FILE_H