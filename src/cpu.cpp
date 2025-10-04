#include "cpu.h"

// Helper functions for addressing modes and instruction lengths
cpu::addressing_mode_t cpu::get_addressing_mode(sc_uint<8> opcode) {
    switch (opcode) {
        // Immediate
        case 0xA9: case 0xA2: case 0xA0: case 0x29: case 0x09: case 0x49: case 0x69: case 0xE9: case 0xC9: case 0xE0: case 0xC0:
            return IMMEDIATE;
        
        // Zero Page
        case 0xA5: case 0xA6: case 0xA4: case 0x85: case 0x86: case 0x84: case 0x25: case 0x05: case 0x45: case 0x65: case 0xE5:
        case 0xC5: case 0xE4: case 0xC4: case 0x06: case 0x46: case 0x26: case 0x66: case 0xE6: case 0xC6:
            return ZERO_PAGE;
            
        // Zero Page,X
        case 0xB5: case 0x95: case 0x35: case 0x15: case 0x55: case 0x75: case 0xF5: case 0xD5: case 0x16: case 0x56:
        case 0x36: case 0x76: case 0xF6: case 0xD6: case 0x94:
            return ZERO_PAGE_X;
            
        // Zero Page,Y
        case 0xB6: case 0x96: case 0xB4:
            return ZERO_PAGE_Y;
            
        // Absolute
        case 0xAD: case 0xAE: case 0xAC: case 0x8D: case 0x8E: case 0x8C: case 0x2D: case 0x0D: case 0x4D: case 0x6D:
        case 0xED: case 0xCD: case 0xEC: case 0xCC: case 0x0E: case 0x4E: case 0x2E: case 0x6E: case 0xEE: case 0xCE:
        case 0x20: case 0x4C: case 0x6C:
            return ABSOLUTE;
            
        // Absolute,X
        case 0xBD: case 0x9D: case 0x3D: case 0x1D: case 0x5D: case 0x7D: case 0xFD: case 0xDD: case 0x1E: case 0x5E:
        case 0x3E: case 0x7E: case 0xFE: case 0xDE: case 0xBC:
            return ABSOLUTE_X;
            
        // Absolute,Y
        case 0xB9: case 0x99: case 0x39: case 0x19: case 0x59: case 0x79: case 0xF9: case 0xD9: case 0xBE:
            return ABSOLUTE_Y;
            
        // (Zero Page,X)
        case 0xA1: case 0x81: case 0x21: case 0x01: case 0x41: case 0x61: case 0xE1: case 0xC1:
            return INDIRECT_X;
            
        // (Zero Page),Y
        case 0xB1: case 0x91: case 0x31: case 0x11: case 0x51: case 0x71: case 0xF1: case 0xD1:
            return INDIRECT_Y;

        // Implied (all others)
        default:
            return IMPLIED;
    }
}

int cpu::get_instruction_length(sc_uint<8> opcode) {
    addressing_mode_t mode = get_addressing_mode(opcode);
    switch (mode) {
        case IMPLIED: return 1;
        case IMMEDIATE: case ZERO_PAGE: case ZERO_PAGE_X: case ZERO_PAGE_Y: 
        case INDIRECT_X: case INDIRECT_Y: return 2;
        case ABSOLUTE: case ABSOLUTE_X: case ABSOLUTE_Y: return 3;
        default: return 1;
    }
}

bool cpu::needs_operand(sc_uint<8> opcode) {
    return get_addressing_mode(opcode) != IMPLIED;
}

bool cpu::is_store_instruction(sc_uint<8> opcode) {
    // Instructions STA, STX, STY do not read operand from memory,
    // they only write to memory at the calculated address
    switch (opcode) {
        // STA variants
        case 0x85: case 0x95: case 0x8D: case 0x9D: case 0x99: case 0x81: case 0x91:
        // STX variants  
        case 0x86: case 0x96: case 0x8E:
        // STY variants
        case 0x84: case 0x94: case 0x8C:
            return true;
        default:
            return false;
    }
}


