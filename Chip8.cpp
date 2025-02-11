#include <fstream> // this includes teh fsteam header file which provides functionality for file input/output in c++
#include "Chip8.hpp"
#include <random>
#include <cstring>
#include <iostream>

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

    // Set up Function pointer table, it stores the memory address of a function. we could theoretically use switch statements or if else but this is faster
    // table is our main lookup table, it looks at the first nibble (first 4 bits, half a byte) of the opcode, in the opcode it will be a single hex
    table[0x0] = &Chip8::Table0; // secondary tables that points to opcode that starts with 0x0
    table[0x1] = &Chip8::OP_1nnn; // this sets the element at index 0x1 in the table aray to the address of Chip8::OP_1nnn, & operator is used to get the memory location
    table[0x2] = &Chip8::OP_2nnn;
    table[0x3] = &Chip8::OP_3xkk;
    table[0x4] = &Chip8::OP_4xkk;
    table[0x5] = &Chip8::OP_5xy0;
    table[0x6] = &Chip8::OP_6xkk;
    table[0x7] = &Chip8::OP_7xkk;
    table[0x8] = &Chip8::Table8; // secondary table that points to opcode that starts with 0x8, in opcode that starts with 0x8 the last bit is nessesary to differentiate which function to call
    table[0x9] = &Chip8::OP_9xy0;
    table[0xA] = &Chip8::OP_Annn;
    table[0xB] = &Chip8::OP_Bnnn;
    table[0xC] = &Chip8::OP_Cxkk;
    table[0xD] = &Chip8::OP_Dxyn;
    table[0xE] = &Chip8::TableE; // secondary table that again needs last byte to differentiate 
    table[0xF] = &Chip8::TableF; // secondary table that again needs last byte to differentiate

    // this loop will initialize all the secondary table values to a function pointer to NULL, just in case there is an opcode in the rom that is incorrect or something like that there wont be a function that is called that has no defined action
    // we initialize all the values to null, and then we update the ones that have functions after this loop here
    for (size_t i = 0; i <= 0xE; i++) // i here takes on the value of 0x0 (0000 in binay) up to 0xE (1110), which is basically 0-14 in real digits as these are the only possible values the last byte could take according to the CHIP-8 instructions
    {
        table0[i] = &Chip8::OP_NULL;
        table8[i] = &Chip8::OP_NULL;
        tableE[i] = &Chip8::OP_NULL;
    }

    // set the table0 opcode values to their function pointers
    table0[0x0] = &Chip8::OP_00E0;
    table0[0xE] = &Chip8::OP_00EE;

    table8[0x0] = &Chip8::OP_8xy0;
    table8[0x1] = &Chip8::OP_8xy1;
    table8[0x2] = &Chip8::OP_8xy2;    
    table8[0x3] = &Chip8::OP_8xy3;
    table8[0x4] = &Chip8::OP_8xy4;
    table8[0x5] = &Chip8::OP_8xy5;
    table8[0x6] = &Chip8::OP_8xy6;    
    table8[0x7] = &Chip8::OP_8xy7;
    table8[0xE] = &Chip8::OP_8xyE;

    tableE[0x1] = &Chip8::OP_ExA1;
    tableE[0xE] = &Chip8::OP_Ex9E;

    for (size_t i = 0; i <= 0x65; i++)
    {
        tableF[i] = &Chip8::OP_NULL;
    }

    tableF[0x07] = &Chip8::OP_Fx07;
    tableF[0x0A] = &Chip8::OP_Fx0A;
    tableF[0x15] = &Chip8::OP_Fx15;
    tableF[0x18] = &Chip8::OP_Fx18;
    tableF[0x1E] = &Chip8::OP_Fx1E;
    tableF[0x29] = &Chip8::OP_Fx29;
    tableF[0x33] = &Chip8::OP_Fx33;
    tableF[0x55] = &Chip8::OP_Fx55;
    tableF[0x65] = &Chip8::OP_Fx65;
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
        std::cout << "Loading ROM, SIZE: " << size << "bytes\n";
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
    else
    {
        std::cout <<"Failed to open Rom file \n";
    }

}

