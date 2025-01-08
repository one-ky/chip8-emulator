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


void Chip8::OP_4xkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFU;

    if (register[Vx] != byte) // similar to above, but only runs if the numbet in the register is not the same as the nubmer we check
    {
        pc += 2;
    }
}

// if two registers are equal, then we skip the next task
void Chip8::OP_5xy0()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    unit8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (register[Vx] == register[Vy])
    {
        pc += 2
    }
}

// register a register to a specific byte
void Chip8::OP_6xkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x0FFu;

    registers[Vx] = byte;
}

// increase a counter stored in a register by a value
void Chip::OP_7xkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    registers[Vx] += byte;
}


// replace one register value with another
void Chip8::OP_8xy0()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] = registers[Vy];
}

// bitwise or opereration between two registers and store value in Vx
// usecase could be for flags like if a byte represented powerups and each bit was on or off for a specific powerup, like 00101, the 1s are the powerups you have and 0s you could have but dont
void Chip8::OP_8xy1()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] |= registes[Vy];
}

// clears a bit value to 0, so set a mask bit value to 1 in the place you want, then call a flag of 1 at the location of what you want to turn it off
// so an initial flag could be 0b00000111, and lag_A could be 0b00000001, call the flag to get 0b00000110 as the value
void Chip8::OP_8xy2()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] &= registers[Vy];
}

// XOR operator to toggle on and off the bit
void Chip8::OP_8xy3()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] ^= registers[Vy];
}

// ADD with overflow, if the sum is greater than what can fit into a byte (255), register VF will be set to 1 as a flag
void Chip8:: OP_8xy4()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    uint16 sum = registers[Vx] + registers[Vy];

    if (sum > 255U)
    {
        registers[0xF] = 1; // register[0xF] is a special register called the carry flag, used to indicate if there was as overflow
    }
    else
    {
        registers[0xF] = 0;
    }

    registers[Vx] = sum & 0xFFu;
}

// if Vx > Vy, set flag register to 1, otherwise set to 0
void Chip8::OP_8xy5()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vx] > registers[Vy])
    {
        registers[0xF] = 1;
    }
    else
    {
        registers[0xF] = 0;
    }

    registers[Vx] -= registers[Vy];
}

// if the rightmost bit is 1, set VF to 1, otherwise set VF to 0, then Vx is divided by 2
void Chip8::OP_8xy()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    
    // save lsb in Vf
    registers[0xF] = (registers[Vx] & 0x1u); // hexideciaml mask binary 00000001 to isolate the last bit

    registers[Vx] >>= 1; // shifting over 1 bit in binary is the same as dividing by two

}

// Vx = Vy - Vx, if Vy > Vx set VF to 1, else 0
void Chip8::OP_8xy7()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vy] > registers[Vx])
    {
        registers[0xF] = 1;
    }
    else
    {
        register[0xF] = 0;
    }

    registers[Vx] = registeres[Vy] - registers[Vx];

}

// if the most significant bit of Vx is 1, set VF to 1, else 0. then Vx is multiplied by 2
void Chip8::OP_8xyE()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    // save msb in VF
    registers[0xF] = (registers[Vx] & 0x80u) >> 7u;

    registers[Vx] <<= 1;
}

// skip next instructions if Vx != Vy
void Chip8::OP_9xy0()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vx] != registers[Vy])
    {
        pc += 2;
    }
}

// set I = nn
void Chip8::OP_Annn()
{
    uint16_t address = opcode & 0x0FFFu;

    index = address; // index is a member of the chip8 class
}

// jump location nnn + V0
void Chip8::OP_Bnnn()
{
    uint16_t address = opcode & 0c0FFFu;

    pc = registers[0] + address;
}

// generate a random number, preform a bitewise AND with a given number, store the result in Vx
// this is controlled randomness
void Chip8::OP_Cxkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    registers[Vx] = randByte(randGen) & byte;
} 

// iterate over sprite row by row column by column
// check if screen location i
void Chip8::OP_Dxyn():
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // val stored in register Vx
    uint8_t Vy = (opcode & 0x00F0u) >> 4u; // val stored in Vy
    uint8_t height = opcode & 0x000Fu;

    // these are the corrdiantes of where the sprite will be drawn on the screen, (wrapped around if they go off the screen)
    uint8_t xPos = registers[Vx] % VIDEO_WIDTH; // this is the x coordinate of the top left corner of the sprite
    uint8_t yPos = registres[Vy] % VIDEO_HEIGHT; // this is the y coordinate of the top left corner of the sprite

    registres[0xF] = 0; // initialize flag register to 0

    for (unsigned int row = 0; row < height; ++row) // iterate over each row of the sprite for the height of the sprite, each row is a string of pixels
    {
        uint8_t spriteByte = memory[index + row]; // memory address of the current row of sprite data

        for (unsigned int col = 0; col < 8; ++col) // 
        {
            uint8_t spritePixel = spriteByte & (0x80u >> col); // shift hex mask to position of current pixel within byte to isolate single bit which is 0 if off 1 if pixel is on
            uint32_t* screenPixel = &video[(yPos + row) * VIDEO_WIDTH + (xPos + col)]; // screenPixel is a pointer to the memory location of where the current sprite pixel is, the one in the loop we are in
            // {video} is a one dimensional member array where each element corresponds to a single pixel on the chip8s display
            // {(yPos + row) * VIDEO_WIDTH + (xPos + col)} calculates the memory index where the screen pixel is equal to the sprite pixel
            
            // checks if the sprite pixel is on
            if (spritePixel)
            {
                // if screen pixel at location we want to draw sprite is on, there is a collision 
                if (*screenPixel == 0xFFFFFFFF)
                {
                    registeres[0xF] = 1; // set the flag to one
                }

                // Effectively XOR with the sprite pixel
                *screenPixel ^= 0xFFFFFFFF; // toggles the screen pixel if the sprite pixel is on
            }

        }
    }
}

// skip the next instructions if the key with the value of Vx is pressed
void Chip8::OP_Ex9E()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    uint8_t key = regisers[Vx];

    if (keypad[key])
    {
        pc += 2;
    }
}

// skip the next instruction if key with the value of Vx is NOT pressed
void Chip8::OP_ExA1()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    uint8_t key = registers[Vx];

    if (!keypad[Vx])
    {
        pc += 2;
    }
}

// set Vx = delay timer value
void Chip8::OP_Fx07()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    registers[Vx] = delayTimer;
}

// decrement pc by two to run the same instruction repeadedly