// Automat fetch/execute
void cpu::fetch_execute() {
	// CPU controlls mem_we signal directly
	mem_we.write(false);  // Default to no write
	
	if (reset.read()) {
		state = FETCH;
		pc_val = 0x0000;
		ir_val = 0x00;
		operand = 0x00;
		effective_addr = 0x0000;
		pc.write(pc_val);
		ir.write(ir_val);
		return;
	}

	switch (state) {
		case FETCH:
			// Set instruction address and wait for cycle
			mem_addr.write(pc_val);
			state = WAIT_INSTRUCTION;
			break;
			
		case WAIT_INSTRUCTION:
			// Wait for one cycle to read instruction from memory
			state = DECODE;
			break;
			
		case DECODE: {
			// Fetch instruction from memory
			ir_val = mem_r_data.read();
			ir.write(ir_val);
			opcode.write(ir_val);
			std::cout << "DECODE: Fetch instruction 0x" << std::hex << (int)ir_val << " from address 0x" << (int)pc_val << std::endl;

			// Check BRK instruction (halt)
			if (ir_val == 0x00) {
				std::cout << "CPU: BRK - simulation stopped" << std::endl;
				//sc_stop(); //comment for running cpu_tb tests
				return;
			}
			
			addressing_mode_t mode = get_addressing_mode(ir_val);
			
			switch (mode) {
				case IMPLIED:
					// Instructions without operands (TAX, PHA, etc.)
					state = EXECUTE;
					break;
					
				case IMMEDIATE:
					// Operand immediately after instruction
					effective_addr = pc_val + 1;
					mem_addr.write(effective_addr);
					state = WAIT_OPERAND;
					break;
					
				case ZERO_PAGE:
				case ZERO_PAGE_X:
				case ZERO_PAGE_Y:
				case INDIRECT_X:
				case INDIRECT_Y:
					// 1 byte address
					mem_addr.write(pc_val + 1);
					state = FETCH_ADDR_LOW;
					break;
					
				case ABSOLUTE:
				case ABSOLUTE_X:
				case ABSOLUTE_Y:
					// 2 bytes address (LSB first)
					//std::cout << "DECODE: ABSOLUTE addressing, przejscie do FETCH_ADDR_LOW" << std::endl;
					mem_addr.write(pc_val + 1);
					state = FETCH_ADDR_LOW;
					break;
					
				default:
					state = EXECUTE;
					break;
			}
			break;
		}
		
		case WAIT_OPERAND:
			// Wait for one cycle to read immediate operand
			state = EXECUTE;
			break;
			
		case FETCH_ADDR_LOW: {
			// Wait for one cycle to read data from memory
			state = PROCESS_ADDR_LOW;
			break;
		}
			
		case PROCESS_ADDR_LOW: {
			// Fetch first byte of address (LSB for absolute, only byte for zero page)
			sc_uint<8> addr_low = mem_r_data.read();
			addressing_mode_t mode = get_addressing_mode(ir_val);
			//std::cout << "PROCESS_ADDR_LOW: Fetched addr_low=0x" << std::hex << (int)addr_low << " mode=" << mode << std::endl;

			if (mode == ABSOLUTE || mode == ABSOLUTE_X || mode == ABSOLUTE_Y) {
				// For absolute we need MSB
				effective_addr = addr_low; // Temporarily store LSB
				mem_addr.write(pc_val + 2);
				state = FETCH_ADDR_HIGH;
			} else {
				// Zero page or indirect - we already have full address
				effective_addr = addr_low;
				
				if (mode == ZERO_PAGE_X) {
					effective_addr = (effective_addr + regfile_i->X) & 0xFF; // X register, wrap w zero page
				} else if (mode == ZERO_PAGE_Y) {
					// Fetch Y register (will need to add logic for reg_src)
					effective_addr = (effective_addr + regfile_i->Y) & 0xFF; // TODO: add Y register
				}
				if(mode == INDIRECT_X){
					// add X to zero page address first
					effective_addr = (addr_low + regfile_i->X) & 0xFF; // X register, wrap w zero page
					// read the 16 bit pointer from this address
					mem_addr.write(effective_addr); // Read LSB of effective address
					state = FETCH_ADDR_HIGH; // Will fetch low byte of pointer next
				}
				else if(mode == INDIRECT_Y){
					// For INDIRECT_Y first read pointer then add Y
					effective_addr = addr_low; // use zero page address
					mem_addr.write(effective_addr); // Read LSB of effective address
					state = FETCH_ADDR_HIGH; // Will fetch pointer next
				} else {
					// Direct access to operand
					mem_addr.write(effective_addr);
					state = WAIT_OPERAND;
				}
			}
			break;
		}
		
		case FETCH_ADDR_HIGH: {
			// Wait for one cycle to read data from memory
			state = PROCESS_ADDR_HIGH;
			break;
		}
			
		case PROCESS_ADDR_HIGH: {
			sc_uint<8> addr_high = mem_r_data.read();
			addressing_mode_t mode = get_addressing_mode(ir_val);
			//std::cout << "PROCESS_ADDR_HIGH: Fetched addr_high=0x" << std::hex << (int)addr_high << " mode=" << mode << std::endl;
			
			if (mode == ABSOLUTE || mode == ABSOLUTE_X || mode == ABSOLUTE_Y) {
				// Assemble full 16-bit address
				effective_addr = effective_addr | (addr_high << 8);
				
				// Add index if needed
				if (mode == ABSOLUTE_X) {
					effective_addr += regfile_i->X;
				} else if (mode == ABSOLUTE_Y) {
					effective_addr += regfile_i->Y;
				}
				
				// Set memory address for operand fetch
				mem_addr.write(effective_addr);
				state = WAIT_OPERAND;
			} else if(mode == INDIRECT_X){
				// For(ind,X) read low byte of pointer, now get the high byte
				sc_uint<8> ptr_low = addr_high; // low byte from previous read
				//Read high byte from next zero page address
				mem_addr.write((effective_addr + 1) & 0xFF); // wrap in zero page
				// Store low byte temporarily in effective_addr
				effective_addr = ptr_low; 
				state = FETCH_INDIRECT_HIGH; 
			} else if(mode == INDIRECT_Y){
				// For(ind), Y read low byte of pointer, now get the high byte
				sc_uint<8> ptr_low = addr_high; // low byte from previous read
				//Read high byte from next zero page address
				mem_addr.write((effective_addr + 1) & 0xFF); // wrap in zero page
				//Store ptr_low temporarily in effective_addr
				effective_addr = ptr_low;
				state = FETCH_INDIRECT_HIGH;
			} else {
				// Should not happen
				state = EXECUTE;
			}
			break;
		}
		case FETCH_INDIRECT_HIGH: {
			// Wait one cycle to read high byte of pointer
			state = PROCESS_INDIRECT_HIGH;
			break;
		}
		case PROCESS_INDIRECT_HIGH: {
			// Read high byte of indirect pointer
			sc_uint<8> ptr_high = mem_r_data.read();
			sc_uint<8> ptr_low = effective_addr; // low byte stored previously

			// Assemble full pointer address
			effective_addr = ptr_low | (ptr_high << 8); 

			addressing_mode_t mode = get_addressing_mode(ir_val);

			//For INDIRECT Y add Y register to pointer
			if(mode == INDIRECT_Y){
				effective_addr += regfile_i->Y;
			}
			// For INDIRECT X register was added before reading pointer

			// Set memory address
			mem_addr.write(effective_addr);
			state = WAIT_OPERAND;
			break;
		}
		case EXECUTE: {
			addressing_mode_t mode = get_addressing_mode(ir_val);
			
			// Fetch operand if needed (does not apply to STORE instructions)
			if (needs_operand(ir_val) && !is_store_instruction(ir_val)) {
				if (mode == IMMEDIATE || mode == ZERO_PAGE || mode == ZERO_PAGE_X || mode == ZERO_PAGE_Y ||
					mode == ABSOLUTE || mode == ABSOLUTE_X || mode == ABSOLUTE_Y) {
					operand = mem_r_data.read();
					std::cout << "EXECUTE: Fetched operand 0x" << std::hex << (int)operand << " from address 0x" << (int)effective_addr << std::endl;
				}
			}
			
			// Execute actions based on control_unit signals
			std::cout << "EXECUTE: reg_we=" << reg_we.read() << ", reg_w_addr=" << (int)reg_w_addr.read() << std::endl;
			
			// Prepare ALU if needed (before writing to register)
			if (alu_enable.read()) {
				// NOTE: we cannot write to reg_r_addr - it is controlled by control_unit
				bool carry_flag = alu_carry_in.read() != 0;

				// Set ALU parameters
				// For MOV operations (LDA/LDX/LDY), pass operand through ALU input 'a'
				if (alu_op.read() == 0xB) {
					alu_a.write(operand);       // For MOV: pass operand through 'a' input
					alu_b.write(0);             // 'b' is unused for MOV
				} else {
					alu_a.write(reg_a_val);     // For arithmetic/logic ops: use current A value
					alu_b.write(operand);       // Operand from instruction
				}
				alu_carry_in.write(carry_flag ? 1 : 0);  // Use true Carry flag

				std::cout << "EXECUTE: Setting ALU - A=0x" << std::hex << (int)alu_a.read()
				          << " op=0x" << (int)alu_op.read() << " operand=0x" << (int)operand << std::endl;

				// Wait for one cycle to compute ALU
				state = WAIT_ALU;
				break;
			}
			
			if (reg_we.read()) {
				// Determine what to write to register
				sc_uint<8> data_to_write;
				
				if (alu_enable.read()) {
					// Use ALU result (for ADC, AND, ORA, EOR, SBC, CMP)
					data_to_write = alu_result.read();
					//std::cout << "EXECUTE: Using ALU result = 0x" << std::hex << (int)data_to_write << std::endl;
				} else {
					// Use operand directly (for LDA, LDX, LDY)
					data_to_write = operand;
					//std::cout << "EXECUTE: Using operand = 0x" << std::hex << (int)data_to_write << std::endl;
				}
				
				reg_w_data.write(data_to_write);
				std::cout << "EXECUTE: Writing 0x" << std::hex << (int)data_to_write << " to register " << (int)reg_w_addr.read() << std::endl;

				// Update tracked A register value
				if (reg_w_addr.read() == 0) {
					reg_a_val = data_to_write;
				}
			}
			
			// Handle memory write (for STORE instructions)
			if (is_store_instruction(ir_val)) {
				// CPU controls mem_we for STORE instructions
				sc_uint<8> data_to_store = reg_r_data.read(); // reg_r_addr is already set by control_unit

				mem_we.write(true);       // CPU enables memory write
				mem_w_data.write(data_to_store);
				std::cout << "EXECUTE: STORE - Writing 0x" << std::hex << (int)data_to_store << " to address 0x" << (int)effective_addr << std::endl;
			}
			
			// Update PC
			int instr_length = get_instruction_length(ir_val);
			if (pc_load.read()) {
				pc_val = pc_new.read();
			} else if (pc_inc.read()) {
				pc_val = pc_val + instr_length;
			}
			pc.write(pc_val);
			//std::cout << "EXECUTE: New PC = 0x" << std::hex << (int)pc_val << std::endl;
			
			state = FETCH;
			break;
		}
		
		case WAIT_ALU: {
			// ALU has correct parameters, we can read the result
			//std::cout << "WAIT_ALU: ALU result=0x" << std::hex << (int)alu_result.read() << std::endl;

			if (reg_we.read()) {
				sc_uint<8> data_to_write = alu_result.read();
				//std::cout << "WAIT_ALU: Using ALU result = 0x" << std::hex << (int)data_to_write << std::endl;

				reg_w_data.write(data_to_write);
				//std::cout << "WAIT_ALU: Writing 0x" << std::hex << (int)data_to_write << " to register " << (int)reg_w_addr.read() << std::endl;

				// Update tracked A register value
				if (reg_w_addr.read() == 0) {
					reg_a_val = data_to_write;
				}
			}
			
			// Update PC
			int instr_length = get_instruction_length(ir_val);
			if (pc_load.read()) {
				pc_val = pc_new.read();
			} else if (pc_inc.read()) {
				pc_val = pc_val + instr_length;
			}
			pc.write(pc_val);
			//std::cout << "WAIT_ALU: New PC = 0x" << std::hex << (int)pc_val << std::endl;
			
			state = FETCH;
			break;
		}
	}
}

