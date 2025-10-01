#pragma once
#include <systemc.h>


// Simple control unit 6502 style

SC_MODULE(control_unit) {
    sc_in<bool> clk;
    sc_in<sc_uint<8>> opcode; // 6502 operation code

    // Control signals for ALU
    sc_out<sc_uint<4>> alu_op;      // ALU operation code
    sc_out<bool> alu_enable;        // Enable ALU operation
    sc_out<bool> set_flags;         // Set flags

    // Control signals for P flags
    sc_out<bool> set_carry;         // set Carry flag
    sc_out<bool> clear_carry;       // clear Carry flag
    sc_out<bool> set_interrupt;     // set Interrupt Disable flag
    sc_out<bool> clear_interrupt;   // clear Interrupt Disable flag
    sc_out<bool> set_decimal;       // set Decimal Mode flag
    sc_out<bool> clear_decimal;     // clear Decimal Mode flag
    sc_out<bool> clear_overflow;    // clear Overflow flag

    // Control signals for registers
    sc_out<bool> reg_we;            // write enable for registers
    sc_out<sc_uint<3>> reg_sel;     // choose register to write (e.g. A, X, Y)
    sc_out<sc_uint<3>> reg_src;     // choose source register (e.g. for transfers)

    // Control signals for memory
    sc_out<bool> mem_we;            // write enable for memory
    sc_out<bool> mem_oe;            // output enable (reading from memory)

    // Control signals for Program Counter (PC)
    sc_out<bool> pc_inc;            // inc PC
    sc_out<bool> pc_load;           // load PC with new value
    sc_out<sc_uint<16>> pc_new;     // new PC value (jump/subroutine address)

    // System signals
    sc_out<bool> halt;              // stop CPU
    sc_out<bool> irq_ack;           // interrupt request acknowledge
    sc_out<bool> nmi_ack;           // non-maskable interrupt acknowledge


    void process() {
        // Reset control signals at the beginning of each cycle
        set_carry.write(false);
        clear_carry.write(false);
        set_interrupt.write(false);
        clear_interrupt.write(false);
        set_decimal.write(false);
        clear_decimal.write(false);
        clear_overflow.write(false);
        
        switch(opcode.read()) {
            // --- Load/Store Operations ---
            case 0xA9: /* LDA #imm (Load Accumulator Immediate)
                A = immediate value (operand after opcode)
                Sets Z (zero) and N (negative) flags
                Addressing mode: immediate
            */
                alu_op.write(0xB);      // MOV (pass operand -> A)
                alu_enable.write(false); // disable ALU
                reg_sel.write(0);       // select register A
                reg_we.write(true);     // write to register
                set_flags.write(true);  // set Z and N flags
                mem_we.write(false);    // don't write to memory
                mem_oe.write(false);    // don't read from memory (operand already fetched)
                pc_inc.write(true);     // go to next instruction
                halt.write(false);
                break;
            case 0xA5: /* LDA zp (Load Accumulator from Zero Page)
                A = value from memory at zero page address
                Sets Z (zero) and N (negative) flags
                Addressing mode: zero page
            */
                alu_op.write(0xB);      // MOV (pass operand -> A)
                alu_enable.write(false); // disable ALU
                reg_sel.write(0);       // select register A
                reg_we.write(true);     // write to register
                set_flags.write(true);  // set Z and N flags
                mem_we.write(false);    // don't write to memory
                mem_oe.write(true);     // read from memory
                pc_inc.write(true);     // go to next instruction
                halt.write(false);
                break;
            case 0xB5: /* LDA zp,X (Load Accumulator from Zero Page,X)
                A = value from memory at address (zero page + X)
                Sets Z (zero) and N (negative) flags
                Addressing mode: zero page,X
                */
                alu_op.write(0xB);      // MOV (pass operand -> A)
                alu_enable.write(false); // disable ALU
                reg_sel.write(0);       // select register A
                reg_we.write(true);     // write to register
                set_flags.write(true);  // set Z and N flags
                mem_we.write(false);    // don't write to memory
                mem_oe.write(true);     // read from memory
                pc_inc.write(true);     // go to next instruction
                halt.write(false);
                break;
            case 0xAD: /* LDA abs (Load Accumulator from Absolute Address)
                A = value from memory at absolute address
                Sets Z (zero) and N (negative) flags
                Addressing mode: absolute
            */
                alu_op.write(0xB);      // MOV (pass operand -> A)
                alu_enable.write(false); // disable ALU
                reg_sel.write(0);       // select register A
                reg_we.write(true);     // write to register
                set_flags.write(true);  // set Z and N flags
                mem_we.write(false);    // don't write to memory
                mem_oe.write(true);     // read from memory
                pc_inc.write(true);     // go to next instruction
                halt.write(false);
                break;
            case 0xBD: /* LDA abs,X (Load Accumulator from Absolute Address + X)
                A = value from memory at address (abs + X)
                Sets Z (zero) and N (negative) flags
                Addressing mode: absolute,X
            */
                alu_op.write(0xB);      // MOV (pass operand -> A)
                alu_enable.write(false); // disable ALU
                reg_sel.write(0);       // select register A
                reg_we.write(true);     // write to register
                set_flags.write(true);  // set Z and N flags
                mem_we.write(false);    // don't write to memory
                mem_oe.write(true);     // read from memory
                pc_inc.write(true);     // go to next instruction
                halt.write(false);
                break;
            case 0xB9: /* LDA abs,Y (Load Accumulator from Absolute Address + Y)
                A = value from memory at address (abs + Y)
                Sets Z (zero) and N (negative) flags
                Addressing mode: absolute,Y
            */
                alu_op.write(0xB);      // MOV (pass operand -> A)
                alu_enable.write(false); // disable ALU
                reg_sel.write(0);       // select register A
                reg_we.write(true);     // write to register
                set_flags.write(true);  // set Z and N flags
                mem_we.write(false);    // don't write to memory
                mem_oe.write(true);     // read from memory
                pc_inc.write(true);     // go to next instruction
                halt.write(false);
                break;
            case 0xA1: /* LDA (ind,X) (Load Accumulator from (Zero Page + X) indirect)
                A = value from memory at address pointed to by (zero page + X)
                Sets Z (zero) and N (negative) flags
                Addressing mode: (indirect,X)
            */
                alu_op.write(0xB);      // MOV (pass operand -> A)
                alu_enable.write(false); // disable ALU
                reg_sel.write(0);       // select register A
                reg_we.write(true);     // write to register
                set_flags.write(true);  // set Z and N flags
                mem_we.write(false);    // don't write to memory
                mem_oe.write(true);     // read from memory
                pc_inc.write(true);     // go to next instruction
                halt.write(false);
                break;
            case 0xB1: /* LDA (ind),Y (Load Accumulator from (Zero Page) indirect + Y)
                A = value from memory at address pointed to by (zero page) + Y
                Sets Z (zero) and N (negative) flags
                Addressing mode: (indirect),Y
            */
                alu_op.write(0xB);      // MOV (pass operand -> A)
                alu_enable.write(false); // disable ALU
                reg_sel.write(0);       // select register A
                reg_we.write(true);     // write to register
                set_flags.write(true);  // set Z and N flags
                mem_we.write(false);    // don't write to memory
                mem_oe.write(true);     // read from memory
                pc_inc.write(true);     // go to next instruction
                halt.write(false);
                break;

            case 0xA2: /* LDX #imm (Load X Register Immediate)
                X = immediate value (operand after opcode)
                Sets Z (zero) and N (negative) flags
                Addressing mode: immediate
            */
                alu_op.write(0xB);      // MOV (pass operand -> X)
                alu_enable.write(false); // disable ALU
                reg_sel.write(1);       // select register X
                reg_we.write(true);     // write to register
                set_flags.write(true);  // set Z and N flags
                mem_we.write(false);    // don't write to memory
                mem_oe.write(false);    // don't read from memory (operand already fetched)
                pc_inc.write(true);     // go to next instruction
                halt.write(false);
                break;
            case 0xA6: /* LDX zp (Load X Register from Zero Page)
                X = value from memory at zero page address
                Sets Z (zero) and N (negative) flags
                Addressing mode: zero page
            */
                alu_op.write(0xB);      // MOV (pass operand -> X)
                alu_enable.write(false); // disable ALU
                reg_sel.write(1);       // select register X
                reg_we.write(true);     // write to register
                set_flags.write(true);  // set Z and N flags
                mem_we.write(false);    // don't write to memory
                mem_oe.write(true);     // read from memory
                pc_inc.write(true);     // go to next instruction
                halt.write(false);
                break;
            case 0xB6: /* LDX zp,Y (Load X Register from Zero Page + Y)
                X = value from memory at address (zero page + Y)
                Sets Z (zero) and N (negative) flags
                Addressing mode: zero page,Y
            */
                alu_op.write(0xB);      // MOV (pass operand -> X)
                alu_enable.write(false); // disable ALU
                reg_sel.write(1);       // select register X
                reg_we.write(true);     // write to register
                set_flags.write(true);  // set Z and N flags
                mem_we.write(false);    // don't write to memory
                mem_oe.write(true);     // read from memory
                pc_inc.write(true);     // go to next instruction
                halt.write(false);
                break;
            case 0xAE: /* LDX abs (Load X Register from Absolute Address)
                X = value from memory at absolute address
                Sets Z (zero) and N (negative) flags
                Addressing mode: absolute
            */
                alu_op.write(0xB);      // MOV (pass operand -> X)
                alu_enable.write(false); // disable ALU
                reg_sel.write(1);       // select register X
                reg_we.write(true);     // write to register
                set_flags.write(true);  // set Z and N flags
                mem_we.write(false);    // don't write to memory
                mem_oe.write(true);     // read from memory
                pc_inc.write(true);     // go to next instruction
                halt.write(false);
                break;
            case 0xBE: /* LDX abs,Y (Load X Register from Absolute Address + Y)
                X = value from memory at address (abs + Y)
                Sets Z (zero) and N (negative) flags
                Addressing mode: absolute,Y
            */
                alu_op.write(0xB);      // MOV (pass operand -> X)
                alu_enable.write(false); // disable ALU
                reg_sel.write(1);       // select register X
                reg_we.write(true);     // write to register
                set_flags.write(true);  // set Z and N flags
                mem_we.write(false);    // don't write to memory
                mem_oe.write(true);     // read from memory
                pc_inc.write(true);     // go to next instruction
                halt.write(false);
                break;

            case 0xA0: /* LDY #imm (Load Y Register Immediate)
                Y = immediate value (operand after opcode)
                Sets Z (zero) and N (negative) flags
                Addressing mode: immediate
            */
                alu_op.write(0xB);      // MOV (pass operand -> Y)
                alu_enable.write(false); // disable ALU
                reg_sel.write(2);       // select register Y
                reg_we.write(true);     // write to register
                set_flags.write(true);  // set Z and N flags
                mem_we.write(false);    // don't write to memory
                mem_oe.write(false);    // don't read from memory (operand already fetched)
                pc_inc.write(true);     // go to next instruction
                halt.write(false);
                break;
            case 0xA4: /* LDY zp (Load Y Register from Zero Page)
                Y = value from memory at zero page address
                Sets Z (zero) and N (negative) flags
                Addressing mode: zero page
            */
                alu_op.write(0xB);      // MOV (pass operand -> Y)
                alu_enable.write(false); // disable ALU
                reg_sel.write(2);       // select register Y
                reg_we.write(true);     // write to register
                set_flags.write(true);  // set Z and N flags
                mem_we.write(false);    // don't write to memory
                mem_oe.write(true);     // read from memory
                pc_inc.write(true);     // go to next instruction
                halt.write(false);
                break;
            case 0xB4: /* LDY zp,X (Load Y Register from Zero Page + X)
                Y = value from memory at address (zero page + X)
                Sets Z (zero) and N (negative) flags
                Addressing mode: zero page,X
            */
                alu_op.write(0xB);      // MOV (pass operand -> Y)
                alu_enable.write(false); // disable ALU
                reg_sel.write(2);       // select register Y
                reg_we.write(true);     // write to register
                set_flags.write(true);  // set Z and N flags
                mem_we.write(false);    // don't write to memory
                mem_oe.write(true);     // read from memory
                pc_inc.write(true);     // go to next instruction
                halt.write(false);
                break;
            case 0xAC: /* LDY abs (Load Y Register from Absolute Address)
                Y = value from memory at absolute address
                Sets Z (zero) and N (negative) flags
                Addressing mode: absolute
            */
                alu_op.write(0xB);      // MOV (pass operand -> Y)
                alu_enable.write(false); // disable ALU
                reg_sel.write(2);       // select register Y
                reg_we.write(true);     // write to register
                set_flags.write(true);  // set Z and N flags
                mem_we.write(false);    // don't write to memory
                mem_oe.write(true);     // read from memory
                pc_inc.write(true);     // go to next instruction
                halt.write(false);
                break;
            case 0xBC: /* LDY abs,X (Load Y Register from Absolute Address + X)
                Y = value from memory at address (abs + X)
                Sets Z (zero) and N (negative) flags
                Addressing mode: absolute,X
            */
                alu_op.write(0xB);      // MOV (pass operand -> Y)
                alu_enable.write(false); // disable ALU
                reg_sel.write(2);       // select register Y
                reg_we.write(true);     // write to register
                set_flags.write(true);  // set Z and N flags
                mem_we.write(false);    // don't write to memory
                mem_oe.write(true);     // read from memory
                pc_inc.write(true);     // go to next instruction
                halt.write(false);
                break;

            case 0x85: /* STA zp (Store Accumulator in Zero Page)
                [zero page] = A
                Does not set Z/N flags
                Addressing mode: zero page
            */
                alu_enable.write(false); // ALU not needed
                reg_src.write(0);        // select register A as source
                reg_we.write(false);     // don't write to register
                mem_we.write(true);      // write to memory
                mem_oe.write(false);     // don't read from memory
                set_flags.write(false);  // don't set flags
                pc_inc.write(true);      // go to next instruction
                halt.write(false);
                break;
            case 0x95: /* STA zp,X (Store Accumulator in Zero Page + X)
                [zero page + X] = A
                Does not set Z/N flags
                Addressing mode: zero page,X
            */
                alu_enable.write(false); // ALU not needed
                reg_src.write(0);        // select register A as source
                reg_we.write(false);     // don't write to register
                mem_we.write(true);      // write to memory
                mem_oe.write(false);     // don't read from memory
                set_flags.write(false);  // don't set flags
                pc_inc.write(true);      // go to next instruction
                halt.write(false);
                break;
            case 0x8D: /* STA abs (Store Accumulator in Absolute Address)
                [abs] = A
                Does not set Z/N flags
                Addressing mode: absolute
                NOTE: mem_we should not be true during address fetch!
            */
                alu_enable.write(false); // ALU not needed
                reg_src.write(0);        // select register A as source
                reg_we.write(false);     // don't write to register
                mem_we.write(false);     // TEMPORARY: don't set mem_we
                mem_oe.write(false);     // don't read from memory
                set_flags.write(false);  // don't set flags
                pc_inc.write(true);      // go to next instruction
                halt.write(false);
                break;
            case 0x9D: /* STA abs,X (Store Accumulator in Absolute Address + X)
                [abs + X] = A
                Does not set Z/N flags
                Addressing mode: absolute,X
            */
                alu_enable.write(false); // ALU not needed
                reg_src.write(0);        // select register A as source
                reg_we.write(false);     // don't write to register
                mem_we.write(true);      // write to memory
                mem_oe.write(false);     // don't read from memory
                set_flags.write(false);  // don't set flags
                pc_inc.write(true);      // go to next instruction
                halt.write(false);
                break;
            case 0x99: /* STA abs,Y (Store Accumulator in Absolute Address + Y)
                [abs + Y] = A
                Does not set Z/N flags
                Addressing mode: absolute,Y
            */
                alu_enable.write(false); // ALU not needed
                reg_src.write(0);        // select register A as source
                reg_we.write(false);     // don't write to register
                mem_we.write(true);      // write to memory
                mem_oe.write(false);     // don't read from memory
                set_flags.write(false);  // don't set flags
                pc_inc.write(true);      // go to next instruction
                halt.write(false);
                break;
            case 0x81: /* STA (ind,X) (Store Accumulator in (Zero Page + X) indirect)
                [(zero page + X)] = A
                Does not set Z/N flags
                Addressing mode: (indirect,X)
            */
                alu_enable.write(false); // ALU not needed
                reg_src.write(0);        // select register A as source
                reg_we.write(false);     // don't write to register
                mem_we.write(true);      // write to memory
                mem_oe.write(false);     // don't read from memory
                set_flags.write(false);  // don't set flags
                pc_inc.write(true);      // go to next instruction
                halt.write(false);
                break;
            case 0x91: /* STA (ind),Y (Store Accumulator in (Zero Page) indirect + Y)
                [(zero page) + Y] = A
                Does not set Z/N flags
                Addressing mode: (indirect),Y
            */
                alu_enable.write(false); // ALU not needed
                reg_src.write(0);        // select register A as source
                reg_we.write(false);     // don't write to register
                mem_we.write(true);      // write to memory
                mem_oe.write(false);     // don't read from memory
                set_flags.write(false);  // don't set flags
                pc_inc.write(true);      // go to next instruction
                halt.write(false);
                break;

            case 0x86: /* STX zp (Store X Register in Zero Page)
                [zero page] = X
                Does not set Z/N flags
                Addressing mode: zero page
            */
                alu_enable.write(false); // ALU not needed
                reg_src.write(1);        // select register X as source
                reg_we.write(false);     // don't write to register
                mem_we.write(true);      // write to memory
                mem_oe.write(false);     // don't read from memory
                set_flags.write(false);  // don't set flags
                pc_inc.write(true);      // go to next instruction
                halt.write(false);
                break;
            case 0x96: /* STX zp,Y (Store X Register in Zero Page + Y)
                [zero page + Y] = X
                Does not set Z/N flags
                Addressing mode: zero page,Y
            */
                alu_enable.write(false); // ALU not needed
                reg_src.write(1);        // select register X as source
                reg_we.write(false);     // don't write to register
                mem_we.write(true);      // write to memory
                mem_oe.write(false);     // don't read from memory
                set_flags.write(false);  // don't set flags
                pc_inc.write(true);      // go to next instruction
                halt.write(false);
                break;
            case 0x8E: /* STX abs (Store X Register in Absolute Address)
                [abs] = X
                Does not set Z/N flags
                Addressing mode: absolute
            */
                alu_enable.write(false); // ALU not needed
                reg_src.write(1);        // select register X as source
                reg_we.write(false);     // don't write to register
                mem_we.write(true);      // write to memory
                mem_oe.write(false);     // don't read from memory
                set_flags.write(false);  // don't set flags
                pc_inc.write(true);      // go to next instruction
                halt.write(false);
                break;

            case 0x84: /* STY zp (Store Y Register in Zero Page)
                [zero page] = Y
                Does not set Z/N flags
                Addressing mode: zero page
            */
                alu_enable.write(false); // ALU not needed
                reg_src.write(2);        // select register Y as source
                reg_we.write(false);     // don't write to register
                mem_we.write(true);      // write to memory
                mem_oe.write(false);     // don't read from memory
                set_flags.write(false);  // don't set flags
                pc_inc.write(true);      // go to next instruction
                halt.write(false);
                break;
            case 0x94: /* STY zp,X (Store Y Register in Zero Page + X)
                [zero page + X] = Y
                Does not set Z/N flags
                Addressing mode: zero page,X
            */
                alu_enable.write(false); // ALU not needed
                reg_src.write(2);        // select register Y as source
                reg_we.write(false);     // don't write to register
                mem_we.write(true);      // write to memory
                mem_oe.write(false);     // don't read from memory
                set_flags.write(false);  // don't set flags
                pc_inc.write(true);      // go to next instruction
                halt.write(false);
                break;
            case 0x8C: /* STY abs (Store Y Register in Absolute Address)
                [abs] = Y
                Does not set Z/N flags
                Addressing mode: absolute
            */
                alu_enable.write(false); // ALU not needed
                reg_src.write(2);        // select register Y as source
                reg_we.write(false);     // don't write to register
                mem_we.write(true);      // write to memory
                mem_oe.write(false);     // don't read from memory
                set_flags.write(false);  // don't set flags
                pc_inc.write(true);      // go to next instruction
                halt.write(false);
                break;

            // --- Register Transfers ---
            case 0xAA: /* TAX (Transfer Accumulator to X)
                X = A
                Sets Z and N flags
            */
                alu_op.write(0xB);      // MOV (A -> X)
                alu_enable.write(true);
                reg_sel.write(1);       // X
                reg_src.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);  // set Z/N
                mem_we.write(false);
                mem_oe.write(false);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0xA8: /* TAY (Transfer Accumulator to Y)
                Y = A
                Sets Z and N flags
            */
                alu_op.write(0xB);      // MOV (A -> Y)
                alu_enable.write(true);
                reg_sel.write(2);       // Y
                reg_src.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);  // set Z/N
                mem_we.write(false);
                mem_oe.write(false);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0xBA: /* TSX (Transfer Stack Pointer to X)
                X = S
                Does not set flags (in 6502 sets Z/N, but can be added if ALU supports it)
            */
                alu_op.write(0xB);      // MOV (S -> X)
                alu_enable.write(true);
                reg_sel.write(1);       // X
                reg_src.write(3);       // S (Stack Pointer, assume reg_src=3)
                reg_we.write(true);
                set_flags.write(false); // (optionally true if you want Z/N)
                mem_we.write(false);
                mem_oe.write(false);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x8A: /* TXA (Transfer X to Accumulator)
                A = X
                Sets Z and N flags
            */
                alu_op.write(0xB);      // MOV (X -> A)
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_src.write(1);       // X
                reg_we.write(true);
                set_flags.write(true);  // set Z/N
                mem_we.write(false);
                mem_oe.write(false);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x9A: /* TXS (Transfer X to Stack Pointer)
                S = X
                Does not set flags
            */
                alu_op.write(0xB);      // MOV (X -> S)
                alu_enable.write(true);
                reg_sel.write(3);       // S (Stack Pointer, assume reg_sel=3)
                reg_src.write(1);       // X
                reg_we.write(true);
                set_flags.write(false);
                mem_we.write(false);
                mem_oe.write(false);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x98: /* TYA (Transfer Y to Accumulator)
                A = Y
                Sets Z and N flags
            */
                alu_op.write(0xB);      // MOV (Y -> A)
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_src.write(2);       // Y
                reg_we.write(true);
                set_flags.write(true);  // set Z/N
                mem_we.write(false);
                mem_oe.write(false);
                pc_inc.write(true);
                halt.write(false);
                break;

            // --- Stack Operations ---
            case 0x48: /* PHA (Push Accumulator on Stack)
                S = S - 1; [S] = A
                Does not set flags
            */
                alu_enable.write(false); // ALU not needed
                reg_src.write(0);        // A
                reg_we.write(false);
                mem_we.write(true);      // write to stack
                mem_oe.write(false);
                set_flags.write(false);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x08: /* PHP (Push Processor Status on Stack)
                S = S - 1; [S] = P
                Does not set flags
            */
                alu_enable.write(false);
                reg_src.write(4);        // P (status)
                reg_we.write(false);
                mem_we.write(true);
                mem_oe.write(false);
                set_flags.write(false);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x68: /* PLA (Pull Accumulator from Stack)
                A = [S]; S = S + 1
                Sets Z/N flags
            */
                alu_op.write(0xB);      // MOV ([S] -> A)
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);  // set Z/N
                mem_we.write(false);
                mem_oe.write(true);     // read from stack
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x28: /* PLP (Pull Processor Status from Stack)
                P = [S]; S = S + 1
                Does not set flags
            */
                alu_op.write(0xB);      // MOV ([S] -> P)
                alu_enable.write(true);
                reg_sel.write(4);       // P (status)
                reg_we.write(true);
                set_flags.write(false);
                mem_we.write(false);
                mem_oe.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;

            // --- Logical Operations ---
            case 0x29: /* AND #imm (A = A & #imm)
                Sets Z and N flags
                Addressing mode: immediate
            */
                alu_op.write(0x2);      // AND
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(false);    // operand already fetched
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x25: /* AND zp (A = A & [zp])
                Sets Z and N flags
                Addressing mode: zero page
            */
                alu_op.write(0x2);      // AND
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);     // read operand from memory
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x35: /* AND zp,X (A = A & [zp + X])
                Sets Z and N flags
                Addressing mode: zero page,X
            */
                alu_op.write(0x2);      // AND
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x2D: /* AND abs (A = A & [abs])
                Sets Z and N flags
                Addressing mode: absolute
            */
                alu_op.write(0x2);      // AND
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x3D: /* AND abs,X (A = A & [abs + X])
                Sets Z and N flags
                Addressing mode: absolute,X
            */
                alu_op.write(0x2);      // AND
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x39: /* AND abs,Y (A = A & [abs + Y])
                Sets Z and N flags
                Addressing mode: absolute,Y
            */
                alu_op.write(0x2);      // AND
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x21: /* AND (ind,X) (A = A & [[zp + X]])
                Sets Z and N flags
                Addressing mode: (indirect,X)
            */
                alu_op.write(0x2);      // AND
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x31: /* AND (ind),Y (A = A & [[zp] + Y])
                Sets Z and N flags
                Addressing mode: (indirect),Y
            */
                alu_op.write(0x2);      // AND
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            
            case 0x09: /* ORA #imm (A = A | #imm)
                Sets Z and N flags
                Addressing mode: immediate
            */
                alu_op.write(0x3);      // OR
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(false);    // operand already fetched
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x05: /* ORA zp (A = A | [zp])
                Sets Z and N flags
                Addressing mode: zero page
            */
                alu_op.write(0x3);      // OR
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);     // read operand from memory
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x15: /* ORA zp,X (A = A | [zp + X])
                Sets Z and N flags
                Addressing mode: zero page,X
            */
                alu_op.write(0x3);      // OR
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;            
            case 0x0D: /* ORA abs (A = A | [abs])
                Sets Z and N flags
                Addressing mode: absolute
            */
                alu_op.write(0x3);      // OR
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;            
            case 0x1D: /* ORA abs,X (A = A | [abs + X])
                Sets Z and N flags
                Addressing mode: absolute,X
            */
                alu_op.write(0x3);      // OR
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;            
            case 0x19: /* ORA abs,Y (A = A | [abs + Y])
                Sets Z and N flags
                Addressing mode: absolute,Y
            */
                alu_op.write(0x3);      // OR
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;            
            case 0x01: /* ORA (ind,X) (A = A | [[zp + X]])
                Sets Z and N flags
                Addressing mode: (indirect,X)
            */
                alu_op.write(0x3);      // OR
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;            
            case 0x11: /* ORA (ind),Y (A = A | [[zp] + Y])
                Sets Z and N flags
                Addressing mode: (indirect),Y
            */
                alu_op.write(0x3);      // OR
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;

            case 0x49: /* EOR #imm (A = A ^ #imm)
                Sets Z and N flags
                Addressing mode: immediate
            */
                alu_op.write(0x4);      // XOR
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(false);    // operand already fetched
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x45: /* EOR zp (A = A ^ [zp])
                Sets Z and N flags
                Addressing mode: zero page
            */
                alu_op.write(0x4);      // XOR
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);     // read operand from memory
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x55: /* EOR zp,X (A = A ^ [zp + X])
                Sets Z and N flags
                Addressing mode: zero page,X
            */
                alu_op.write(0x4);      // XOR
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x4D: /* EOR abs (A = A ^ [abs])
                Sets Z and N flags
                Addressing mode: absolute
            */
                alu_op.write(0x4);      // XOR
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x5D: /* EOR abs,X (A = A ^ [abs + X])
                Sets Z and N flags
                Addressing mode: absolute,X
            */
                alu_op.write(0x4);      // XOR
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x59: /* EOR abs,Y (A = A ^ [abs + Y])
                Sets Z and N flags
                Addressing mode: absolute,Y
            */
                alu_op.write(0x4);      // XOR
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x41: /* EOR (ind,X) (A = A ^ [[zp + X]])
                Sets Z and N flags
                Addressing mode: (indirect,X)
            */
                alu_op.write(0x4);      // XOR
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x51: /* EOR (ind),Y (A = A ^ [[zp] + Y])
                Sets Z and N flags
                Addressing mode: (indirect),Y
            */
                alu_op.write(0x4);      // XOR
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;

            // --- Arithmetic Operations ---
            case 0x69: /* ADC #imm (A = A + #imm + C)
                Sets C, Z, N, V flags
                Addressing mode: immediate
            */
                alu_op.write(0x0);      // ADC
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(false);    // operand already fetched
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x65: /* ADC zp (A = A + [zp] + C)
                Sets C, Z, N, V flags
                Addressing mode: zero page
            */
                alu_op.write(0x0);      // ADC
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);     // read operand from memory
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x75: /* ADC zp,X (A = A + [zp + X] + C)
                Sets C, Z, N, V flags
                Addressing mode: zero page,X
            */
                alu_op.write(0x0);      // ADC
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x6D: /* ADC abs (A = A + [abs] + C)
                Sets C, Z, N, V flags
                Addressing mode: absolute
            */
                alu_op.write(0x0);      // ADC
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x7D: /* ADC abs,X (A = A + [abs + X] + C)
                Sets C, Z, N, V flags
                Addressing mode: absolute,X
            */
                alu_op.write(0x0);      // ADC
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x79: /* ADC abs,Y (A = A + [abs + Y] + C)
                Sets C, Z, N, V flags
                Addressing mode: absolute,Y
            */
                alu_op.write(0x0);      // ADC
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x61: /* ADC (ind,X) (A = A + [[zp + X]] + C)
                Sets C, Z, N, V flags
                Addressing mode: (indirect,X)
            */
                alu_op.write(0x0);      // ADC
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x71: /* ADC (ind),Y (A = A + [[zp] + Y] + C)
                Sets C, Z, N, V flags
                Addressing mode: (indirect),Y
            */
                alu_op.write(0x0);      // ADC
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;

            case 0xE9: /* SBC #imm (A = A - #imm - (1-C))
                Sets C, Z, N, V flags
                Addressing mode: immediate
            */
                alu_op.write(0x1);      // SBC
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(false);    // operand already fetched
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0xE5: /* SBC zp (A = A - [zp] - (1-C))
                Sets C, Z, N, V flags
                Addressing mode: zero page
            */
                alu_op.write(0x1);      // SBC
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);     // read operand from memory
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0xF5: /* SBC zp,X (A = A - [zp + X] - (1-C))
                Sets C, Z, N, V flags
                Addressing mode: zero page,X
            */
                alu_op.write(0x1);      // SBC
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0xED: /* SBC abs (A = A - [abs] - (1-C))
                Sets C, Z, N, V flags
                Addressing mode: absolute
            */
                alu_op.write(0x1);      // SBC
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0xFD: /* SBC abs,X (A = A - [abs + X] - (1-C))
                Sets C, Z, N, V flags
                Addressing mode: absolute,X
            */
                alu_op.write(0x1);      // SBC
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0xF9: /* SBC abs,Y (A = A - [abs + Y] - (1-C))
                Sets C, Z, N, V flags
                Addressing mode: absolute,Y
            */
                alu_op.write(0x1);      // SBC
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0xE1: /* SBC (ind,X) (A = A - [[zp + X]] - (1-C))
                Sets C, Z, N, V flags
                Addressing mode: (indirect,X)
            */
                alu_op.write(0x1);      // SBC
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0xF1: /* SBC (ind),Y (A = A - [[zp] + Y] - (1-C))
                Sets C, Z, N, V flags
                Addressing mode: (indirect),Y
            */
                alu_op.write(0x1);      // SBC
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;

            case 0xC6: /* DEC zp (Decrement memory at zero page)
                Decrement value in memory at zero page address by 1
                Sets Z and N flags
                Addressing mode: zero page
            */
                alu_op.write(0x6);      // DEC (ALU: a - 1)
                alu_enable.write(true);
                reg_we.write(false);    // do not write to register
                mem_we.write(true);     // write to memory
                mem_oe.write(true);     // read from memory
                set_flags.write(true);  // set Z and N flags
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0xD6: /* DEC zp,X (Decrement memory at zero page + X)
                Decrement value in memory at (zero page + X) address by 1
                Sets Z and N flags
                Addressing mode: zero page,X
            */
                alu_op.write(0x6);      // DEC
                alu_enable.write(true);
                reg_we.write(false);
                mem_we.write(true);
                mem_oe.write(true);
                set_flags.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0xCE: /* DEC abs (Decrement memory at absolute address)
                Decrement value in memory at absolute address by 1
                Sets Z and N flags
                Addressing mode: absolute
            */
                alu_op.write(0x6);      // DEC
                alu_enable.write(true);
                reg_we.write(false);
                mem_we.write(true);
                mem_oe.write(true);
                set_flags.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0xDE: /* DEC abs,X (Decrement memory at absolute address + X)
                Decrement value in memory at (abs + X) address by 1
                Sets Z and N flags
                Addressing mode: absolute,X
            */
                alu_op.write(0x6);      // DEC
                alu_enable.write(true);
                reg_we.write(false);
                mem_we.write(true);
                mem_oe.write(true);
                set_flags.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;

            case 0xE6: /* INC zp (Increment memory at zero page)
                Increment value in memory at zero page address by 1
                Sets Z (zero) and N (negative) flags
                Addressing mode: zero page
            */
                alu_op.write(0x5);      // INC (ALU: a + 1)
                alu_enable.write(true); // enable ALU
                reg_we.write(false);    // do not write to register
                mem_we.write(true);     // write to memory
                mem_oe.write(true);     // read from memory
                set_flags.write(true);  // set Z and N flags
                pc_inc.write(true);     // proceed to next instruction
                halt.write(false);
                break;
            case 0xF6: /* INC zp,X (Increment memory at zero page + X)
                Increment value in memory at (zero page + X) address by 1
                Sets Z and N flags
                Addressing mode: zero page,X
            */
                alu_op.write(0x5);      // INC (ALU: a + 1)
                alu_enable.write(true);
                reg_we.write(false);
                mem_we.write(true);
                mem_oe.write(true);
                set_flags.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0xEE: /* INC abs (Increment memory at absolute address)
                Increment value in memory at absolute address by 1
                Sets Z and N flags
                Addressing mode: absolute
            */
                alu_op.write(0x5);      // INC
                alu_enable.write(true);
                reg_we.write(false);
                mem_we.write(true);
                mem_oe.write(true);
                set_flags.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0xFE: /* INC abs,X (Increment memory at absolute address + X)
                Increment value in memory at (abs + X) address by 1
                Sets Z and N flags
                Addressing mode: absolute,X
            */
                alu_op.write(0x5);      // INC
                alu_enable.write(true);
                reg_we.write(false);
                mem_we.write(true);
                mem_oe.write(true);
                set_flags.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;

            // --- Shift/Rotate Operations ---
            case 0x0A: /* ASL A (Arithmetic Shift Left Accumulator)
                A = A << 1
                Sets C, Z, N flags
                Addressing mode: accumulator
            */
                alu_op.write(0x7);      // ASL
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(false);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x06: /* ASL zp (Arithmetic Shift Left Zero Page)
                [zp] = [zp] << 1
                Sets C, Z, N flags
                Addressing mode: zero page
            */
                alu_op.write(0x7);      // ASL
                alu_enable.write(true);
                reg_we.write(false);
                mem_we.write(true);
                mem_oe.write(true);
                set_flags.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x16: /* ASL zp,X (Arithmetic Shift Left Zero Page,X)
                [zp + X] = [zp + X] << 1
                Sets C, Z, N flags
                Addressing mode: zero page,X
            */
                alu_op.write(0x7);      // ASL
                alu_enable.write(true);
                reg_we.write(false);
                mem_we.write(true);
                mem_oe.write(true);
                set_flags.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x0E: /* ASL abs (Arithmetic Shift Left Absolute)
                [abs] = [abs] << 1
                Sets C, Z, N flags
                Addressing mode: absolute
            */
                alu_op.write(0x7);      // ASL
                alu_enable.write(true);
                reg_we.write(false);
                mem_we.write(true);
                mem_oe.write(true);
                set_flags.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x1E: /* ASL abs,X (Arithmetic Shift Left Absolute,X)
                [abs + X] = [abs + X] << 1
                Sets C, Z, N flags
                Addressing mode: absolute,X
            */
                alu_op.write(0x7);      // ASL
                alu_enable.write(true);
                reg_we.write(false);
                mem_we.write(true);
                mem_oe.write(true);
                set_flags.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;

            case 0x4A: /* LSR A (Logical Shift Right Accumulator)
                A = A >> 1
                Sets C, Z, N flags
                Addressing mode: accumulator
            */
                alu_op.write(0x8);      // LSR
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(false);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x46: /* LSR zp (Logical Shift Right Zero Page)
                [zp] = [zp] >> 1
                Sets C, Z, N flags
                Addressing mode: zero page
            */
                alu_op.write(0x8);      // LSR
                alu_enable.write(true);
                reg_we.write(false);
                mem_we.write(true);
                mem_oe.write(true);
                set_flags.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x56: /* LSR zp,X (Logical Shift Right Zero Page,X)
                [zp + X] = [zp + X] >> 1
                Sets C, Z, N flags
                Addressing mode: zero page,X
            */
                alu_op.write(0x8);      // LSR
                alu_enable.write(true);
                reg_we.write(false);
                mem_we.write(true);
                mem_oe.write(true);
                set_flags.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x4E: /* LSR abs (Logical Shift Right Absolute)
                [abs] = [abs] >> 1
                Sets C, Z, N flags
                Addressing mode: absolute
            */
                alu_op.write(0x8);      // LSR
                alu_enable.write(true);
                reg_we.write(false);
                mem_we.write(true);
                mem_oe.write(true);
                set_flags.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x5E: /* LSR abs,X (Logical Shift Right Absolute,X)
                [abs + X] = [abs + X] >> 1
                Sets C, Z, N flags
                Addressing mode: absolute,X
            */
                alu_op.write(0x8);      // LSR
                alu_enable.write(true);
                reg_we.write(false);
                mem_we.write(true);
                mem_oe.write(true);
                set_flags.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;

            case 0x2A: /* ROL A (Rotate Left Accumulator)
                A = (A << 1) | C
                Sets C, Z, N flags
                Addressing mode: accumulator
            */
                alu_op.write(0x9);      // ROL
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(false);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x26: /* ROL zp (Rotate Left Zero Page)
                [zp] = ([zp] << 1) | C
                Sets C, Z, N flags
                Addressing mode: zero page
            */
                alu_op.write(0x9);      // ROL
                alu_enable.write(true);
                reg_we.write(false);
                mem_we.write(true);
                mem_oe.write(true);
                set_flags.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x36: /* ROL zp,X (Rotate Left Zero Page,X)
                [zp + X] = ([zp + X] << 1) | C
                Sets C, Z, N flags
                Addressing mode: zero page,X
            */
                alu_op.write(0x9);      // ROL
                alu_enable.write(true);
                reg_we.write(false);
                mem_we.write(true);
                mem_oe.write(true);
                set_flags.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x2E: /* ROL abs (Rotate Left Absolute)
                [abs] = ([abs] << 1) | C
                Sets C, Z, N flags
                Addressing mode: absolute
            */
                alu_op.write(0x9);      // ROL
                alu_enable.write(true);
                reg_we.write(false);
                mem_we.write(true);
                mem_oe.write(true);
                set_flags.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x3E: /* ROL abs,X (Rotate Left Absolute,X)
                [abs + X] = ([abs + X] << 1) | C
                Sets C, Z, N flags
                Addressing mode: absolute,X
            */
                alu_op.write(0x9);      // ROL
                alu_enable.write(true);
                reg_we.write(false);
                mem_we.write(true);
                mem_oe.write(true);
                set_flags.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;

            case 0x6A: /* ROR A (Rotate Right Accumulator)
                A = (A >> 1) | (C << 7)
                Sets C, Z, N flags
                Addressing mode: accumulator
            */
                alu_op.write(0xA);      // ROR
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(false);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x66: /* ROR zp (Rotate Right Zero Page)
                [zp] = ([zp] >> 1) | (C << 7)
                Sets C, Z, N flags
                Addressing mode: zero page
            */
                alu_op.write(0xA);      // ROR
                alu_enable.write(true);
                reg_we.write(false);
                mem_we.write(true);
                mem_oe.write(true);
                set_flags.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x76: /* ROR zp,X (Rotate Right Zero Page,X)
                [zp + X] = ([zp + X] >> 1) | (C << 7)
                Sets C, Z, N flags
                Addressing mode: zero page,X
            */
                alu_op.write(0xA);      // ROR
                alu_enable.write(true);
                reg_we.write(false);
                mem_we.write(true);
                mem_oe.write(true);
                set_flags.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x6E: /* ROR abs (Rotate Right Absolute)
                [abs] = ([abs] >> 1) | (C << 7)
                Sets C, Z, N flags
                Addressing mode: absolute
            */
                alu_op.write(0xA);      // ROR
                alu_enable.write(true);
                reg_we.write(false);
                mem_we.write(true);
                mem_oe.write(true);
                set_flags.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x7E: /* ROR abs,X (Rotate Right Absolute,X)
                [abs + X] = ([abs + X] >> 1) | (C << 7)
                Sets C, Z, N flags
                Addressing mode: absolute,X
            */
                alu_op.write(0xA);      // ROR
                alu_enable.write(true);
                reg_we.write(false);
                mem_we.write(true);
                mem_oe.write(true);
                set_flags.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;

            // --- Jump/Call Operations ---
            case 0x4C: /* JMP abs (Jump to Absolute Address)
                PC = abs
                Addressing mode: absolute
            */
                pc_load.write(true);    // load new address to PC
                // pc_new should be set by fetch/decode logic
                pc_inc.write(false);    // do not increment PC
                halt.write(false);
                break;
            case 0x6C: /* JMP (ind) (Jump to Indirect Address)
                PC = [ind]
                Addressing mode: indirect
            */
                pc_load.write(true);    // load new address to PC
                // pc_new should be set by fetch/decode logic
                pc_inc.write(false);    // do not increment PC
                halt.write(false);
                break;
            case 0x20: /* JSR abs (Jump to Subroutine)
                Push (PC-1) to stack, PC = abs
                Addressing mode: absolute
            */
                // Push (PC-1) to stack should be handled by control/stack logic
                pc_load.write(true);    // load new address to PC
                pc_inc.write(false);    // do not increment PC
                halt.write(false);
                break;

            // --- Branch Operations ---
            case 0x10: /* BPL (Branch if Positive)
                if (N==0) PC = PC + offset
                Addressing mode: relative
            */
                // branch_condition = (N==0) should be evaluated by flag logic
                // if (branch_condition) pc_load.write(true); else pc_inc.write(true);
                halt.write(false);
                break;
            case 0x30: /* BMI (Branch if Minus)
                if (N==1) PC = PC + offset
                Addressing mode: relative
            */
                // branch_condition = (N==1)
                // if (branch_condition) pc_load.write(true); else pc_inc.write(true);
                halt.write(false);
                break;
            case 0x50: /* BVC (Branch if Overflow Clear)
                if (V==0) PC = PC + offset
                Addressing mode: relative
            */
                // branch_condition = (V==0)
                // if (branch_condition) pc_load.write(true); else pc_inc.write(true);
                halt.write(false);
                break;
            case 0x70: /* BVS (Branch if Overflow Set)
                if (V==1) PC = PC + offset
                Addressing mode: relative
            */
                // branch_condition = (V==1)
                // if (branch_condition) pc_load.write(true); else pc_inc.write(true);
                halt.write(false);
                break;
            case 0x90: /* BCC (Branch if Carry Clear)
                if (C==0) PC = PC + offset
                Addressing mode: relative
            */
                // branch_condition = (C==0)
                // if (branch_condition) pc_load.write(true); else pc_inc.write(true);
                halt.write(false);
                break;
            case 0xB0: /* BCS (Branch if Carry Set)
                if (C==1) PC = PC + offset
                Addressing mode: relative
            */
                // branch_condition = (C==1)
                // if (branch_condition) pc_load.write(true); else pc_inc.write(true);
                halt.write(false);
                break;
            case 0xD0: /* BNE (Branch if Not Equal)
                if (Z==0) PC = PC + offset
                Addressing mode: relative
            */
                // branch_condition = (Z==0)
                // if (branch_condition) pc_load.write(true); else pc_inc.write(true);
                halt.write(false);
                break;
            case 0xF0: /* BEQ (Branch if Equal)
                if (Z==1) PC = PC + offset
                Addressing mode: relative
            */
                // branch_condition = (Z==1)
                // if (branch_condition) pc_load.write(true); else pc_inc.write(true);
                halt.write(false);
                break;

            // --- Status Flag Changes ---
            case 0x18: /* CLC (Clear Carry Flag)
                P.C = 0
            */
                clear_carry.write(true);    // clear Carry flag
                reg_we.write(false);        // do not write to register
                alu_enable.write(false);    // do not use ALU
                set_flags.write(false);     // do not set Z,N flags from ALU
                mem_we.write(false);        // do not write to memory
                mem_oe.write(false);        // do not read from memory
                pc_inc.write(true);         // proceed to next instruction
                halt.write(false);
                break;
            case 0x38: /* SEC (Set Carry Flag)
                P.C = 1
            */
                set_carry.write(true);      // set Carry flag
                reg_we.write(false);        // do not write to register
                alu_enable.write(false);    // do not use ALU
                set_flags.write(false);     // do not set Z,N flags from ALU
                mem_we.write(false);        // do not write to memory
                mem_oe.write(false);        // do not read from memory
                pc_inc.write(true);         // proceed to next instruction
                halt.write(false);
                break;
            case 0x58: /* CLI (Clear Interrupt Disable)
                P.I = 0
            */
                // Set bit I=0 in status register (P)
                // reg_sel.write(4); // P
                // reg_we.write(true);
                // ...logic for setting bit I=0...
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x78: /* SEI (Set Interrupt Disable)
                P.I = 1
            */
                // Set bit I=1 in status register (P)
                // reg_sel.write(4); // P
                // reg_we.write(true);
                // ...logic for setting bit I=1...
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0xB8: /* CLV (Clear Overflow Flag)
                P.V = 0
            */
                // Set bit V=0 in status register (P)
                // reg_sel.write(4); // P
                // reg_we.write(true);
                // ...logic for setting bit V=0...
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0xD8: /* CLD (Clear Decimal Mode)
                P.D = 0
            */
                // Set bit D=0 in status register (P)
                // reg_sel.write(4); // P
                // reg_we.write(true);
                // ...logic for setting bit D=0...
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0xF8: /* SED (Set Decimal Mode)
                P.D = 1
            */
                // Set bit D=1 in status register (P)
                // reg_sel.write(4); // P
                // reg_we.write(true);
                // ...logic for setting bit D=1...
                pc_inc.write(true);
                halt.write(false);
                break;

            // --- System Functions ---
            case 0x00: /* BRK (Force Interrupt)
                Execute software interrupt, save PC+2 and P to stack, set PC to IRQ vector
            */
                irq_ack.write(true);    // acknowledge IRQ interrupt
                halt.write(false);
                // pc_load.write(true); // load PC from IRQ vector
                // ...logic for saving PC+2 and P to stack...
                break;
            case 0x40: /* RTI (Return from Interrupt)
                Restore P and PC from stack
            */
                nmi_ack.write(true);    // acknowledge return from NMI interrupt
                pc_load.write(true);    // load PC from stack
                halt.write(false);
                // ...logic for reading P and PC from stack...
                break;
            case 0x60: /* RTS (Return from Subroutine)
                PC = (pop from stack) + 1
            */
                pc_load.write(true);    // load PC from stack
                halt.write(false);
                // ...logic for reading PC from stack and incrementing...
                break;

            // --- Comparison Operations ---            
            case 0xC9: /* CMP #imm (Compare A with #imm)
                Sets C, Z, N flags
                Addressing mode: immediate
            */
                alu_op.write(0xC);      // CMP (SUB, tylko flagi)
                alu_enable.write(true);
                reg_we.write(false);
                mem_we.write(false);
                mem_oe.write(false);
                set_flags.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;            
            case 0xC5: /* CMP zp (Compare A with [zp])
                Sets C, Z, N flags
                Addressing mode: zero page
            */
                alu_op.write(0xC);
                alu_enable.write(true);
                reg_we.write(false);
                mem_we.write(false);
                mem_oe.write(true);
                set_flags.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;            
            case 0xD5: /* CMP zp,X (Compare A with [zp + X])
                Sets C, Z, N flags
                Addressing mode: zero page,X
            */
                alu_op.write(0xC);
                alu_enable.write(true);
                reg_we.write(false);
                mem_we.write(false);
                mem_oe.write(true);
                set_flags.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;            
            case 0xCD: /* CMP abs (Compare A with [abs])
                Sets C, Z, N flags
                Addressing mode: absolute
            */
                alu_op.write(0xC);
                alu_enable.write(true);
                reg_we.write(false);
                mem_we.write(false);
                mem_oe.write(true);
                set_flags.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;            
            case 0xDD: /* CMP abs,X (Compare A with [abs + X])
                Sets C, Z, N flags
                Addressing mode: absolute,X
            */
                alu_op.write(0xC);
                alu_enable.write(true);
                reg_we.write(false);
                mem_we.write(false);
                mem_oe.write(true);
                set_flags.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;            
            case 0xD9: /* CMP abs,Y (Compare A with [abs + Y])
                Sets C, Z, N flags
                Addressing mode: absolute,Y
            */
                alu_op.write(0xC);
                alu_enable.write(true);
                reg_we.write(false);
                mem_we.write(false);
                mem_oe.write(true);
                set_flags.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;            
            case 0xC1: /* CMP (ind,X) (Compare A with [[zp + X]])
                Sets C, Z, N flags
                Addressing mode: (indirect,X)
            */
                alu_op.write(0xC);
                alu_enable.write(true);
                reg_we.write(false);
                mem_we.write(false);
                mem_oe.write(true);
                set_flags.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;            
            case 0xD1: /* CMP (ind),Y (Compare A with [[zp] + Y])
                Sets C, Z, N flags
                Addressing mode: (indirect),Y
            */
                alu_op.write(0xC);
                alu_enable.write(true);
                reg_we.write(false);
                mem_we.write(false);
                mem_oe.write(true);
                set_flags.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;
            
            case 0xE0: /* CPX #imm (Compare X with #imm)
                Sets C, Z, N flags
                Addressing mode: immediate
            */
                alu_op.write(0xD);      // CPX (SUB, tylko flagi)
                alu_enable.write(true);
                reg_we.write(false);
                mem_we.write(false);
                mem_oe.write(false);
                set_flags.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;            
            case 0xE4: /* CPX zp (Compare X with [zp])
                Sets C, Z, N flags
                Addressing mode: zero page
            */
                alu_op.write(0xD);
                alu_enable.write(true);
                reg_we.write(false);
                mem_we.write(false);
                mem_oe.write(true);
                set_flags.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;            
            case 0xEC: /* CPX abs (Compare X with [abs])
                Sets C, Z, N flags
                Addressing mode: absolute
            */
                alu_op.write(0xD);
                alu_enable.write(true);
                reg_we.write(false);
                mem_we.write(false);
                mem_oe.write(true);
                set_flags.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;            
            case 0xC0: /* CPY #imm (Compare Y with #imm)
                Sets C, Z, N flags
                Addressing mode: immediate
            */
                alu_op.write(0xE);      // CPY (SUB, tylko flagi)
                alu_enable.write(true);
                reg_we.write(false);
                mem_we.write(false);
                mem_oe.write(false);
                set_flags.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;            
            case 0xC4: /* CPY zp (Compare Y with [zp])
                Sets C, Z, N flags
                Addressing mode: zero page
            */
                alu_op.write(0xE);
                alu_enable.write(true);
                reg_we.write(false);
                mem_we.write(false);
                mem_oe.write(true);
                set_flags.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;        
            case 0xCC: /* CPY abs (Compare Y with [abs])
                Sets C, Z, N flags
                Addressing mode: absolute
            */
                alu_op.write(0xE);
                alu_enable.write(true);
                reg_we.write(false);
                mem_we.write(false);
                mem_oe.write(true);
                set_flags.write(true);
                pc_inc.write(true);
                halt.write(false);
                break;

            // --- Default (NOP or illegal) ---
            default: /* NOP/illegal */ break;
        }
    }

    SC_CTOR(control_unit) {
        SC_METHOD(process);
        sensitive << clk.pos() << opcode;
    }
};
