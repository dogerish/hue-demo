#include <stdio.h>
#ifdef _WIN32
#include <SDL2_gfxPrimitives.h>
#else
#include <SDL2/SDL2_gfxPrimitives.h>
#endif

// width and height of the window
const int WIDTH = 1600;
const int HEIGHT = 900;

struct Color
{
	int r, g, b, a;
	Color() { set(0, 0, 0, 255); } // no arguments, default to black
	Color(int _r, int _g, int _b, int _a = 255) { set(_r, _g, _b, _a); }
	Color(int rgba[]) { set(rgba[0], rgba[1], rgba[2], rgba[3]); }
	Color(Uint32 h) { set(h); }
	// set attributes
	void set(int _r, int _g, int _b, int _a = 255) { r = _r; g = _g; b = _b; a = _a; }
	// set attributes given a hexadecimal value 0xRRGGBBAA
	void set(Uint32 h)
	{
		set(
			h >> 24,
			(h >> 16) - (h >> 24 << 8),
			(h >> 8) - (h >> 16 << 8),
			h - (h >> 8 << 8)
		);
	}
};

bool init(SDL_Window *&window, SDL_Renderer *&renderer)
{
	// initialize sdl library
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{ printf("Failed to initialize SDL:\t\t%s", SDL_GetError()); return false; }
	
	// create window called hue demo at default position with the shown flag
	window = SDL_CreateWindow("Hue Demo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	                          WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	if (window == NULL)
	{ printf("Failed to create window:\t\t%s", SDL_GetError()); return false; }

	// create hardware accelerated renderer
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL)
	{ printf("Failed to create renderer:\t\t%s", SDL_GetError()); return false; }
	
	// made it here, success
	return true;
}

// returns color at given index of the hue rainbow or whatever idk
Color get_hue(int index)
{
	int value = index % 256; // the changing aspect of the end color
	int loops = index / 256; // how many times it looped over to the next position
	// 2 possible combinations, switches between them every other loop
	// even: { 255, value, 0 }
	// odd: { 255 - value, 255, 0 }
	int out[] = { (loops % 2) ? 255 - value : 255, (loops % 2) ? 255 : value, 0 };
	// only rotates once every 2 loops
	// it only has 3 values, so it's redundant to rotate more than that
	loops = loops / 2 % 3;
	// rotation algorithm; rotates items from left to right
	return Color(out[(3 - loops) % 3], out[ (4 - loops) % 3 ], out[ (5 - loops) % 3 ]);
}

void close(SDL_Window *&window, SDL_Renderer *&renderer)
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int main(int argc, char *args[])
{
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
	
	// return 1 if initialization failed
	if (!init(window, renderer)) { close(window, renderer); return 1; }

	SDL_RenderClear(renderer);
	for (int x = 0; x < WIDTH; x++)
	{
		for (int y = 0; y < HEIGHT; y++)
		{
			// get the color with some interesting relationship between x and y
			// tbh, theres no logical thought behind this
			Color color = get_hue(x*(double)y/HEIGHT);
			// set the pixel to this color
			pixelRGBA(renderer, x, y, color.r, color.g, color.b, color.a);
		}
	}

	SDL_Event event; // container for putting an event into
	bool active = true;
	while (active)
	{
		SDL_Delay(60); // limit the rate of checking to once every ~60 milliseconds
		SDL_RenderPresent(renderer); // show the renderer on the screen
		// look through all the events for a quit event, in which case stop the loops
		while (SDL_PollEvent(&event) && active) if (event.type == SDL_QUIT) active = false;
	}

	// close and return 0
	close(window, renderer);
	return 0;
}
