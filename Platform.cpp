#include "Platform.hpp"

Platform::Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight) // constructor for the platform class, takes in arguments
{
	SDL_Init(SDL_INIT_VIDEO); // initializes the sdl video system

	window = SDL_CreateWindow(title, 0, 0, windowWidth, windowHeight, SDL_WINDOW_SHOWN); // creates an sdl window with the specific title, dimensions, and flags

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); // creates an sdl renderer, which is used for drawing graphics to the window

	texture = SDL_CreateTexture(
		renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, textureWidth, textureHeight); // creates an sdl texture, which is a surface that can be drawn to then rendered to the window, sdl_...8888 specifies that
		// each pixel will be represented by four bytes, which is why the video array in chip8 class is uint32_t
} // purpose is to initialize the sdl librarym creates a window, and sets up a renderer and a texture for drawring

Platform::~Platform() // destructor for the Platform class, its called automatically when a platfor object is destroyes
{
	// cleans up resources used by the platform object when its no longer needed
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Platform::Update(void const* buffer, int pitch) // function takes in a buffer (a pointer to a pixel data) and a pitch (the width of a row in bytes) as arguments
{
	SDL_UpdateTexture(texture, nullptr, buffer, pitch); // updates sdl texture with new pixel data from buffer
	SDL_RenderClear(renderer); // clears the renderer with a default color, usually black
	SDL_RenderCopy(renderer, texture, nullptr, nullptr); // copies the texture to the renderer
	SDL_RenderPresent(renderer); // presents the rendered graphics to the window
}// this function effectivly draws the contents of the buffer to the screen


// this function ahndles specific key presses and releases, mapping them to the keys array, each key corresponds to an index in the array. for example, if the x key is pressed, key[0] = 1, if it is released key[0] = 0
bool Platform::ProcessInput(uint8_t* keys)// this function takes a pointer to an array of uint8_t named keys as an argument, this array will be used to store the state of the Chip8 keys
{
	bool quit = false; // initializes a boolean variable quit to flase, used to indicate whether the user wants to quit the emulator

	SDL_Event event; // declares and sdlevent variable to store info about events (like keyboard presses, window events)

	while (SDL_PollEvent(&event)) // this loop proccesses events in the sdl event queue
	// SDL_PollEvent gets the next event from the queue
	{
		switch (event.type) // handles different event types
		{
			case SDL_QUIT: // indicates the user has requested to quit
			{
				quit = true;
			} break;

			case SDL_KEYDOWN: // indicates that a key has been pressed down
			{
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
					{
						quit = true;
					} break;

					case SDLK_x:
					{
						keys[0] = 1;
					} break;

					case SDLK_1:
					{
						keys[1] = 1;
					} break;

					case SDLK_2:
					{
						keys[2] = 1;
					} break;

					case SDLK_3:
					{
						keys[3] = 1;
					} break;

					case SDLK_q:
					{
						keys[4] = 1;
					} break;

					case SDLK_w:
					{
						keys[5] = 1;
					} break;

					case SDLK_e:
					{
						keys[6] = 1;
					} break;

					case SDLK_a:
					{
						keys[7] = 1;
					} break;

					case SDLK_s:
					{
						keys[8] = 1;
					} break;

					case SDLK_d:
					{
						keys[9] = 1;
					} break;

					case SDLK_z:
					{
						keys[0xA] = 1;
					} break;

					case SDLK_c:
					{
						keys[0xB] = 1;
					} break;

					case SDLK_4:
					{
						keys[0xC] = 1;
					} break;

					case SDLK_r:
					{
						keys[0xD] = 1;
					} break;

					case SDLK_f:
					{
						keys[0xE] = 1;
					} break;

					case SDLK_v:
					{
						keys[0xF] = 1;
					} break;
				}
			} break;

			case SDL_KEYUP: // indicates that a key has been released
			{
				switch (event.key.keysym.sym)
				{
					case SDLK_x:
					{
						keys[0] = 0;
					} break;

					case SDLK_1:
					{
						keys[1] = 0;
					} break;

					case SDLK_2:
					{
						keys[2] = 0;
					} break;

					case SDLK_3:
					{
						keys[3] = 0;
					} break;

					case SDLK_q:
					{
						keys[4] = 0;
					} break;

					case SDLK_w:
					{
						keys[5] = 0;
					} break;

					case SDLK_e:
					{
						keys[6] = 0;
					} break;

					case SDLK_a:
					{
						keys[7] = 0;
					} break;

					case SDLK_s:
					{
						keys[8] = 0;
					} break;

					case SDLK_d:
					{
						keys[9] = 0;
					} break;

					case SDLK_z:
					{
						keys[0xA] = 0;
					} break;

					case SDLK_c:
					{
						keys[0xB] = 0;
					} break;

					case SDLK_4:
					{
						keys[0xC] = 0;
					} break;

					case SDLK_r:
					{
						keys[0xD] = 0;
					} break;

					case SDLK_f:
					{
						keys[0xE] = 0;
					} break;

					case SDLK_v:
					{
						keys[0xF] = 0;
					} break;
				}
			} break;
		}
	}

	return quit;
}