SC_HAS_PROCESS(cpu);

cpu::cpu(sc_module_name name) : sc_module(name) {
	// Creating submodule instances
	alu_i = new alu("alu_i");
	regfile_i = new regfile("regfile_i");
	memory_i = new memory("memory_i");
	control_unit_i = new control_unit("control_unit_i");

	// --- ALU connections ---
	alu_i->a(alu_a);
	alu_i->b(alu_b);
	alu_i->carry_in(alu_carry_in);
	alu_i->op(alu_op);
	alu_i->result(alu_result);
	alu_i->carry(alu_carry);
	alu_i->zero(alu_zero);
	alu_i->negative(alu_negative);
	alu_i->overflow(alu_overflow);

	// --- Register file connections ---
	regfile_i->clk(clk);
	regfile_i->we(reg_we);
	regfile_i->w_addr(reg_w_addr);
	regfile_i->w_data(reg_w_data);
	regfile_i->r_addr(reg_r_addr);
	regfile_i->r_data(reg_r_data);
	regfile_i->set_flags(set_flags);
	regfile_i->zero(alu_zero);
	regfile_i->negative(alu_negative);

	// --- Flag control signal connections ---
	regfile_i->set_carry(set_carry);
	regfile_i->clear_carry(clear_carry);
	regfile_i->set_interrupt(set_interrupt);
	regfile_i->clear_interrupt(clear_interrupt);
	regfile_i->set_decimal(set_decimal);
	regfile_i->clear_decimal(clear_decimal);
	regfile_i->clear_overflow(clear_overflow);

	// --- Memory connections ---
	memory_i->clk(clk);
	memory_i->we(mem_we);
	memory_i->addr(mem_addr);
	memory_i->w_data(mem_w_data);
	memory_i->r_data(mem_r_data);

	// --- Control unit connections ---
	control_unit_i->clk(clk);
	control_unit_i->opcode(opcode);
	control_unit_i->alu_op(alu_op);
	control_unit_i->alu_enable(alu_enable);
	control_unit_i->set_flags(set_flags);
	control_unit_i->reg_we(reg_we);
	control_unit_i->reg_sel(reg_w_addr);
	control_unit_i->reg_src(reg_r_addr);
	// NOTE: mem_we will be controlled by CPU, not control_unit
	control_unit_i->mem_we(control_unit_mem_we); // Connect to unused signal
	control_unit_i->mem_oe(mem_oe);
	control_unit_i->pc_inc(pc_inc);
	control_unit_i->pc_load(pc_load);
	control_unit_i->pc_new(pc_new);
	control_unit_i->halt(halt);
	control_unit_i->irq_ack(irq_ack);
	control_unit_i->nmi_ack(nmi_ack);

	// --- Flag control signal connections ---
	control_unit_i->set_carry(set_carry);
	control_unit_i->clear_carry(clear_carry);
	control_unit_i->set_interrupt(set_interrupt);
	control_unit_i->clear_interrupt(clear_interrupt);
	control_unit_i->set_decimal(set_decimal);
	control_unit_i->clear_decimal(clear_decimal);
	control_unit_i->clear_overflow(clear_overflow);

	SC_METHOD(fetch_execute);
	sensitive << clk.pos();
	dont_initialize();
}

