#include <fstream> // this includes teh fsteam header file which provides functionality for file input/output in c++
#include "Chip8.hpp"

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
{
    //Initilize PC
    pc = START_ADRESS;

    // load the bytemaps into memory starting at 0x50
    for (unsigned int i = 0; i < FONTSET_SIZE; ++i)
    {
        memory[FONTSET_START_ADRESS + i] = fontset[i];
    }
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

// The Instructions
// Chip8 has 32 instuctions we need to emulate, below is the opcode
