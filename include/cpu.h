#pragma once
#include <systemc.h>
#include "alu.h"
#include "regfile.h"
#include "memory.h"
#include "control_unit.h"

// Main CPU Module
SC_MODULE(cpu) {
    sc_in<bool> clk;
    sc_in<bool> reset;

    // Instances of submodules
    alu* alu_i;
    regfile* regfile_i;
    memory* memory_i;
    control_unit* control_unit_i;

    // Connecting signals
    sc_signal<sc_uint<8>> opcode;
    sc_signal<sc_uint<4>> alu_op;
    sc_signal<bool> alu_enable, set_flags;
    sc_signal<bool> reg_we;
    sc_signal<sc_uint<3>> reg_sel, reg_src;
    sc_signal<bool> mem_we, mem_oe;
    sc_signal<bool> control_unit_mem_we;  // Unused output from control_unit
    sc_signal<bool> pc_inc, pc_load;
    sc_signal<sc_uint<16>> pc_new;
    sc_signal<bool> halt, irq_ack, nmi_ack;
    
    // P flag control signals
    sc_signal<bool> set_carry, clear_carry;
    sc_signal<bool> set_interrupt, clear_interrupt;
    sc_signal<bool> set_decimal, clear_decimal;
    sc_signal<bool> clear_overflow;

    // Additional CPU signals
    sc_signal<sc_uint<8>> alu_a, alu_b, alu_result;
    sc_signal<sc_uint<8>> alu_carry_in;
    sc_signal<bool> alu_carry, alu_zero, alu_negative, alu_overflow;

    sc_signal<sc_uint<8>> reg_w_data, reg_r_data;
    sc_signal<sc_uint<3>> reg_w_addr, reg_r_addr;

    sc_signal<bool> mem_clk;
    sc_signal<sc_uint<16>> mem_addr;
    sc_signal<sc_uint<8>> mem_w_data, mem_r_data;

    // Example signals for PC, IR (for fetch/execute)
    sc_signal<sc_uint<16>> pc;
    sc_signal<sc_uint<8>> ir;

    cpu(sc_module_name name);


    // --- fetch/execute fields ---
    enum cpu_state_t { FETCH, WAIT_INSTRUCTION, DECODE, WAIT_OPERAND, FETCH_ADDR_LOW, PROCESS_ADDR_LOW, FETCH_ADDR_HIGH, PROCESS_ADDR_HIGH,FETCH_INDIRECT_HIGH,PROCESS_INDIRECT_HIGH, EXECUTE, WAIT_ALU };
    cpu_state_t state = FETCH;
    
    // Addressing modes
    enum addressing_mode_t { 
        IMPLIED,     // TAX, PHA, etc.
        IMMEDIATE,   // LDA #$42
        ZERO_PAGE,   // LDA $42
        ZERO_PAGE_X, // LDA $42,X
        ZERO_PAGE_Y, // LDX $42,Y
        ABSOLUTE,    // LDA $1234
        ABSOLUTE_X,  // LDA $1234,X
        ABSOLUTE_Y,  // LDA $1234,Y
        INDIRECT_X,  // LDA ($42,X)
        INDIRECT_Y   // LDA ($42),Y
    };
    
    sc_uint<16> pc_val = 0x0000;
    sc_uint<8> ir_val = 0x00;
    sc_uint<8> operand = 0x00;
    sc_uint<16> effective_addr = 0x0000; // Effective address for complex addressing
    sc_uint<8> reg_a_val = 0x00; // Track value of register A

    void fetch_execute();
    
    // helper functions
    addressing_mode_t get_addressing_mode(sc_uint<8> opcode);
    int get_instruction_length(sc_uint<8> opcode);
    bool needs_operand(sc_uint<8> opcode);
    bool is_store_instruction(sc_uint<8> opcode);

    ~cpu() {
        delete alu_i;
        delete regfile_i;
        delete memory_i;
        delete control_unit_i;
    }
};
