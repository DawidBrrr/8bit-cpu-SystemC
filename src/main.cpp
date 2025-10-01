#include <systemc.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include "cpu.h"
#include "cpu_defs.h"

SC_MODULE(testbench) {
    sc_signal<bool> clk;
    sc_signal<bool> reset;
    cpu* cpu_i;
    std::string program_file_path;

    // Function to load program from file
    bool load_program(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cout << "ERROR: Cannot open file: " << filename << std::endl;
            return false;
        }
        
        std::vector<uint8_t> program_bytes;
        std::string line;
        
        std::cout << "Loading program: " << filename << std::endl;

        while (std::getline(file, line)) {
            // Skip comments and empty lines
            if (line.empty() || line[0] == '#') continue;
            // Parse hex bytes
            std::istringstream iss(line);
            std::string hex_byte;
            
            while (iss >> hex_byte) {
                if (hex_byte[0] == '#') break; // line comment skip
                
                try {
                    uint8_t byte = std::stoi(hex_byte, nullptr, 16);
                    program_bytes.push_back(byte);
                } catch (const std::exception& e) {
                    std::cout << "ERROR parsing: " << hex_byte << std::endl;
                }
            }
        }
        
        // Załaduj do pamięci CPU
        for (size_t i = 0; i < program_bytes.size() && i < 65536; ++i) {
            cpu_i->memory_i->mem[i] = program_bytes[i];
        }
        
        std::cout << "Loaded " << program_bytes.size() << " bytes:" << std::endl;
        /*
        for (size_t i = 0; i < program_bytes.size() && i < 16; ++i) {
            std::cout << "Memory[0x" << std::hex << std::setfill('0') << std::setw(4) 
                      << i << "] = 0x" << std::setw(2) << (int)program_bytes[i] << std::endl;
        }
        */
        
        file.close();
        return true;
    }

    void clock_gen() {
        while (true) {
            clk.write(false);
            wait(5, SC_NS);
            clk.write(true);
            wait(5, SC_NS);
        }
    }

    void run() {
        std::cout << "=== Start CPU Simulation ===" << std::endl;

        // Load program from file (path from command line arguments)
        std::cout << "Loading program: " << program_file_path << std::endl;

        if (!load_program(program_file_path)) {
            std::cout << "Fallback: Failed to load program, stopping simulation" << std::endl;
            // Fallback - hardcoded program
            cpu_i->memory_i->mem[0x0002] = 0x00; // BRK (halt)
        }
        
        // Reset AFTER loading program
        reset.write(true);
        wait(20, SC_NS);
        reset.write(false);
        wait(10, SC_NS);

        // Simulation for a specified number of cycles
        for (int i = 0; i < CPU_CYCLES; ++i) {
            wait(10, SC_NS);
        }

        // Print A register value
        std::cout << "=== Simulation Result ===" << std::endl;
        std::cout << "A Register: 0x" << std::hex << (int)cpu_i->regfile_i->A << std::endl;
        std::cout << "PC: 0x" << std::hex << (int)cpu_i->pc_val << std::endl;
        std::cout << "Operand (debug): 0x" << std::hex << (int)cpu_i->operand << std::endl;
        std::cout << "IR: 0x" << std::hex << (int)cpu_i->ir_val << std::endl;
        sc_stop();
    }

    testbench(sc_module_name name, const std::string& prog_file) : sc_module(name), program_file_path(prog_file) {
        cpu_i = new cpu("cpu_i");
        cpu_i->clk(clk);
        cpu_i->reset(reset);        
        
        SC_THREAD(clock_gen);
        SC_THREAD(run);
    }
    
    SC_CTOR(testbench) {
        program_file_path = FALLBACK_PROGRAM; // fallback
        cpu_i = new cpu("cpu_i");
        cpu_i->clk(clk);
        cpu_i->reset(reset);        
        
        SC_THREAD(clock_gen);
        SC_THREAD(run);
    }

    ~testbench() {
        delete cpu_i;
    }
};

int sc_main(int argc, char* argv[]) {
    std::string program_file = FALLBACK_PROGRAM; // default program

    // Check CLI arguments for program file
    if (argc > 1) {
        program_file = argv[1];
    } else {
        std::cout << "Using default program: " << program_file << std::endl;
    }
    
    testbench tb("tb", program_file);
    sc_start();
    return 0;
}