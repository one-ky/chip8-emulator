#include <iostream>
#include <chrono>
#include "Platform.hpp"
#include "Chip8.hpp"


int main(int argc, char** argv) // argc is the nubmer of command line arguments passed to the program, char** argv is an array of c style strings containing the command line arguments 
// what will be inputted into this function is a command line once the program is compiled, which could look like this
    // ./chip8 roms/PONG.ch8 10 5
        // chip8 is the program.exe name, roms/PONG.ch8 id the rom file, 10 is the video scale factor, 5 is the delay in milliseconds 
{
    if (argc != 4) // check to see that there are the correct number of arguments 
    {
        std::cerr << "Usage:" << argv[0] << " <Scale> <Delay> <ROM>\n"; // error message if the number of arguments is not 4
        std::exit(EXIT_FAILURE);
    }

    // setting varibles to the argumnet values
    // std::stoi converts a strign o an integer
    int videoScale = std::stoi(argv[1]); // gets the scale factor from the command line
    int cycleDelay = std::stoi(argv[2]); // gets the delay in milliseconds from the command line argument
    char const* romFilename = argv[3]; // creates a pointer to the rom filename in memory

    // initializes an object called platform from the platform class calling its constructor, we do this to initialize SDL which is in the platform.cpp file in the platform constructor which
    Platform platform("CHIP-8 Emulator", VIDEO_WIDTH * videoScale, VIDEO_HEIGHT * videoScale, VIDEO_WIDTH, VIDEO_HEIGHT); 

    Chip8 chip8; // creates an object chip8 of the chip8 class
    chip8.LoadROM(romFilename); // loads rom file

    int videoPitch = sizeof(chip8.video[0]) * VIDEO_WIDTH; // variable to store pitch of video buffer

    auto lastCycleTime = std::chrono::high_resolution_clock::now(); // decalres a varaible so store a timepoint, this records the starting time with high precision
    bool quit = false; // loop continues as long as the quit is false

    while (!quit) // this continues as long as quit is flase
    {
        quit = platform.ProcessInput(chip8.keypad); // calls the processinput member function of the platform object, passes a pointer to the chip8 keyboard array allowing processinput to
        // update the state of the chip8 keys based on keyboad input, return true if the user wants to quit
        // it basically handles user input and updates the quit variable if nessesary

        // measures the time elapsed since the last cycle
        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

        // controlls the speed of the emulation, only executes a new cycle if enough time has passed based on the cycleDay
        if (dt > cycleDelay)
        {
            lastCycleTime = currentTime;

            chip8.Cycle();

            platform.Update(chip8.video, videoPitch);
        }
    }

    return 0; // return 0, exiting the main function
}