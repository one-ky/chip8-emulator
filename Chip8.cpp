#include <fstream> // this includes teh fsteam header file which provides functionality for file input/output in c++
#include "Chip8.hpp"


// initially set PC to 0x200 in the constructor because that will be the first instruction executed
Chip8::Chip8()
{
    //Initilize PC
    pc = START_ADRESS;
}


// roms will look for memeory starting at 0x200 address as the 0x000-0x1FF was reserved in the original
const unsigned int START_ADRESS = 0x200; 
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