// One cycle of this CPU will do three things
// Fetch the next instruction in the form of opcode
// Decode the instruction to determine what operation needs to occur
// execute the instuction
void Chip8::Cycle()
{
    // Fetch
    opcode = (memory[pc] << 8u) | memory[pc + 1]; // memory is a member variable representing the chip8s memory, the | is used to combine the shifted bytes to create a single 16 bit value
    // remember the opcode first 8 bits is the function and last 8 bits is the numbers
    
    // increment the pc before we execute anything
    pc += 2;

    // decodes the first nibble of opcode, finds the corresponding function pointer in the table array, calls that function
    (this->*(table[(opcode & 0xF000u) >> 12u]))();
    // opcode...12u extracting and shifting the its to turn it into a number
    // index into the function pointer table with this[op..12u]  
    // ((*this...)) syntax for calling a member function using a pointer to it
        // this is a pointer to the current chip8 object
        // dot operator is used to access a member
        // * is used to dereference the function pointer that was retrieved from the table, dereference to get the actual function not just the mem location
    

    // decrement the delay timer if its been set
    if (delayTimer > 0)
    {
        --delayTimer;
    }

    // decrement the sound timer if its been set
    if (soundTimer > 0)
    {
        --soundTimer;
    }

}



// Table0 through OP_NULL are member functions
void Chip8::Table0() // function parameter list is empty ()
{
(this ->*table0[opcode & 0x000Fu])(); // removed *this
// {this} is a key word in c++, it is a pointer that holds the memory locatoin of the current object on which the function was called
// {*this} is dereferencing the this pointer to access the object at the {this} memory location
// {.} is the dot operator used to access the membership of an object, 
// {.*} here is used to call a member function pointer, * is used to dereference the function pointer, getting the actual value of the function from its memory address
// table0 is an array of function pointers, each element holds the memory address of a function
// opcode is variable that holds the 16bit opcode
// & 0x000Fu is to mask to isolate the last 4 bits
// IN ESSENSE
// it extracts the last nibble of the opcode, uses that nibble as an index into the table0 to get a function pointer, calls the function pointed to by that pointer, using the syntax for calling a 
    // member function through a pointer on a specific object this
}

void Chip8::Table8()
{
    (this->*(table8[opcode & 0x000Fu]))();
    // this->* is the pointer to member function syntax, 
}

void Chip8::TableE()
{
    (this->*(tableE[opcode & 0x00FFu]))();
}

void Chip8::TableF()
{
    (this->*(tableF[opcode & 0x00FFu]))();
}

void Chip8::OP_NULL()
{}



// The Instructions Chip8 has 32 instuctions we need to emulate

// CLS Clear the display
void Chip8::OP_00E0()
{
    memset(video, 0, sizeof(video));
}

// RET decrement stack pointer by one, set pc to return adress pushed onto stack before subrutine was called
void Chip8::OP_00EE()
{
    --sp; // move stack pointer back to last saved spot
    pc = stack[sp]; // resotre the program counter from the stack, return to mem location before a jump to a subroutine
}

//jump to location nnn in memory and continue forward without saving original place
void Chip8::OP_1nnn()
{
    uint16_t address = opcode & 0x0FFFu; // & operator is helping us return the last 12 bits by masking the first 4 digits with 0, FFF show the last 12 digits
    pc = address;
}

// call a subroutine, execute it, return to the next instruction after that call by checking the top of the stack 
void Chip8::OP_2nnn()
{
    uint16_t address = opcode & 0x0FFFu; // mask the fist 4 digits 

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
        pc +=2;
    } // this is like an if statement, if the value of the byte is the same as the register then increment the pc by two
}


void Chip8::OP_4xkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFU;

    if (registers[Vx] != byte) // similar to above, but only runs if the numbet in the register is not the same as the nubmer we check
    {
        pc += 2;
    }
}

