# 8-bit CPU SystemC Simulator

A SystemC implementation of an 8-bit CPU inspired by the 6502 architecture. This project simulates a complete processor with modular components including ALU, control unit, register file, and memory.

## How it Works

The CPU simulator loads hex programs from text files and executes them cycle by cycle. The architecture consists of:

- **ALU** - Arithmetic and logic operations (add, subtract, bitwise)
- **Control Unit** - Instruction fetch, decode, and execute logic
- **Register File** - CPU registers (A, X, Y, status flags, stack pointer)
- **Memory** - 64KB addressable memory space

## Quick Start

### Build and Run

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Debug

# Run with default program
./Debug/cpu.exe

# Run with custom program (command line argument)
./Debug/cpu.exe ../programs/hello.txt
```

## Writing Programs

Programs are written in hex format with comments. Each line can contain hex bytes separated by spaces:

```assembly
# Simple program example
A9 42    # LDA #0x42 - Load 0x42 into accumulator
8D 00 02 # STA $0200 - Store accumulator to memory address 0x0200
00       # BRK - Break (halt processor)
```

### Program Format Rules

- **Hex bytes**: Space-separated (e.g., `A9 42`)
- **Comments**: Lines starting with `#` or inline after `#`
- **Empty lines**: Ignored

### Sample Programs

Check the `programs/` directory:

- `hello.txt` - ASCII output demo
- `add2num.txt` - Addition example  
- `lda_simple.txt` - Basic load operations

### Command Line Usage

Pass any program file as an argument:

```bash
./cpu.exe path/to/your/program.txt
```

If no argument is provided, it uses the default program (`programs/hello.txt`).

## Supported Instructions

Supports most of the basic instructions, 
but this project is still work in progress so some
instructions may not work as expected

## Output

The simulator shows:
- Register states (A, PC, IR)
- Execution cycles
- I/O port output (saved to `output/io_output.txt`)

Example output:
```
=== CPU 6502 Simulation Start ===
Loading program: ../programs/hello.txt
Loaded 24 bytes
=== Simulation Result ===
Register A: 0x6c
PC: 0x18
```

