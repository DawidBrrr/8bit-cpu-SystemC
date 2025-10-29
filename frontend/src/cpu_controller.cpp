#include "cpu_controller.h"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>

namespace {
constexpr uint16_t kProgramBaseAddress = 0x0000;
constexpr int kResetPulseCycles = 2;
const sc_core::sc_time kCycleTime(10, sc_core::SC_NS);

inline bool IsHexChar(unsigned char ch) {
    return std::isxdigit(static_cast<int>(ch)) != 0;
}
}

CPUController::CPUController()
    : cpu_instance(nullptr),
      clk(nullptr),
      is_running(false),
      sim_thread(),
      thread_shutdown(false),
      systemc_initialized(false),
    pending_cycles(0),
    io_buffer() {
    reset.write(false);
    InitializeSystemC();
    EnsureSimulationThread();
}

CPUController::~CPUController() {
    Stop();
    thread_shutdown.store(true);
    sim_cv.notify_all();
    step_done_cv.notify_all();
    if (sim_thread.joinable()) {
        sim_thread.join();
    }
    CleanupSystemC();
}

void CPUController::LoadProgram(const std::string& code) {
    if (!systemc_initialized) {
        InitializeSystemC();
    }

    Stop();
    auto program_bytes = ParseProgram(code);

    if (!cpu_instance) {
        std::cerr << "CPU instance is not available; cannot load program." << std::endl;
        return;
    }

    {
        std::lock_guard<std::mutex> lock(sim_mutex);
        for (size_t i = 0; i < 65536; ++i) {
            cpu_instance->memory_i->mem[i] = 0;
        }
        cpu_instance->memory_i->io_buffer.clear();
        io_buffer.clear();

        for (size_t i = 0; i < program_bytes.size(); ++i) {
            size_t target = static_cast<size_t>(kProgramBaseAddress) + i;
            if (target < 65536) {
                cpu_instance->memory_i->mem[target] = program_bytes[i];
            } else {
                break;
            }
        }

        cpu_instance->state = cpu::FETCH;
        cpu_instance->pc_val = kProgramBaseAddress;
        cpu_instance->pc.write(cpu_instance->pc_val);
    }

    Reset();

    if (program_bytes.empty()) {
        std::cout << "Program buffer empty; memory cleared and CPU reset." << std::endl;
    } else {
        std::cout << "Program loaded: " << program_bytes.size() << " bytes" << std::endl;
    }
}

void CPUController::Start() {
    if (!systemc_initialized) {
        InitializeSystemC();
    }

    EnsureSimulationThread();

    bool expected = false;
    if (!is_running.compare_exchange_strong(expected, true)) {
        return;
    }

    std::cout << "CPU Started" << std::endl;
    sim_cv.notify_all();
}

void CPUController::Stop() {
    bool was_running = is_running.exchange(false);
    if (!was_running) {
        return;
    }

    std::cout << "CPU Stopped" << std::endl;
    sim_cv.notify_all();
}

void CPUController::Reset() {
    if (!systemc_initialized) {
        InitializeSystemC();
    }

    Stop();

    if (!cpu_instance) {
        std::cerr << "CPU instance is not available; cannot reset." << std::endl;
        return;
    }

    {
        std::lock_guard<std::mutex> lock(sim_mutex);
        reset.write(true);
        io_buffer.clear();
        if (cpu_instance && cpu_instance->memory_i) {
            cpu_instance->memory_i->io_buffer.clear();
        }
    }

    RunCycles(kResetPulseCycles);

    {
        std::lock_guard<std::mutex> lock(sim_mutex);
        reset.write(false);
        cpu_instance->pc_val = kProgramBaseAddress;
        cpu_instance->pc.write(cpu_instance->pc_val);
    }

    RunCycles(kResetPulseCycles);

    std::cout << "CPU Reset" << std::endl;
}

