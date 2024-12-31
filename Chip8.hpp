// chip8.hpp

#ifndef CHIP8_HPP
#define CHIP8_HPP

#include <cstdint>
#include <random>
#include <chrono>
#include <random>

class Chip8 //class for the chip8 components
// created a class with the following components, we can now make variables with these attributes
{
public: // can be called from any scope in the program

    Chip8(); // constructor delaration, automatically runs when we create an object to setup, initializes the contents of the object

    uint8_t registers[16]{};
    uint8_t memory[4096]{};
    uint16_t index{};
    uint16_t pc{};
    uint16_t stack[16];
    uint8_t sp{};
    uint8_t delayTimer{};
    uint8_t soundTimer{};
    uint8_t keypad[16]{};
    uint32_t video[64 * 32]{};
    uint16_t opcode;

private: // cannot be called directly outside of the class, cannot be manipulated outside the class
    std::default_random_engine randGen;
    std::uniform_int_distribution<uint8_t> randByte;
};

# endif