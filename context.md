# Cortex-M0 Virtual MCU Emulator (C Language)

## 👤 Author Context
- Background: Electrical Engineering
- Strong in: C, Embedded Systems, ARM, STM32
- Goal: Build a **Cortex-M0 virtual CPU from scratch in C** for deep understanding

---

# 📌 Project Overview

This project implements a **virtual ARM Cortex-M0 CPU**, including:

- Register set (R0–R15)
- APSR flags (N, Z, C, V)
- Instruction set (subset of Thumb)
- Memory system (byte-addressable)
- Execution engine (Fetch → Decode → Execute)
- Exception and reset handling (planned)

---

# Current Implementation Status

## ✅ CPU Core
- General purpose registers: `R[0..15]`
- PC = `R[15]`
- Stack Pointer handled manually

## ✅ APSR Flags
- N (Negative)
- Z (Zero)
- C (Carry)
- V (Overflow)

## ✅ Implemented Instructions

### Arithmetic
- ADD
- SUB
- CMP

### Logical
- AND
- ORR
- EOR
- TST

### Shift
- LSL (correct carry + edge cases)
- LSR (ARMv6-M correct: shift=0 → shift=32)

### Memory
- STR
- STRH
- STRB
- LDR
- LDRH
- LDRSH
- LDRB
- LDRSB

### Branching
- B (unconditional)
- Bcond (all conditions implemented correctly using APSR)

---

# ⚙️ Key Design Decisions

## Memory Model
- Byte-addressable:
```c
uint8_t Memory[MEMORY_SIZE];
````

## Stack Handling

* Stack uses memory (not separate array)
* PUSH/POP implemented using raw memory writes

## Helper Functions

```c
push_word()
pop_word()
mem_read8/16/32()
mem_write8/16/32()
```

---

# 🧠 APSR / xPSR Handling

* APSR stored as struct
* Converted to/from 32-bit xPSR during exception stacking

```c
uint32_t get_xpsr(...)
void set_xpsr(...)
```

---

# 🧠 Important ARM Behaviors Implemented

## Carry vs Overflow

* Carry → unsigned overflow
* Overflow → signed overflow

## LSL

* shift=0 → no change, carry unchanged
* shift=1–31 → normal
* shift≥32 → result=0, carry=0

## LSR

* shift=0 → treated as shift=32
* carry = bit[31]
* result = 0

---

#  Testing Approach

* Unit tests per instruction
* Manual flag manipulation
* Branch validation via `branch_taken` flag
* Debug prints for instruction tracing

---

# 🚧 Current Focus: Execution Engine

## Goal

Implement full CPU execution loop:

```
FETCH → DECODE → EXECUTE → REPEAT
```

---

#  Execution Engine Plan

## Step 1 — PC Handling

```c
#define PC cpu->R[15]
```

---

## Step 2 — Instruction Fetch

```c
uint16_t fetch16(CortexM0_CPU *cpu);
```

* Uses `mem_read16`
* PC += 2 after fetch

---

## Step 3 — Decoder (Pattern Matching)

Example:

```c
if ((instr & 0xF800) == 0x0000) → LSL
```

---

## Step 4 — Operand Extraction

Example:

```c
imm5 = (instr >> 6) & 0x1F;
Rm   = (instr >> 3) & 0x07;
Rd   = instr & 0x07;
```

---

## Step 5 — Instruction Dispatch

```c
void execute_instruction(cpu, instr);
```

---

## Step 6 — Main Loop

```c
while (1) {
    instr = fetch16(cpu);
    execute_instruction(cpu, instr);
}
```

---

## Step 7 — Branch Handling

* Branch overwrites PC directly
* Do NOT auto-increment after branch

---

## Step 8 — Debugging

```c
printf("PC: 0x%08X INSTR: 0x%04X\n");
```

---

#  Memory System Requirements

## Must Have

* Byte addressing

* Alignment checks:

  * 8-bit → no restriction
  * 16-bit → addr % 2 == 0
  * 32-bit → addr % 4 == 0

* Little-endian format

---

## Memory API

```c
bool mem_read8(...)
bool mem_read16(...)
bool mem_read32(...)
bool mem_write8(...)
bool mem_write16(...)
bool mem_write32(...)
```

---

## Future: Memory Map

| Region      | Address    |
| ----------- | ---------- |
| Flash       | 0x00000000 |
| SRAM        | 0x20000000 |
| Peripherals | 0x40000000 |

---

# ⚡ Next Major Feature: Exceptions

## To Implement

### Reset

* SP = vector_table[0]
* PC = vector_table[1]

### Exception Entry

Push:

* R0–R3
* R12
* LR
* PC
* xPSR

### Exception Return

* Pop all registers
* Restore PC + APSR

---

# 🔜 Next Steps Roadmap

## Immediate

* [ ] Implement fetch16()
* [ ] Implement basic decoder (LSL only)
* [ ] Build execution loop

## Short-Term

* [ ] Add more instruction decoding
* [ ] Integrate memory access in execution
* [ ] Add debug trace

## Mid-Term

* [ ] Exception handling (HardFault, Reset)
* [ ] Vector table support
* [ ] Stack-based exception entry/exit

## Advanced

* [ ] NVIC simulation
* [ ] Run compiled ARM code (ELF)
* [ ] Pipeline behavior (PC + 4)

---

# 🎯 Long-Term Vision

* Fully working Cortex-M0 emulator
* Ability to run real embedded C programs
* Possibly extend to:

  * RTOS simulation
  * Peripheral emulation
  * Debugger interface

---

# 💡 Notes

* Always follow ARMv6-M spec behavior strictly
* Do NOT simplify flags or shifts incorrectly
* Build incrementally — test each instruction