void CPUController::Step() {
    if (!systemc_initialized) {
        InitializeSystemC();
    }

    Stop();
    EnsureSimulationThread();

    bool left_fetch = false;
    {
        std::lock_guard<std::mutex> lock(sim_mutex);
        if (!cpu_instance) {
            std::cerr << "CPU instance unavailable; cannot step." << std::endl;
            return;
        }
        if (cpu_instance->halted) {
            std::cout << "CPU Step" << std::endl;
            return;
        }
        left_fetch = (cpu_instance->state != cpu::FETCH);
    }

    bool instruction_completed = false;
    constexpr int kMaxInstructionCycles = 1024;

    // Advance the simulator until the CPU returns to FETCH (start of next instruction).
    for (int cycle = 0; cycle < kMaxInstructionCycles; ++cycle) {
        RunCycles(1);

        std::lock_guard<std::mutex> lock(sim_mutex);
        if (!cpu_instance) {
            break;
        }

        if (cpu_instance->halted) {
            instruction_completed = true;
            break;
        }

        cpu::cpu_state_t current_state = cpu_instance->state;
        if (!left_fetch && current_state != cpu::FETCH) {
            left_fetch = true;
        }

        if (left_fetch && current_state == cpu::FETCH) {
            instruction_completed = true;
            break;
        }
    }

    if (!instruction_completed) {
        std::cerr << "Warning: step did not complete within cycle budget." << std::endl;
    }

    std::cout << "CPU Step" << std::endl;
}

void CPUController::GetCPUState(uint8_t& a, uint8_t& x, uint8_t& y, uint8_t& s, uint8_t& p, uint16_t& pc) {
    if (!systemc_initialized || !cpu_instance || !cpu_instance->regfile_i) {
        a = x = y = s = p = 0;
        pc = 0;
        return;
    }

    std::lock_guard<std::mutex> lock(sim_mutex);

    a = static_cast<uint8_t>(cpu_instance->regfile_i->A.to_uint());
    x = static_cast<uint8_t>(cpu_instance->regfile_i->X.to_uint());
    y = static_cast<uint8_t>(cpu_instance->regfile_i->Y.to_uint());
    s = static_cast<uint8_t>(cpu_instance->regfile_i->S.to_uint());
    p = static_cast<uint8_t>(cpu_instance->regfile_i->P.to_uint());
    pc = static_cast<uint16_t>(cpu_instance->pc_val.to_uint());
}

void CPUController::InitializeSystemC() {
    std::lock_guard<std::mutex> lock(sim_mutex);
    if (systemc_initialized) {
        return;
    }

    try {
        clk = new sc_clock("frontend_clk", kCycleTime);
        cpu_instance = new cpu("frontend_cpu");
        cpu_instance->clk(*clk);
        cpu_instance->reset(reset);
        reset.write(false);

        for (size_t i = 0; i < 65536; ++i) {
            cpu_instance->memory_i->mem[i] = 0;
        }

        systemc_initialized = true;
        pending_cycles = 0;

        sc_core::sc_start(sc_core::sc_time(0, sc_core::SC_NS));
        sim_cv.notify_all();
    } catch (const std::exception& ex) {
        std::cerr << "Failed to initialize SystemC CPU: " << ex.what() << std::endl;
        if (cpu_instance) {
            delete cpu_instance;
            cpu_instance = nullptr;
        }
        if (clk) {
            delete clk;
            clk = nullptr;
        }
    }
}

void CPUController::CleanupSystemC() {
    std::lock_guard<std::mutex> lock(sim_mutex);

    pending_cycles = 0;
    systemc_initialized = false;

    if (cpu_instance) {
        delete cpu_instance;
        cpu_instance = nullptr;
    }
    if (clk) {
        delete clk;
        clk = nullptr;
    }
}

void CPUController::EnsureSimulationThread() {
    if (sim_thread.joinable()) {
        return;
    }

    thread_shutdown.store(false);
    sim_thread = std::thread(&CPUController::SimulationThread, this);
}

