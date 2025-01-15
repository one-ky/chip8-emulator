#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#include <SDL.h>
#include <cstdint>

class Platform
{
public:
    Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
    ~Platform();

    void Update(void const* bugger, int pitch);
    bool ProcessInput(uint8_t* keys);

private:
    SDL_Window* window{};
    SDL_Renderer* renderer{};
    SDL_Texture* texture{};
};

#endif

