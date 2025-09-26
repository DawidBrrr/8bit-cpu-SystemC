#include <systemc.h>
#include "control_unit.h"

int sc_main(int, char**) {
    sc_signal<bool> clk_sig;
    sc_signal<sc_uint<8>> opcode_sig;
    sc_signal<sc_uint<4>> alu_op_sig;
    sc_signal<bool> alu_enable_sig, set_flags_sig;
    sc_signal<bool> reg_we_sig;
    sc_signal<sc_uint<3>> reg_sel_sig, reg_src_sig;
    sc_signal<bool> mem_we_sig, mem_oe_sig;
    sc_signal<bool> pc_inc_sig, pc_load_sig;
    sc_signal<sc_uint<16>> pc_new_sig;
    sc_signal<bool> halt_sig, irq_ack_sig, nmi_ack_sig;

    control_unit cu("CU");
    cu.clk(clk_sig);
    cu.opcode(opcode_sig);
    cu.alu_op(alu_op_sig);
    cu.alu_enable(alu_enable_sig);
    cu.set_flags(set_flags_sig);
    cu.reg_we(reg_we_sig);
    cu.reg_sel(reg_sel_sig);
    cu.reg_src(reg_src_sig);
    cu.mem_we(mem_we_sig);
    cu.mem_oe(mem_oe_sig);
    cu.pc_inc(pc_inc_sig);
    cu.pc_load(pc_load_sig);
    cu.pc_new(pc_new_sig);
    cu.halt(halt_sig);
    cu.irq_ack(irq_ack_sig);
    cu.nmi_ack(nmi_ack_sig);

    // Test LDA #imm (0xA9)
    opcode_sig = 0xA9;
    clk_sig = 0;
    sc_start(1, SC_NS);
    clk_sig = 1;
    sc_start(1, SC_NS);
    std::cout << "LDA #imm: alu_op=" << alu_op_sig.read()
              << " alu_enable=" << alu_enable_sig.read()
              << " reg_sel=" << reg_sel_sig.read()
              << " reg_we=" << reg_we_sig.read()
              << " set_flags=" << set_flags_sig.read()
              << " mem_we=" << mem_we_sig.read()
              << " mem_oe=" << mem_oe_sig.read()
              << " pc_inc=" << pc_inc_sig.read()
              << " halt=" << halt_sig.read() << std::endl;

    // Test INC zp (0xE6)
    opcode_sig = 0xE6;
    clk_sig = 0;
    sc_start(1, SC_NS);
    clk_sig = 1;
    sc_start(1, SC_NS);
    std::cout << "INC zp: alu_op=" << alu_op_sig.read()
              << " alu_enable=" << alu_enable_sig.read()
              << " reg_we=" << reg_we_sig.read()
              << " mem_we=" << mem_we_sig.read()
              << " mem_oe=" << mem_oe_sig.read()
              << " set_flags=" << set_flags_sig.read()
              << " pc_inc=" << pc_inc_sig.read()
              << " halt=" << halt_sig.read() << std::endl;

    return 0;
}
