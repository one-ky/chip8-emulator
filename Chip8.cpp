#include <fstream> // this includes teh fsteam header file which provides functionality for file input/output in c++
#include "Chip8.hpp"


// roms will look for memeory starting at 0x200 address as the 0x000-0x1FF was reserved in the original
const unsigned int START_ADRESS = 0x200; 
// LoadROM is a function to load the contents of chip8 tom file into the eulators memory
void Chip8::LoadROM(char const* filename)
// {void} indicates a function does not return a value
// {Chip8::} indicates that this function is a member of the chip8 class
// {char const* filename} declares a parameter named filename, which is a pointer to a constant character array. this will hold the name of the ROM file to be loaded

{
    // open file as a stream of binary and move the file pointer to the end
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    // {std::ifstream filedeclares an input file stream object names file:
    // {filename} attempts to opena the file specified by the filename param
    // {std::ios::binary} this flag specifies that the file should be opened in binary mode
    // {std::ios::ate} this flag sets the inital file position to the end of the file
    // IMPORTANCE: this line opens the ROM filee for reading, peping it to be loaded into memory, opening in binary mode ensures the data is read correctly

    if (file.is_open())
    // checks to see if the file was opened, code wont execute if the files was not opened
    {
        std::streampos size = file.tellg();
        // {std::streampos size} declare a variable named size to store the file size
        // {file.tellg} this function returns the current file position, since we opened the file with {std::ios::ate} this effectivly gives us the size of the file

        char* buffer = new char[size];
        // {char* buffer} decalres a pointer buffer that points to a dynamically allocated array of characters (bytes)
        // {new char[size]} dynamically allocates a block of memory to store the contents of the ROM file before we copy it into the CHIP8

        // go back to the begginning of the file and fill the buffer
        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        // load the rom contents into the chip8s memory, starting at 0x200
        for (long i = 0; i < size; i++)
        {
            memory[START_ADRESS + i] = buffer[i];
        }
        delete [] buffer;
    }

}