// if two registers are equal, then we skip the next task
void Chip8::OP_5xy0()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vx] == registers[Vy])
    {
        pc += 2;
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
void Chip8::OP_7xkk()
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

    registers[Vx] |= registers[Vy];
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

    uint16_t sum = registers[Vx] + registers[Vy];

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
void Chip8::OP_8xy6()
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
        registers[0xF] = 0;
    }

    registers[Vx] = registers[Vy] - registers[Vx];

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
    uint16_t address = opcode & 0x0FFFu;

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
void Chip8::OP_Dxyn()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; // val stored in register Vx
    uint8_t Vy = (opcode & 0x00F0u) >> 4u; // val stored in Vy
    uint8_t height = opcode & 0x000Fu;

    std::cout << "Drawing sprite at position: (" << (int)registers[Vx] << "," << (int)registers[Vy] << ")\n";

    // these are the corrdiantes of where the sprite will be drawn on the screen, (wrapped around if they go off the screen)
    uint8_t xPos = registers[Vx] % VIDEO_WIDTH; // this is the x coordinate of the top left corner of the sprite
    uint8_t yPos = registers[Vy] % VIDEO_HEIGHT; // this is the y coordinate of the top left corner of the sprite

    registers[0xF] = 0; // initialize flag register to 0

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
                    registers[0xF] = 1; // set the flag to one
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

    uint8_t key = registers[Vx];

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

    if (!keypad[key])
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
void Chip8::OP_Fx0A()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    if (keypad[0])
    {
        registers[Vx] = 0;
    }
    else if (keypad[1])
    {
        registers[Vx] = 1;
    }
    else if (keypad[2])
    {
        registers[Vx] = 2;
    }
    else if (keypad[3])
    {
        registers[Vx] = 3;
    }
    else if (keypad[4])
    {
        registers[Vx] = 4;
    }
    else if (keypad[5])
    {
        registers[Vx] = 5;
    }
    else if (keypad[6])
    {
        registers[Vx] = 6;
    }
    else if (keypad[7])
    {
        registers[Vx] = 7;
    }
    else if (keypad[8])
    {
        registers[Vx] = 8;
    }
    else if (keypad[9])
    {
        registers[Vx] = 9;
    }
    else if (keypad[10])
    {
        registers[Vx] = 10;
    }
    else if (keypad[11])
    {
        registers[Vx] = 11;
    }
    else if (keypad[12])
    {
        registers[Vx] = 12;
    }
    else if (keypad[13])
    {
        registers[Vx] = 13;
    }
    else if (keypad[14])
    {
        registers[Vx] = 14;
    }
    else if (keypad[15])
    {
        registers[Vx] = 15;
    }
}

// set delay timer = Vx
void Chip8::OP_Fx15()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    delayTimer = registers[Vx];
}

// set timer sound to Vx
void Chip8::OP_Fx18()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    soundTimer = registers[Vx];
}

// set I = I + Vx
void Chip8::OP_Fx1E()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    index += registers[Vx];
}

// set I = location of sprite for digit Vx
void Chip8::OP_Fx29()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t digit = registers[Vx];

    index = FONTSET_START_ADRESS + (5 * digit);
}

// helps convert hexidecimal to decimal
void Chip8::OP_Fx33()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t value = registers[Vx];

    //ones place
    memory[index + 2] = value % 10;

    // tens place
    value /= 10;
    memory[index + 1] = value % 10;

    // hundreds place
    value /= 10;
    memory[index] = value % 10;
}

// store register V0 through Vx in memory starting at location I
void Chip8::OP_Fx55()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    for (uint8_t i = 0; i <= Vx; ++i)
    {
        memory[index + i] = registers[i];
    }
}

// read register V0 through Vx from memory starting at location I
void Chip8::OP_Fx65()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    for (uint8_t i = 0; i <= Vx; ++i)
    {
        registers[i] = memory[index + i];
    }
}

// Function Pointer Table