#pragma once
#include <systemc.h>


// Jednostka sterująca w stylu 6502 (prosty dekoder instrukcji)

SC_MODULE(control_unit) {
    sc_in<bool> clk;
    sc_in<sc_uint<8>> opcode; // kod operacji 6502

    // Sygnały sterujące do ALU
    sc_out<sc_uint<4>> alu_op;      // kod operacji ALU
    sc_out<bool> alu_enable;        // czy aktywować ALU
    sc_out<bool> set_flags;         // czy ustawiać flagi

    // Sygnały sterujące do flag P
    sc_out<bool> set_carry;         // ustaw flagę Carry
    sc_out<bool> clear_carry;       // wyczyść flagę Carry
    sc_out<bool> set_interrupt;     // ustaw flagę Interrupt Disable
    sc_out<bool> clear_interrupt;   // wyczyść flagę Interrupt Disable
    sc_out<bool> set_decimal;       // ustaw flagę Decimal Mode
    sc_out<bool> clear_decimal;     // wyczyść flagę Decimal Mode
    sc_out<bool> clear_overflow;    // wyczyść flagę Overflow

    // Sygnały sterujące do rejestrów
    sc_out<bool> reg_we;            // write enable dla rejestrów
    sc_out<sc_uint<3>> reg_sel;     // wybór rejestru do zapisu
    sc_out<sc_uint<3>> reg_src;     // wybór rejestru źródłowego (np. do transferów)

    // Sygnały sterujące do pamięci
    sc_out<bool> mem_we;            // write enable dla pamięci
    sc_out<bool> mem_oe;            // output enable (odczyt z pamięci)

    // Sygnały sterujące do PC
    sc_out<bool> pc_inc;            // inkrementacja PC
    sc_out<bool> pc_load;           // załaduj nową wartość do PC (np. skok)
    sc_out<sc_uint<16>> pc_new;     // nowa wartość PC (adres skoku/podprogramu)

    // Sygnały systemowe
    sc_out<bool> halt;              // zatrzymanie CPU
    sc_out<bool> irq_ack;           // potwierdzenie przerwania
    sc_out<bool> nmi_ack;           // potwierdzenie przerwania NMI 


    void process() {
        // Wyzeruj sygnały kontroli flag na początku każdego cyklu
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
                A = natychmiastowa wartość (operand po opcode)
                Ustawia flagi Z (zero) i N (negative)
                Tryb adresowania: immediate
            */
                alu_op.write(0xB);      // MOV (przekazanie operand -> A)
                alu_enable.write(false); // dezaktywj ALU
                reg_sel.write(0);       // wybierz rejestr A
                reg_we.write(true);     // zapisz do rejestru
                set_flags.write(true);  // ustaw flagi Z i N
                mem_we.write(false);    // nie zapisuj do pamięci
                mem_oe.write(false);    // nie odczytuj z pamięci (operand już pobrany)
                pc_inc.write(true);     // przejdź do następnej instrukcji
                halt.write(false);
                break;
            case 0xA5: /* LDA zp (Load Accumulator from Zero Page)
                A = wartość z pamięci pod adresem zero page
                Ustawia flagi Z (zero) i N (negative)
                Tryb adresowania: zero page
            */
                alu_op.write(0xB);      // MOV (przekazanie operand -> A)
                alu_enable.write(false); // dezaktywj ALU
                reg_sel.write(0);       // wybierz rejestr A
                reg_we.write(true);     // zapisz do rejestru
                set_flags.write(true);  // ustaw flagi Z i N
                mem_we.write(false);    // nie zapisuj do pamięci
                mem_oe.write(true);     // odczytaj z pamięci
                pc_inc.write(true);     // przejdź do następnej instrukcji
                halt.write(false);
                break;
            case 0xB5: /* LDA zp,X (Load Accumulator from Zero Page,X)
                A = wartość z pamięci pod adresem (zero page + X)
                Ustawia flagi Z (zero) i N (negative)
                Tryb adresowania: zero page,X
                */
                alu_op.write(0xB);      // MOV (przekazanie operand -> A)
                alu_enable.write(false); // dezaktywj ALU
                reg_sel.write(0);       // wybierz rejestr A
                reg_we.write(true);     // zapisz do rejestru
                set_flags.write(true);  // ustaw flagi Z i N
                mem_we.write(false);    // nie zapisuj do pamięci
                mem_oe.write(true);     // odczytaj z pamięci
                pc_inc.write(true);     // przejdź do następnej instrukcji
                halt.write(false);
                break;
            case 0xAD: /* LDA abs (Load Accumulator from Absolute Address)
                A = wartość z pamięci pod adresem absolutnym
                Ustawia flagi Z (zero) i N (negative)
                Tryb adresowania: absolute
            */
                alu_op.write(0xB);      // MOV (przekazanie operand -> A)
                alu_enable.write(false); // dezaktywj ALU
                reg_sel.write(0);       // wybierz rejestr A
                reg_we.write(true);     // zapisz do rejestru
                set_flags.write(true);  // ustaw flagi Z i N
                mem_we.write(false);    // nie zapisuj do pamięci
                mem_oe.write(true);     // odczytaj z pamięci
                pc_inc.write(true);     // przejdź do następnej instrukcji
                halt.write(false);
                break;
            case 0xBD: /* LDA abs,X (Load Accumulator from Absolute Address + X)
                A = wartość z pamięci pod adresem (abs + X)
                Ustawia flagi Z (zero) i N (negative)
                Tryb adresowania: absolute,X
            */
                alu_op.write(0xB);      // MOV (przekazanie operand -> A)
                alu_enable.write(false); // dezaktywj ALU
                reg_sel.write(0);       // wybierz rejestr A
                reg_we.write(true);     // zapisz do rejestru
                set_flags.write(true);  // ustaw flagi Z i N
                mem_we.write(false);    // nie zapisuj do pamięci
                mem_oe.write(true);     // odczytaj z pamięci
                pc_inc.write(true);     // przejdź do następnej instrukcji
                halt.write(false);
                break;
            case 0xB9: /* LDA abs,Y (Load Accumulator from Absolute Address + Y)
                A = wartość z pamięci pod adresem (abs + Y)
                Ustawia flagi Z (zero) i N (negative)
                Tryb adresowania: absolute,Y
            */
                alu_op.write(0xB);      // MOV (przekazanie operand -> A)
                alu_enable.write(false); // dezaktywj ALU
                reg_sel.write(0);       // wybierz rejestr A
                reg_we.write(true);     // zapisz do rejestru
                set_flags.write(true);  // ustaw flagi Z i N
                mem_we.write(false);    // nie zapisuj do pamięci
                mem_oe.write(true);     // odczytaj z pamięci
                pc_inc.write(true);     // przejdź do następnej instrukcji
                halt.write(false);
                break;
            case 0xA1: /* LDA (ind,X) (Load Accumulator from (Zero Page + X) indirect)
                A = wartość z pamięci pod adresem wskazanym przez (zero page + X)
                Ustawia flagi Z (zero) i N (negative)
                Tryb adresowania: (indirect,X)
            */
                alu_op.write(0xB);      // MOV (przekazanie operand -> A)
                alu_enable.write(false); // dezaktywj ALU
                reg_sel.write(0);       // wybierz rejestr A
                reg_we.write(true);     // zapisz do rejestru
                set_flags.write(true);  // ustaw flagi Z i N
                mem_we.write(false);    // nie zapisuj do pamięci
                mem_oe.write(true);     // odczytaj z pamięci
                pc_inc.write(true);     // przejdź do następnej instrukcji
                halt.write(false);
                break;
            case 0xB1: /* LDA (ind),Y (Load Accumulator from (Zero Page) indirect + Y)
                A = wartość z pamięci pod adresem wskazanym przez (zero page) + Y
                Ustawia flagi Z (zero) i N (negative)
                Tryb adresowania: (indirect),Y
            */
                alu_op.write(0xB);      // MOV (przekazanie operand -> A)
                alu_enable.write(false); // dezaktywj ALU
                reg_sel.write(0);       // wybierz rejestr A
                reg_we.write(true);     // zapisz do rejestru
                set_flags.write(true);  // ustaw flagi Z i N
                mem_we.write(false);    // nie zapisuj do pamięci
                mem_oe.write(true);     // odczytaj z pamięci
                pc_inc.write(true);     // przejdź do następnej instrukcji
                halt.write(false);
                break;

            case 0xA2: /* LDX #imm (Load X Register Immediate)
                X = natychmiastowa wartość (operand po opcode)
                Ustawia flagi Z (zero) i N (negative)
                Tryb adresowania: immediate
            */
                alu_op.write(0xB);      // MOV (przekazanie operand -> X)
                alu_enable.write(false); // dezaktywj ALU
                reg_sel.write(1);       // wybierz rejestr X
                reg_we.write(true);     // zapisz do rejestru
                set_flags.write(true);  // ustaw flagi Z i N
                mem_we.write(false);    // nie zapisuj do pamięci
                mem_oe.write(false);    // nie odczytuj z pamięci (operand już pobrany)
                pc_inc.write(true);     // przejdź do następnej instrukcji
                halt.write(false);
                break;
            case 0xA6: /* LDX zp (Load X Register from Zero Page)
                X = wartość z pamięci pod adresem zero page
                Ustawia flagi Z (zero) i N (negative)
                Tryb adresowania: zero page
            */
                alu_op.write(0xB);      // MOV (przekazanie operand -> X)
                alu_enable.write(false); // dezaktywj ALU
                reg_sel.write(1);       // wybierz rejestr X
                reg_we.write(true);     // zapisz do rejestru
                set_flags.write(true);  // ustaw flagi Z i N
                mem_we.write(false);    // nie zapisuj do pamięci
                mem_oe.write(true);     // odczytaj z pamięci
                pc_inc.write(true);     // przejdź do następnej instrukcji
                halt.write(false);
                break;
            case 0xB6: /* LDX zp,Y (Load X Register from Zero Page + Y)
                X = wartość z pamięci pod adresem (zero page + Y)
                Ustawia flagi Z (zero) i N (negative)
                Tryb adresowania: zero page,Y
            */
                alu_op.write(0xB);      // MOV (przekazanie operand -> X)
                alu_enable.write(false); // dezaktywj ALU
                reg_sel.write(1);       // wybierz rejestr X
                reg_we.write(true);     // zapisz do rejestru
                set_flags.write(true);  // ustaw flagi Z i N
                mem_we.write(false);    // nie zapisuj do pamięci
                mem_oe.write(true);     // odczytaj z pamięci
                pc_inc.write(true);     // przejdź do następnej instrukcji
                halt.write(false);
                break;
            case 0xAE: /* LDX abs (Load X Register from Absolute Address)
                X = wartość z pamięci pod adresem absolutnym
                Ustawia flagi Z (zero) i N (negative)
                Tryb adresowania: absolute
            */
                alu_op.write(0xB);      // MOV (przekazanie operand -> X)
                alu_enable.write(false); // dezaktywj ALU
                reg_sel.write(1);       // wybierz rejestr X
                reg_we.write(true);     // zapisz do rejestru
                set_flags.write(true);  // ustaw flagi Z i N
                mem_we.write(false);    // nie zapisuj do pamięci
                mem_oe.write(true);     // odczytaj z pamięci
                pc_inc.write(true);     // przejdź do następnej instrukcji
                halt.write(false);
                break;
            case 0xBE: /* LDX abs,Y (Load X Register from Absolute Address + Y)
                X = wartość z pamięci pod adresem (abs + Y)
                Ustawia flagi Z (zero) i N (negative)
                Tryb adresowania: absolute,Y
            */
                alu_op.write(0xB);      // MOV (przekazanie operand -> X)
                alu_enable.write(false); // dezaktywj ALU
                reg_sel.write(1);       // wybierz rejestr X
                reg_we.write(true);     // zapisz do rejestru
                set_flags.write(true);  // ustaw flagi Z i N
                mem_we.write(false);    // nie zapisuj do pamięci
                mem_oe.write(true);     // odczytaj z pamięci
                pc_inc.write(true);     // przejdź do następnej instrukcji
                halt.write(false);
                break;

            case 0xA0: /* LDY #imm (Load Y Register Immediate)
                Y = natychmiastowa wartość (operand po opcode)
                Ustawia flagi Z (zero) i N (negative)
                Tryb adresowania: immediate
            */
                alu_op.write(0xB);      // MOV (przekazanie operand -> Y)
                alu_enable.write(false); // dezaktywj ALU
                reg_sel.write(2);       // wybierz rejestr Y
                reg_we.write(true);     // zapisz do rejestru
                set_flags.write(true);  // ustaw flagi Z i N
                mem_we.write(false);    // nie zapisuj do pamięci
                mem_oe.write(false);    // nie odczytuj z pamięci (operand już pobrany)
                pc_inc.write(true);     // przejdź do następnej instrukcji
                halt.write(false);
                break;
            case 0xA4: /* LDY zp (Load Y Register from Zero Page)
                Y = wartość z pamięci pod adresem zero page
                Ustawia flagi Z (zero) i N (negative)
                Tryb adresowania: zero page
            */
                alu_op.write(0xB);      // MOV (przekazanie operand -> Y)
                alu_enable.write(false); // dezaktywj ALU
                reg_sel.write(2);       // wybierz rejestr Y
                reg_we.write(true);     // zapisz do rejestru
                set_flags.write(true);  // ustaw flagi Z i N
                mem_we.write(false);    // nie zapisuj do pamięci
                mem_oe.write(true);     // odczytaj z pamięci
                pc_inc.write(true);     // przejdź do następnej instrukcji
                halt.write(false);
                break;
            case 0xB4: /* LDY zp,X (Load Y Register from Zero Page + X)
                Y = wartość z pamięci pod adresem (zero page + X)
                Ustawia flagi Z (zero) i N (negative)
                Tryb adresowania: zero page,X
            */
                alu_op.write(0xB);      // MOV (przekazanie operand -> Y)
                alu_enable.write(false); // dezaktywj ALU
                reg_sel.write(2);       // wybierz rejestr Y
                reg_we.write(true);     // zapisz do rejestru
                set_flags.write(true);  // ustaw flagi Z i N
                mem_we.write(false);    // nie zapisuj do pamięci
                mem_oe.write(true);     // odczytaj z pamięci
                pc_inc.write(true);     // przejdź do następnej instrukcji
                halt.write(false);
                break;
            case 0xAC: /* LDY abs (Load Y Register from Absolute Address)
                Y = wartość z pamięci pod adresem absolutnym
                Ustawia flagi Z (zero) i N (negative)
                Tryb adresowania: absolute
            */
                alu_op.write(0xB);      // MOV (przekazanie operand -> Y)
                alu_enable.write(false); // dezaktywj ALU
                reg_sel.write(2);       // wybierz rejestr Y
                reg_we.write(true);     // zapisz do rejestru
                set_flags.write(true);  // ustaw flagi Z i N
                mem_we.write(false);    // nie zapisuj do pamięci
                mem_oe.write(true);     // odczytaj z pamięci
                pc_inc.write(true);     // przejdź do następnej instrukcji
                halt.write(false);
                break;
            case 0xBC: /* LDY abs,X (Load Y Register from Absolute Address + X)
                Y = wartość z pamięci pod adresem (abs + X)
                Ustawia flagi Z (zero) i N (negative)
                Tryb adresowania: absolute,X
            */
                alu_op.write(0xB);      // MOV (przekazanie operand -> Y)
                alu_enable.write(false); // dezaktywj ALU
                reg_sel.write(2);       // wybierz rejestr Y
                reg_we.write(true);     // zapisz do rejestru
                set_flags.write(true);  // ustaw flagi Z i N
                mem_we.write(false);    // nie zapisuj do pamięci
                mem_oe.write(true);     // odczytaj z pamięci
                pc_inc.write(true);     // przejdź do następnej instrukcji
                halt.write(false);
                break;

            case 0x85: /* STA zp (Store Accumulator in Zero Page)
                [zero page] = A
                Nie ustawia flag Z/N
                Tryb adresowania: zero page
            */
                alu_enable.write(false); // ALU niepotrzebne
                reg_src.write(0);        // wybierz rejestr A jako źródło
                reg_we.write(false);     // nie zapisuj do rejestru
                mem_we.write(true);      // zapisz do pamięci
                mem_oe.write(false);     // nie odczytuj z pamięci
                set_flags.write(false);  // nie ustawiaj flag
                pc_inc.write(true);      // przejdź do następnej instrukcji
                halt.write(false);
                break;
            case 0x95: /* STA zp,X (Store Accumulator in Zero Page + X)
                [zero page + X] = A
                Nie ustawia flag Z/N
                Tryb adresowania: zero page,X
            */
                alu_enable.write(false); // ALU niepotrzebne
                reg_src.write(0);        // wybierz rejestr A jako źródło
                reg_we.write(false);     // nie zapisuj do rejestru
                mem_we.write(true);      // zapisz do pamięci
                mem_oe.write(false);     // nie odczytuj z pamięci
                set_flags.write(false);  // nie ustawiaj flag
                pc_inc.write(true);      // przejdź do następnej instrukcji
                halt.write(false);
                break;
            case 0x8D: /* STA abs (Store Accumulator in Absolute Address)
                [abs] = A
                Nie ustawia flag Z/N
                Tryb adresowania: absolute
                UWAGA: mem_we nie powinien być true podczas fetch adresu!
            */
                alu_enable.write(false); // ALU niepotrzebne
                reg_src.write(0);        // wybierz rejestr A jako źródło
                reg_we.write(false);     // nie zapisuj do rejestru
                mem_we.write(false);     // TYMCZASOWO: nie ustawiaj mem_we
                mem_oe.write(false);     // nie odczytuj z pamięci
                set_flags.write(false);  // nie ustawiaj flag
                pc_inc.write(true);      // przejdź do następnej instrukcji
                halt.write(false);
                break;
            case 0x9D: /* STA abs,X (Store Accumulator in Absolute Address + X)
                [abs + X] = A
                Nie ustawia flag Z/N
                Tryb adresowania: absolute,X
            */
                alu_enable.write(false); // ALU niepotrzebne
                reg_src.write(0);        // wybierz rejestr A jako źródło
                reg_we.write(false);     // nie zapisuj do rejestru
                mem_we.write(true);      // zapisz do pamięci
                mem_oe.write(false);     // nie odczytuj z pamięci
                set_flags.write(false);  // nie ustawiaj flag
                pc_inc.write(true);      // przejdź do następnej instrukcji
                halt.write(false);
                break;
            case 0x99: /* STA abs,Y (Store Accumulator in Absolute Address + Y)
                [abs + Y] = A
                Nie ustawia flag Z/N
                Tryb adresowania: absolute,Y
            */
                alu_enable.write(false); // ALU niepotrzebne
                reg_src.write(0);        // wybierz rejestr A jako źródło
                reg_we.write(false);     // nie zapisuj do rejestru
                mem_we.write(true);      // zapisz do pamięci
                mem_oe.write(false);     // nie odczytuj z pamięci
                set_flags.write(false);  // nie ustawiaj flag
                pc_inc.write(true);      // przejdź do następnej instrukcji
                halt.write(false);
                break;
            case 0x81: /* STA (ind,X) (Store Accumulator in (Zero Page + X) indirect)
                [(zero page + X)] = A
                Nie ustawia flag Z/N
                Tryb adresowania: (indirect,X)
            */
                alu_enable.write(false); // ALU niepotrzebne
                reg_src.write(0);        // wybierz rejestr A jako źródło
                reg_we.write(false);     // nie zapisuj do rejestru
                mem_we.write(true);      // zapisz do pamięci
                mem_oe.write(false);     // nie odczytuj z pamięci
                set_flags.write(false);  // nie ustawiaj flag
                pc_inc.write(true);      // przejdź do następnej instrukcji
                halt.write(false);
                break;
            case 0x91: /* STA (ind),Y (Store Accumulator in (Zero Page) indirect + Y)
                [(zero page) + Y] = A
                Nie ustawia flag Z/N
                Tryb adresowania: (indirect),Y
            */
                alu_enable.write(false); // ALU niepotrzebne
                reg_src.write(0);        // wybierz rejestr A jako źródło
                reg_we.write(false);     // nie zapisuj do rejestru
                mem_we.write(true);      // zapisz do pamięci
                mem_oe.write(false);     // nie odczytuj z pamięci
                set_flags.write(false);  // nie ustawiaj flag
                pc_inc.write(true);      // przejdź do następnej instrukcji
                halt.write(false);
                break;

            case 0x86: /* STX zp (Store X Register in Zero Page)
                [zero page] = X
                Nie ustawia flag Z/N
                Tryb adresowania: zero page
            */
                alu_enable.write(false); // ALU niepotrzebne
                reg_src.write(1);        // wybierz rejestr X jako źródło
                reg_we.write(false);     // nie zapisuj do rejestru
                mem_we.write(true);      // zapisz do pamięci
                mem_oe.write(false);     // nie odczytuj z pamięci
                set_flags.write(false);  // nie ustawiaj flag
                pc_inc.write(true);      // przejdź do następnej instrukcji
                halt.write(false);
                break;
            case 0x96: /* STX zp,Y (Store X Register in Zero Page + Y)
                [zero page + Y] = X
                Nie ustawia flag Z/N
                Tryb adresowania: zero page,Y
            */
                alu_enable.write(false); // ALU niepotrzebne
                reg_src.write(1);        // wybierz rejestr X jako źródło
                reg_we.write(false);     // nie zapisuj do rejestru
                mem_we.write(true);      // zapisz do pamięci
                mem_oe.write(false);     // nie odczytuj z pamięci
                set_flags.write(false);  // nie ustawiaj flag
                pc_inc.write(true);      // przejdź do następnej instrukcji
                halt.write(false);
                break;
            case 0x8E: /* STX abs (Store X Register in Absolute Address)
                [abs] = X
                Nie ustawia flag Z/N
                Tryb adresowania: absolute
            */
                alu_enable.write(false); // ALU niepotrzebne
                reg_src.write(1);        // wybierz rejestr X jako źródło
                reg_we.write(false);     // nie zapisuj do rejestru
                mem_we.write(true);      // zapisz do pamięci
                mem_oe.write(false);     // nie odczytuj z pamięci
                set_flags.write(false);  // nie ustawiaj flag
                pc_inc.write(true);      // przejdź do następnej instrukcji
                halt.write(false);
                break;

            case 0x84: /* STY zp (Store Y Register in Zero Page)
                [zero page] = Y
                Nie ustawia flag Z/N
                Tryb adresowania: zero page
            */
                alu_enable.write(false); // ALU niepotrzebne
                reg_src.write(2);        // wybierz rejestr Y jako źródło
                reg_we.write(false);     // nie zapisuj do rejestru
                mem_we.write(true);      // zapisz do pamięci
                mem_oe.write(false);     // nie odczytuj z pamięci
                set_flags.write(false);  // nie ustawiaj flag
                pc_inc.write(true);      // przejdź do następnej instrukcji
                halt.write(false);
                break;
            case 0x94: /* STY zp,X (Store Y Register in Zero Page + X)
                [zero page + X] = Y
                Nie ustawia flag Z/N
                Tryb adresowania: zero page,X
            */
                alu_enable.write(false); // ALU niepotrzebne
                reg_src.write(2);        // wybierz rejestr Y jako źródło
                reg_we.write(false);     // nie zapisuj do rejestru
                mem_we.write(true);      // zapisz do pamięci
                mem_oe.write(false);     // nie odczytuj z pamięci
                set_flags.write(false);  // nie ustawiaj flag
                pc_inc.write(true);      // przejdź do następnej instrukcji
                halt.write(false);
                break;
            case 0x8C: /* STY abs (Store Y Register in Absolute Address)
                [abs] = Y
                Nie ustawia flag Z/N
                Tryb adresowania: absolute
            */
                alu_enable.write(false); // ALU niepotrzebne
                reg_src.write(2);        // wybierz rejestr Y jako źródło
                reg_we.write(false);     // nie zapisuj do rejestru
                mem_we.write(true);      // zapisz do pamięci
                mem_oe.write(false);     // nie odczytuj z pamięci
                set_flags.write(false);  // nie ustawiaj flag
                pc_inc.write(true);      // przejdź do następnej instrukcji
                halt.write(false);
                break;

            // --- Register Transfers ---
            case 0xAA: /* TAX (Transfer Accumulator to X)
                X = A
                Ustawia flagi Z i N
            */
                alu_op.write(0xB);      // MOV (A -> X)
                alu_enable.write(true);
                reg_sel.write(1);       // X
                reg_src.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);  // ustaw Z/N
                mem_we.write(false);
                mem_oe.write(false);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0xA8: /* TAY (Transfer Accumulator to Y)
                Y = A
                Ustawia flagi Z i N
            */
                alu_op.write(0xB);      // MOV (A -> Y)
                alu_enable.write(true);
                reg_sel.write(2);       // Y
                reg_src.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);  // ustaw Z/N
                mem_we.write(false);
                mem_oe.write(false);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0xBA: /* TSX (Transfer Stack Pointer to X)
                X = S
                Nie ustawia flag (w 6502 ustawia Z/N, ale można dodać jeśli ALU obsługuje)
            */
                alu_op.write(0xB);      // MOV (S -> X)
                alu_enable.write(true);
                reg_sel.write(1);       // X
                reg_src.write(3);       // S (Stack Pointer, załóżmy reg_src=3)
                reg_we.write(true);
                set_flags.write(false); // (opcjonalnie true jeśli chcesz Z/N)
                mem_we.write(false);
                mem_oe.write(false);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x8A: /* TXA (Transfer X to Accumulator)
                A = X
                Ustawia flagi Z i N
            */
                alu_op.write(0xB);      // MOV (X -> A)
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_src.write(1);       // X
                reg_we.write(true);
                set_flags.write(true);  // ustaw Z/N
                mem_we.write(false);
                mem_oe.write(false);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x9A: /* TXS (Transfer X to Stack Pointer)
                S = X
                Nie ustawia flag
            */
                alu_op.write(0xB);      // MOV (X -> S)
                alu_enable.write(true);
                reg_sel.write(3);       // S (Stack Pointer, załóżmy reg_sel=3)
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
                Ustawia flagi Z i N
            */
                alu_op.write(0xB);      // MOV (Y -> A)
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_src.write(2);       // Y
                reg_we.write(true);
                set_flags.write(true);  // ustaw Z/N
                mem_we.write(false);
                mem_oe.write(false);
                pc_inc.write(true);
                halt.write(false);
                break;

            // --- Stack Operations ---
            case 0x48: /* PHA (Push Accumulator on Stack)
                S = S - 1; [S] = A
                Nie ustawia flag
            */
                alu_enable.write(false); // ALU niepotrzebne
                reg_src.write(0);        // A
                reg_we.write(false);
                mem_we.write(true);      // zapisz na stos
                mem_oe.write(false);
                set_flags.write(false);
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x08: /* PHP (Push Processor Status on Stack)
                S = S - 1; [S] = P
                Nie ustawia flag
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
                Ustawia flagi Z/N
            */
                alu_op.write(0xB);      // MOV ([S] -> A)
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);  // ustaw Z/N
                mem_we.write(false);
                mem_oe.write(true);     // odczytaj ze stosu
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x28: /* PLP (Pull Processor Status from Stack)
                P = [S]; S = S + 1
                Nie ustawia flag
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
                Ustawia flagi Z i N
                Tryb adresowania: immediate
            */
                alu_op.write(0x2);      // AND
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(false);    // operand już pobrany
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x25: /* AND zp (A = A & [zp])
                Ustawia flagi Z i N
                Tryb adresowania: zero page
            */
                alu_op.write(0x2);      // AND
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);     // odczytaj operand z pamięci
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x35: /* AND zp,X (A = A & [zp + X])
                Ustawia flagi Z i N
                Tryb adresowania: zero page,X
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
                Ustawia flagi Z i N
                Tryb adresowania: absolute
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
                Ustawia flagi Z i N
                Tryb adresowania: absolute,X
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
                Ustawia flagi Z i N
                Tryb adresowania: absolute,Y
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
                Ustawia flagi Z i N
                Tryb adresowania: (indirect,X)
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
                Ustawia flagi Z i N
                Tryb adresowania: (indirect),Y
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
                Ustawia flagi Z i N
                Tryb adresowania: immediate
            */
                alu_op.write(0x3);      // OR
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(false);    // operand już pobrany
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x05: /* ORA zp (A = A | [zp])
                Ustawia flagi Z i N
                Tryb adresowania: zero page
            */
                alu_op.write(0x3);      // OR
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);     // odczytaj operand z pamięci
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x15: /* ORA zp,X (A = A | [zp + X])
                Ustawia flagi Z i N
                Tryb adresowania: zero page,X
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
                Ustawia flagi Z i N
                Tryb adresowania: absolute
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
                Ustawia flagi Z i N
                Tryb adresowania: absolute,X
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
                Ustawia flagi Z i N
                Tryb adresowania: absolute,Y
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
                Ustawia flagi Z i N
                Tryb adresowania: (indirect,X)
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
                Ustawia flagi Z i N
                Tryb adresowania: (indirect),Y
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
                Ustawia flagi Z i N
                Tryb adresowania: immediate
            */
                alu_op.write(0x4);      // XOR
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(false);    // operand już pobrany
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x45: /* EOR zp (A = A ^ [zp])
                Ustawia flagi Z i N
                Tryb adresowania: zero page
            */
                alu_op.write(0x4);      // XOR
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);     // odczytaj operand z pamięci
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x55: /* EOR zp,X (A = A ^ [zp + X])
                Ustawia flagi Z i N
                Tryb adresowania: zero page,X
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
                Ustawia flagi Z i N
                Tryb adresowania: absolute
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
                Ustawia flagi Z i N
                Tryb adresowania: absolute,X
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
                Ustawia flagi Z i N
                Tryb adresowania: absolute,Y
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
                Ustawia flagi Z i N
                Tryb adresowania: (indirect,X)
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
                Ustawia flagi Z i N
                Tryb adresowania: (indirect),Y
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
                Ustawia flagi C, Z, N, V
                Tryb adresowania: immediate
            */
                alu_op.write(0x0);      // ADC
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(false);    // operand już pobrany
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x65: /* ADC zp (A = A + [zp] + C)
                Ustawia flagi C, Z, N, V
                Tryb adresowania: zero page
            */
                alu_op.write(0x0);      // ADC
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);     // odczytaj operand z pamięci
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x75: /* ADC zp,X (A = A + [zp + X] + C)
                Ustawia flagi C, Z, N, V
                Tryb adresowania: zero page,X
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
                Ustawia flagi C, Z, N, V
                Tryb adresowania: absolute
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
                Ustawia flagi C, Z, N, V
                Tryb adresowania: absolute,X
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
                Ustawia flagi C, Z, N, V
                Tryb adresowania: absolute,Y
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
                Ustawia flagi C, Z, N, V
                Tryb adresowania: (indirect,X)
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
                Ustawia flagi C, Z, N, V
                Tryb adresowania: (indirect),Y
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
                Ustawia flagi C, Z, N, V
                Tryb adresowania: immediate
            */
                alu_op.write(0x1);      // SBC
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(false);    // operand już pobrany
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0xE5: /* SBC zp (A = A - [zp] - (1-C))
                Ustawia flagi C, Z, N, V
                Tryb adresowania: zero page
            */
                alu_op.write(0x1);      // SBC
                alu_enable.write(true);
                reg_sel.write(0);       // A
                reg_we.write(true);
                set_flags.write(true);
                mem_we.write(false);
                mem_oe.write(true);     // odczytaj operand z pamięci
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0xF5: /* SBC zp,X (A = A - [zp + X] - (1-C))
                Ustawia flagi C, Z, N, V
                Tryb adresowania: zero page,X
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
                Ustawia flagi C, Z, N, V
                Tryb adresowania: absolute
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
                Ustawia flagi C, Z, N, V
                Tryb adresowania: absolute,X
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
                Ustawia flagi C, Z, N, V
                Tryb adresowania: absolute,Y
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
                Ustawia flagi C, Z, N, V
                Tryb adresowania: (indirect,X)
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
                Ustawia flagi C, Z, N, V
                Tryb adresowania: (indirect),Y
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
                Zmniejsz wartość w pamięci pod adresem zero page o 1
                Ustawia flagi Z i N
                Tryb adresowania: zero page
            */
                alu_op.write(0x6);      // DEC (ALU: a - 1)
                alu_enable.write(true);
                reg_we.write(false);    // nie zapisuj do rejestru
                mem_we.write(true);     // zapisz do pamięci
                mem_oe.write(true);     // odczytaj z pamięci
                set_flags.write(true);  // ustaw flagi Z i N
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0xD6: /* DEC zp,X (Decrement memory at zero page + X)
                Zmniejsz wartość w pamięci pod adresem (zero page + X) o 1
                Ustawia flagi Z i N
                Tryb adresowania: zero page,X
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
                Zmniejsz wartość w pamięci pod adresem absolutnym o 1
                Ustawia flagi Z i N
                Tryb adresowania: absolute
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
                Zmniejsz wartość w pamięci pod adresem (abs + X) o 1
                Ustawia flagi Z i N
                Tryb adresowania: absolute,X
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
                Zwiększ wartość w pamięci pod adresem zero page o 1
                Ustawia flagi Z (zero) i N (negative)
                Tryb adresowania: zero page
            */
                alu_op.write(0x5);      // INC (ALU: a + 1)
                alu_enable.write(true); // aktywuj ALU
                reg_we.write(false);    // nie zapisuj do rejestru
                mem_we.write(true);     // zapisz do pamięci
                mem_oe.write(true);     // odczytaj z pamięci
                set_flags.write(true);  // ustaw flagi Z i N
                pc_inc.write(true);     // przejdź do następnej instrukcji
                halt.write(false);
                break;
            case 0xF6: /* INC zp,X (Increment memory at zero page + X)
                Zwiększ wartość w pamięci pod adresem (zero page + X) o 1
                Ustawia flagi Z i N
                Tryb adresowania: zero page,X
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
                Zwiększ wartość w pamięci pod adresem absolutnym o 1
                Ustawia flagi Z i N
                Tryb adresowania: absolute
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
                Zwiększ wartość w pamięci pod adresem (abs + X) o 1
                Ustawia flagi Z i N
                Tryb adresowania: absolute,X
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
                Ustawia flagi C, Z, N
                Tryb adresowania: accumulator
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
                Ustawia flagi C, Z, N
                Tryb adresowania: zero page
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
                Ustawia flagi C, Z, N
                Tryb adresowania: zero page,X
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
                Ustawia flagi C, Z, N
                Tryb adresowania: absolute
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
                Ustawia flagi C, Z, N
                Tryb adresowania: absolute,X
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
                Ustawia flagi C, Z, N
                Tryb adresowania: accumulator
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
                Ustawia flagi C, Z, N
                Tryb adresowania: zero page
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
                Ustawia flagi C, Z, N
                Tryb adresowania: zero page,X
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
                Ustawia flagi C, Z, N
                Tryb adresowania: absolute
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
                Ustawia flagi C, Z, N
                Tryb adresowania: absolute,X
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
                Ustawia flagi C, Z, N
                Tryb adresowania: accumulator
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
                Ustawia flagi C, Z, N
                Tryb adresowania: zero page
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
                Ustawia flagi C, Z, N
                Tryb adresowania: zero page,X
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
                Ustawia flagi C, Z, N
                Tryb adresowania: absolute
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
                Ustawia flagi C, Z, N
                Tryb adresowania: absolute,X
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
                Ustawia flagi C, Z, N
                Tryb adresowania: accumulator
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
                Ustawia flagi C, Z, N
                Tryb adresowania: zero page
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
                Ustawia flagi C, Z, N
                Tryb adresowania: zero page,X
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
                Ustawia flagi C, Z, N
                Tryb adresowania: absolute
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
                Ustawia flagi C, Z, N
                Tryb adresowania: absolute,X
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
                Tryb adresowania: absolute
            */
                pc_load.write(true);    // załaduj nowy adres do PC
                // pc_new powinien być ustawiony przez logikę fetch/decode
                pc_inc.write(false);    // nie inkrementuj PC
                halt.write(false);
                break;
            case 0x6C: /* JMP (ind) (Jump to Indirect Address)
                PC = [ind]
                Tryb adresowania: indirect
            */
                pc_load.write(true);    // załaduj nowy adres do PC
                // pc_new powinien być ustawiony przez logikę fetch/decode
                pc_inc.write(false);    // nie inkrementuj PC
                halt.write(false);
                break;
            case 0x20: /* JSR abs (Jump to Subroutine)
                Push (PC-1) na stos, PC = abs
                Tryb adresowania: absolute
            */
                // Push (PC-1) na stos powinno być obsłużone przez logikę sterującą/stosu
                pc_load.write(true);    // załaduj nowy adres do PC
                pc_inc.write(false);    // nie inkrementuj PC
                halt.write(false);
                break;

            // --- Branch Operations ---
            case 0x10: /* BPL (Branch if Positive)
                if (N==0) PC = PC + offset
                Tryb adresowania: relative
            */
                // warunek_branch = (N==0) powinien być oceniany przez logikę flag
                // if (warunek_branch) pc_load.write(true); else pc_inc.write(true);
                halt.write(false);
                break;
            case 0x30: /* BMI (Branch if Minus)
                if (N==1) PC = PC + offset
                Tryb adresowania: relative
            */
                // warunek_branch = (N==1)
                // if (warunek_branch) pc_load.write(true); else pc_inc.write(true);
                halt.write(false);
                break;
            case 0x50: /* BVC (Branch if Overflow Clear)
                if (V==0) PC = PC + offset
                Tryb adresowania: relative
            */
                // warunek_branch = (V==0)
                // if (warunek_branch) pc_load.write(true); else pc_inc.write(true);
                halt.write(false);
                break;
            case 0x70: /* BVS (Branch if Overflow Set)
                if (V==1) PC = PC + offset
                Tryb adresowania: relative
            */
                // warunek_branch = (V==1)
                // if (warunek_branch) pc_load.write(true); else pc_inc.write(true);
                halt.write(false);
                break;
            case 0x90: /* BCC (Branch if Carry Clear)
                if (C==0) PC = PC + offset
                Tryb adresowania: relative
            */
                // warunek_branch = (C==0)
                // if (warunek_branch) pc_load.write(true); else pc_inc.write(true);
                halt.write(false);
                break;
            case 0xB0: /* BCS (Branch if Carry Set)
                if (C==1) PC = PC + offset
                Tryb adresowania: relative
            */
                // warunek_branch = (C==1)
                // if (warunek_branch) pc_load.write(true); else pc_inc.write(true);
                halt.write(false);
                break;
            case 0xD0: /* BNE (Branch if Not Equal)
                if (Z==0) PC = PC + offset
                Tryb adresowania: relative
            */
                // warunek_branch = (Z==0)
                // if (warunek_branch) pc_load.write(true); else pc_inc.write(true);
                halt.write(false);
                break;
            case 0xF0: /* BEQ (Branch if Equal)
                if (Z==1) PC = PC + offset
                Tryb adresowania: relative
            */
                // warunek_branch = (Z==1)
                // if (warunek_branch) pc_load.write(true); else pc_inc.write(true);
                halt.write(false);
                break;

            // --- Status Flag Changes ---
            case 0x18: /* CLC (Clear Carry Flag)
                P.C = 0
            */
                clear_carry.write(true);    // wyczyść flagę Carry
                reg_we.write(false);        // nie zapisuj do rejestru
                alu_enable.write(false);    // nie używaj ALU
                set_flags.write(false);     // nie ustawiaj flag Z,N z ALU
                mem_we.write(false);        // nie zapisuj do pamięci
                mem_oe.write(false);        // nie odczytuj z pamięci
                pc_inc.write(true);         // przejdź do następnej instrukcji
                halt.write(false);
                break;
            case 0x38: /* SEC (Set Carry Flag)
                P.C = 1
            */
                set_carry.write(true);      // ustaw flagę Carry
                reg_we.write(false);        // nie zapisuj do rejestru
                alu_enable.write(false);    // nie używaj ALU
                set_flags.write(false);     // nie ustawiaj flag Z,N z ALU
                mem_we.write(false);        // nie zapisuj do pamięci
                mem_oe.write(false);        // nie odczytuj z pamięci
                pc_inc.write(true);         // przejdź do następnej instrukcji
                halt.write(false);
                break;
            case 0x58: /* CLI (Clear Interrupt Disable)
                P.I = 0
            */
                // Ustaw bit I=0 w rejestrze statusu (P)
                // reg_sel.write(4); // P
                // reg_we.write(true);
                // ...logika ustawiania bitu I=0...
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0x78: /* SEI (Set Interrupt Disable)
                P.I = 1
            */
                // Ustaw bit I=1 w rejestrze statusu (P)
                // reg_sel.write(4); // P
                // reg_we.write(true);
                // ...logika ustawiania bitu I=1...
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0xB8: /* CLV (Clear Overflow Flag)
                P.V = 0
            */
                // Ustaw bit V=0 w rejestrze statusu (P)
                // reg_sel.write(4); // P
                // reg_we.write(true);
                // ...logika ustawiania bitu V=0...
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0xD8: /* CLD (Clear Decimal Mode)
                P.D = 0
            */
                // Ustaw bit D=0 w rejestrze statusu (P)
                // reg_sel.write(4); // P
                // reg_we.write(true);
                // ...logika ustawiania bitu D=0...
                pc_inc.write(true);
                halt.write(false);
                break;
            case 0xF8: /* SED (Set Decimal Mode)
                P.D = 1
            */
                // Ustaw bit D=1 w rejestrze statusu (P)
                // reg_sel.write(4); // P
                // reg_we.write(true);
                // ...logika ustawiania bitu D=1...
                pc_inc.write(true);
                halt.write(false);
                break;

            // --- System Functions ---
            case 0x00: /* BRK (Force Interrupt)
                Wykonaj przerwanie programowe, zapisz PC+2 i P na stos, ustaw PC na wektor IRQ
            */
                irq_ack.write(true);    // potwierdź przerwanie IRQ
                halt.write(false);
                // pc_load.write(true); // załaduj PC z wektora IRQ
                // ...logika zapisu PC+2 i P na stos...
                break;
            case 0x40: /* RTI (Return from Interrupt)
                Odtwórz P i PC ze stosu
            */
                nmi_ack.write(true);    // potwierdź powrót z przerwania NMI
                pc_load.write(true);    // załaduj PC ze stosu
                halt.write(false);
                // ...logika odczytu P i PC ze stosu...
                break;
            case 0x60: /* RTS (Return from Subroutine)
                PC = (pop ze stosu) + 1
            */
                pc_load.write(true);    // załaduj PC ze stosu
                halt.write(false);
                // ...logika odczytu PC ze stosu i inkrementacji...
                break;

            // --- Comparison Operations ---            
            case 0xC9: /* CMP #imm (Compare A with #imm)
                Ustawia flagi C, Z, N
                Tryb adresowania: immediate
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
                Ustawia flagi C, Z, N
                Tryb adresowania: zero page
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
                Ustawia flagi C, Z, N
                Tryb adresowania: zero page,X
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
                Ustawia flagi C, Z, N
                Tryb adresowania: absolute
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
                Ustawia flagi C, Z, N
                Tryb adresowania: absolute,X
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
                Ustawia flagi C, Z, N
                Tryb adresowania: absolute,Y
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
                Ustawia flagi C, Z, N
                Tryb adresowania: (indirect,X)
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
                Ustawia flagi C, Z, N
                Tryb adresowania: (indirect),Y
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
                Ustawia flagi C, Z, N
                Tryb adresowania: immediate
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
                Ustawia flagi C, Z, N
                Tryb adresowania: zero page
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
                Ustawia flagi C, Z, N
                Tryb adresowania: absolute
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
                Ustawia flagi C, Z, N
                Tryb adresowania: immediate
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
                Ustawia flagi C, Z, N
                Tryb adresowania: zero page
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
                Ustawia flagi C, Z, N
                Tryb adresowania: absolute
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
