#include <fstream> // this includes teh fsteam header file which provides functionality for file input/output in c++
#include "Chip8.hpp"
#include <random>

//roms will look for memeory starting at 0x200 address as the 0x000-0x1FF was reserved in the original
const unsigned int START_ADRESS = 0x200; 
const unsigned int FONTSET_SIZE = 80;
const unsigned int FONTSET_START_ADRESS = 0x50;

// a bit is the smallest unit of info in a computer, either a 0 or 1, a byte is a group of 8 bits, like 0001010
// a bitmap is a way to represent an image using a grid of pixels, we can fit 8 bitmaps wide and 6 bitmaps tall, so a total of 48 unique images in the emulaor
uint8_t fontset[FONTSET_SIZE] = 
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};



// initially set PC to 0x200 in the constructor because that will be the first instruction executed
Chip8::Chip8()
    :randGen(std::chrono::system_clock::now().time_since_epoch().count()) //member initalizer list, init randGen with seed
{
    //Initilize PC
    pc = START_ADRESS;

    // load the bytemaps into memory starting at 0x50
    for (unsigned int i = 0; i < FONTSET_SIZE; ++i)
    {
        memory[FONTSET_START_ADRESS + i] = fontset[i];
    }

    // Initialize RNG
    randByte = std::uniform_int_distribution<uint8_t>(0, 255U); //get a random number between 0 and 255
}



// LoadROM is a function to load the contents of chip8 tom file into the eulators memory
void Chip8::LoadROM(char const* filename)
{
    // open file as a stream of binary and move the file pointer to the end
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (file.is_open())
    // checks to see if the file was opened, code wont execute if the files was not opened
    {
        //
        std::streampos size = file.tellg();
        char* buffer = new char[size];

        // go back to the begginning of the file and fill the buffer
        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        // load the rom contents into the chip8s memory, starting at 0x200
        for (long i = 0; i < size; i++)
        {
            memory[START_ADRESS + i] = buffer[i];
        }
        // free the buffer
        delete [] buffer;
    }

}

// we will address how to call these functions later. a cycle will fetch the instruction from memory, decode the instruction, execute the instruction
// this will be called continuously in a main.cpp file 
// for now we will just define the functions

// The Instructions
// Chip8 has 32 instuctions we need to emulate

// CLS Clear the display
void Chip8::OP_00E0()
{
    memset(video, 0, sizeof(video))
}

// RET decrement stack pointer by one, set pc to return adress pushed onto stack before subrutine was called
void Chip8::OP_00EE()
{
    --sp; // move stack pointer back to last saved spot
    pc = stack[sp] // resotre the program counter from the stack, return to mem location before a jump to a subroutine
}

//jump to location nnn in memory and continue forward without saving original place
void Chip8::OP_1nnn()
{
    uint16_t address = opcode & 0x0FFFu; // & operator is helping us return the last 12 bits by masking the first 4 digits with 0, FFF show the last 12 digits
}

// call a subroutine, execute it, return to the next instruction after that call by checking the top of the stack 
void Chip8::OP_2nnn()
{
    uint16_t addresss = opcode & 0x0FFFu; // mask the fist 4 digits 

    stack[sp] = pc; // save current location to stack
    ++sp; // increment stack pointer to prep for next save
    pc = address; // jump in memory to the subroutine
}

// skip next instructoins if vf = kk, this is just incrementing pc by two again
// its like an if else statement, if the condition is true, skip the next instruction and execute the one after, if it is false, execute the next instruction
void Chip8::OP_3xkk()
{
    uint8_t Vx = (opcode & 0xF00u) >> 8u; // gives us the value of a given register, it shows the second nibble of 16bit instruction, if opcode was 0x3A1F it would return 0x0A00
    uint8_t byte = opcode & 0x00FFu; // mask gives us the last byte, bits 0-7 which show a number

    if (registers[Vx] == byte) // compare the number in register[Vx] with the byte number, if its the same increment pc by 2
    {
        pc +=2
    } // this is like an if statement, if the value of the byte is the same as the register then increment the pc by two
}












