

#include "memory_file.h"

uint8_t Memory[MEMORY_SIZE];
uint8_t Flash[FLASH_SIZE];
uint8_t SRAM[SRAM_SIZE];

_Bool check_memory_bounds(uint32_t address, uint32_t size) {
  return (address + size - 1 < MEMORY_SIZE);
}

/**
 * @brief Prints the contents of the memory array.
 *
 * This function iterates through the global Memory array and prints its contents
 * in a formatted manner, displaying each byte in hexadecimal format.
 */
void print_memory(){
  for(int i = 0; i < MEMORY_SIZE; i++){
    printf("[%d] 0x%02X ",i, Memory[i]);
    if(i % 8 == 0) {
      printf("\n");
    }
  }
  printf("\n");
}

void print_stack(){
  for(int i = 0; i < Stack_size; i++){
    printf("[%d] 0x%02X ",i, Stack[i]);
    if(i % 8 == 0) {
      printf("\n");
    }
  }
  printf("\n");
}


bool mem_read8(uint32_t addr, uint8_t  *value)
{
    if (addr >= MEMORY_SIZE) return false;

    *value = Memory[addr];
    return true;
}

bool mem_read16(uint32_t addr, uint16_t *value)
{
    if (addr + 1 >= MEMORY_SIZE) return false;
    if (addr & 1) {
        // Unaligned halfword → HardFault (later)
        return false;
    }

    *value = Memory[addr] |
            (Memory[addr + 1] << 8);
    return true;
}

bool mem_read32(uint32_t addr, uint32_t *value)
{
    if (addr + 3 >= MEMORY_SIZE) return false;
    if (addr & 3) {
        // Unaligned word → HardFault (later)
        return false;
    }

    *value = Memory[addr] |
            (Memory[addr + 1] << 8) |
            (Memory[addr + 2] << 16) |
            (Memory[addr + 3] << 24);
    return true;
}

bool mem_write8(uint32_t addr, uint8_t  value){
  
  if (addr >= MEMORY_SIZE) return false;
  
  Memory[addr] = value;
  return true;
}

bool mem_write16(uint32_t addr, uint16_t value){
  if (addr + 1 >= MEMORY_SIZE) return false;
  if (addr & 1) { // addr % 2 == 0
        // Unaligned halfword → HardFault (later)
        return false;
    }
  Memory[addr]     = (uint8_t)(value & 0xFF);
  Memory[addr + 1] = (uint8_t)(value >> 8);
  return true;
  
}

bool mem_write32(uint32_t addr, uint32_t value){
  if (addr + 3 >= MEMORY_SIZE) return false;
  if (addr & 3) { // addr % 4 == 0
        // Unaligned halfword → HardFault (later)
        return false;
    }
  Memory[addr]     = (uint8_t)(value & 0xFF);
  Memory[addr + 1] = (uint8_t)((value >> 8) & 0xFF);
  Memory[addr + 2] = (uint8_t)((value >> 16) & 0xFF);
  Memory[addr + 3] = (uint8_t)((value >> 24) & 0xFF);
  return true;
  
}

uint8_t* translate_address(uint32_t addr){
    if (addr < FLASH_SIZE) {
        return &Flash[addr];
    } else if (addr >= 0x20000000 && addr < 0x20000000 + SRAM_SIZE) {
        return &SRAM[addr - 0x20000000];
    } else {
        return NULL; // Invalid address
    }
}

