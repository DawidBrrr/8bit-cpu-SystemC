

cmake_minimum_required(VERSION 3.15)
project(8bitcpu)

set(CMAKE_CXX_STANDARD 11)

# Pobierz SystemC automatycznie
include(FetchContent)
FetchContent_Declare(
  systemc
  GIT_REPOSITORY https://github.com/accellera-official/SystemC.git
  GIT_TAG        main
)
FetchContent_MakeAvailable(systemc)

# Dodaj foldery z nagłówkami
include_directories(${PROJECT_SOURCE_DIR}/include)

# Zbierz wszystkie pliki źródłowe
file(GLOB SRC_FILES src/*.cpp)

# Tworzenie głównego programu CPU
add_executable(cpu ${SRC_FILES})
target_link_libraries(cpu PRIVATE systemc)

# Testy jednostkowe
enable_testing()

# Collect CPU source files (excluding main.cpp for tests)
file(GLOB CPU_SRC_FILES 
    src/alu.cpp 
    src/control_unit.cpp 
    src/cpu.cpp 
    src/memory.cpp 
    src/regfile.cpp
)

file(GLOB TEST_FILES tests/*.cpp)
foreach(test_src ${TEST_FILES})
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src} ${CPU_SRC_FILES})
    target_link_libraries(${test_name} PRIVATE systemc)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()