void CPUController::RunCycles(int cycles) {
    if (cycles <= 0 || !systemc_initialized) {
        return;
    }

    std::unique_lock<std::mutex> lock(sim_mutex);
    pending_cycles += cycles;
    sim_cv.notify_all();

    step_done_cv.wait(lock, [this]() {
        return pending_cycles == 0 || thread_shutdown.load();
    });
}

std::vector<uint8_t> CPUController::ParseProgram(const std::string& code) {
    std::vector<uint8_t> bytes;
    std::istringstream input(code);
    std::string line;

    while (std::getline(input, line)) {
        size_t semicolon = line.find(';');
        size_t hash = line.find('#');
        size_t slash = line.find("//");

        size_t comment_pos = std::string::npos;
        if (semicolon != std::string::npos) {
            comment_pos = semicolon;
        }
        if (hash != std::string::npos) {
            comment_pos = (comment_pos == std::string::npos) ? hash : std::min(comment_pos, hash);
        }
        if (slash != std::string::npos) {
            comment_pos = (comment_pos == std::string::npos) ? slash : std::min(comment_pos, slash);
        }

        if (comment_pos != std::string::npos) {
            line = line.substr(0, comment_pos);
        }

        std::stringstream line_stream(line);
        std::string token;

        while (line_stream >> token) {
            if (token.size() >= 2 && token[0] == '0' && (token[1] == 'x' || token[1] == 'X')) {
                token = token.substr(2);
            }

            token.erase(std::remove_if(token.begin(), token.end(), [](unsigned char ch) {
                return !IsHexChar(ch);
            }), token.end());

            if (token.empty()) {
                continue;
            }

            if (token.size() == 1) {
                token.insert(token.begin(), '0');
            }

            if (token.size() > 2) {
                token = token.substr(token.size() - 2);
            }

            try {
                unsigned long value = std::stoul(token, nullptr, 16);
                bytes.push_back(static_cast<uint8_t>(value & 0xFF));
            } catch (const std::exception&) {
                // Ignore malformed tokens
            }
        }
    }

    return bytes;
}

void CPUController::SimulationThread() {
    std::unique_lock<std::mutex> lock(sim_mutex);

    while (!thread_shutdown.load()) {
        if ((!is_running.load() && pending_cycles == 0) || !systemc_initialized) {
            sim_cv.wait(lock, [this]() {
                return thread_shutdown.load() || is_running.load() || pending_cycles > 0;
            });

            if (thread_shutdown.load()) {
                break;
            }

            if (!systemc_initialized) {
                continue;
            }
        }

        if (pending_cycles > 0) {
            // Advance simulation for each requested manual cycle.
            lock.unlock();
            sc_core::sc_start(kCycleTime);
            lock.lock();
            if (cpu_instance && cpu_instance->memory_i) {
                std::string& io_stream = cpu_instance->memory_i->io_buffer;
                if (!io_stream.empty()) {
                    io_buffer.append(io_stream);
                    io_stream.clear();
                }
            }
            if (pending_cycles > 0) {
                --pending_cycles;
            }
            if (cpu_instance && cpu_instance->halted) {
                is_running.store(false);
                pending_cycles = 0;
                step_done_cv.notify_all();
                continue;
            }
            if (pending_cycles == 0) {
                step_done_cv.notify_all();
            }
            continue;
        }

        if (is_running.load()) {
            // Free-running mode: keep ticking the simulation clock.
            lock.unlock();
            sc_core::sc_start(kCycleTime);
            lock.lock();
            if (cpu_instance && cpu_instance->memory_i) {
                std::string& io_stream = cpu_instance->memory_i->io_buffer;
                if (!io_stream.empty()) {
                    io_buffer.append(io_stream);
                    io_stream.clear();
                }
            }
            if (cpu_instance && cpu_instance->halted) {
                is_running.store(false);
                pending_cycles = 0;
                step_done_cv.notify_all();
            }
        }
    }
}

std::string CPUController::ConsumeIOBuffer() {
    std::lock_guard<std::mutex> lock(sim_mutex);
    std::string data = io_buffer;
    io_buffer.clear();
    return data;
}
