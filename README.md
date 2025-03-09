# Virtual-MCU
Virtual ARM cortex-M0 MCU implemented in C programming language

### 🏗 **1. System Architecture Design**

- **CPU Core**
    - Registers: **R0–R12, SP, LR, PC, APSR**.
    - Pipeline: **3-stage fetch-decode-execute**.
    - Instruction set: **ARMv6-M (Thumb-1 only)**.
- **Memory Model**
    - ROM (Flash) → Holds program instructions.
    - RAM → Stores stack, variables, and runtime data.
    - MMIO → Simulated registers for peripherals.
- **Peripherals (Optional, Later Stages)**
    - GPIO (simple input/output).
    - SysTick Timer (basic timer functionality).
    - UART (for serial output debugging).

### 🛠 **Implementation Plan**

### 🔹 **1. CPU Core Development**

1. Implement **register set** and status flags.
2. Implement **memory read/write operations**.
3. Implement **instruction decoding & execution**:
    - **Data processing** (ADD, SUB, MOV, CMP, etc.).
    - **Memory access** (LDR, STR).
    - **Branching** (B, BL, BX).
    - **Stack operations** (PUSH, POP).
4. Implement **Exception and Reset handling**.

### 🔹 **2. Memory System**

1. Implement **virtual memory** layout.
2. Create **program flash memory (ROM)**.
3. Create **RAM** with simple heap/stack.
4. Implement **MMIO** (for peripherals later).

### 🔹 **3. Execution Engine**

1. Implement **Fetch**: Read instruction from ROM.
2. Implement **Decode**: Identify instruction type.
3. Implement **Execute**: Perform operation.

### 🔹 **4. Debugging & Logging**

- Print **register states** after each instruction.
- Implement **breakpoints** for step execution.
- Implement **simple CLI** (e.g., `run`, `step`, `dump memory`).

### 🔹 **5. Add Peripherals (Future)**

- GPIO (Simple LED toggle example).
- UART (Simulated serial console).
- SysTick (Basic timer interrupts).


🎯 Next Steps

    Implement load and store for other data types (8bit, 16bit)/ signed and unsigned:
    https://developer.arm.com/documentation/ddi0419/c/Application-Level-Architecture/The-ARMv6-M-Instruction-Set/Load-and-store-instructions?lang=en
