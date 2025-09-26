
#include <systemc.h>
#include "alu.h"

int sc_main(int, char**) {
    sc_signal<sc_uint<8>> a_sig, b_sig, carry_in_sig;
    sc_signal<sc_uint<4>> op_sig;
    sc_signal<sc_uint<8>> result_sig;
    sc_signal<bool> carry_sig, zero_sig, negative_sig, overflow_sig;

    alu alu_inst("ALU");
    alu_inst.a(a_sig);
    alu_inst.b(b_sig);
    alu_inst.carry_in(carry_in_sig);
    alu_inst.op(op_sig);
    alu_inst.result(result_sig);
    alu_inst.carry(carry_sig);
    alu_inst.zero(zero_sig);
    alu_inst.negative(negative_sig);
    alu_inst.overflow(overflow_sig);

    // Test: ADC
    a_sig = 100; b_sig = 55; carry_in_sig = 1; op_sig = 0x0;
    sc_start(1, SC_NS);
    std::cout << "ADC: " << result_sig.read() << " carry: " << carry_sig.read() << " zero: " << zero_sig.read() << " neg: " << negative_sig.read() << " ovf: " << overflow_sig.read() << std::endl;

    // Test: SBC
    a_sig = 100; b_sig = 55; carry_in_sig = 1; op_sig = 0x1;
    sc_start(1, SC_NS);
    std::cout << "SBC: " << result_sig.read() << " carry: " << carry_sig.read() << " zero: " << zero_sig.read() << " neg: " << negative_sig.read() << " ovf: " << overflow_sig.read() << std::endl;

    // Test: AND
    a_sig = 0xF0; b_sig = 0x0F; op_sig = 0x2;
    sc_start(1, SC_NS);
    std::cout << "AND: " << result_sig.read() << std::endl;

    // Test: ORA
    a_sig = 0xF0; b_sig = 0x0F; op_sig = 0x3;
    sc_start(1, SC_NS);
    std::cout << "ORA: " << result_sig.read() << std::endl;

    // Test: EOR
    a_sig = 0xFF; b_sig = 0x0F; op_sig = 0x4;
    sc_start(1, SC_NS);
    std::cout << "EOR: " << result_sig.read() << std::endl;

    // Test: INC
    a_sig = 0x7F; op_sig = 0x5;
    sc_start(1, SC_NS);
    std::cout << "INC: " << result_sig.read() << std::endl;

    // Test: DEC
    a_sig = 0x80; op_sig = 0x6;
    sc_start(1, SC_NS);
    std::cout << "DEC: " << result_sig.read() << std::endl;

    // Test: ASL
    a_sig = 0x81; op_sig = 0x7;
    sc_start(1, SC_NS);
    std::cout << "ASL: " << result_sig.read() << " carry: " << carry_sig.read() << std::endl;

    // Test: LSR
    a_sig = 0x81; op_sig = 0x8;
    sc_start(1, SC_NS);
    std::cout << "LSR: " << result_sig.read() << " carry: " << carry_sig.read() << std::endl;

    // Test: ROL
    a_sig = 0x80; carry_in_sig = 1; op_sig = 0x9;
    sc_start(1, SC_NS);
    std::cout << "ROL: " << result_sig.read() << " carry: " << carry_sig.read() << std::endl;

    // Test: ROR
    a_sig = 0x01; carry_in_sig = 1; op_sig = 0xA;
    sc_start(1, SC_NS);
    std::cout << "ROR: " << result_sig.read() << " carry: " << carry_sig.read() << std::endl;

    // Test: MOV
    a_sig = 0xAA; op_sig = 0xB;
    sc_start(1, SC_NS);
    std::cout << "MOV: " << result_sig.read() << std::endl;

    return 0;
}
