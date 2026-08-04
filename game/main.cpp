#include <SDL3/SDL.h>

int main(int argc, char* argv[]) {

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow(
		"ZooEngine",
		800, 600,
		SDL_WINDOW_OPENGL
	);

	SDL_Renderer* renderer = SDL_CreateRenderer(
		window,
		NULL
	);

	
	bool running = true;

	while (running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT)
				running = false;
		}

		const double now = ((double)SDL_GetTicks()) / 1000.0;
		const float red = (float) (0.5 + 0.5 * SDL_sin(now));
		const float green = (float) (0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 2 / 3));
		const float blue = (float)(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 4 / 3));
		SDL_SetRenderDrawColorFloat(renderer, red, green, blue, SDL_ALPHA_OPAQUE_FLOAT);

		SDL_RenderClear(renderer);

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	return 0;
}