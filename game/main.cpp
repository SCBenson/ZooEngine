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
	const double dt = 1.0 / 60.0;
	double accumulator = 0.0;
	double simTime = 0.0;
	Uint64 lastTicks = SDL_GetTicks();

	float red = 0.0f, green = 0.0f, blue = 0.0f;
	float prevRed = 0.0f, prevGreen = 0.0f, prevBlue = 0.0f;

	while (running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT)
				running = false;
		}

		const Uint64 currentTicks = SDL_GetTicks();
		double frameTime = (currentTicks - lastTicks) / 1000.0;
		lastTicks = currentTicks;

		if (frameTime > 0.25) frameTime = 0.25;

		accumulator += frameTime;

		while (accumulator >= dt) {
			prevRed = red;
			prevGreen = green;
			prevBlue = blue;

			simTime += dt;
			red = (float)(0.5 + 0.5 * SDL_sin(simTime));
			green = (float)(0.5 + 0.5 * SDL_sin(simTime + SDL_PI_D * 2 / 3));
			blue = (float)(0.5 + 0.5 * SDL_sin(simTime + SDL_PI_D * 4 / 3));

			accumulator -= dt;
		}

		const double alpha = accumulator / dt;
		const float r = (float)(prevRed + (red - prevRed) * alpha);
		const float g = (float)(prevGreen + (green - prevGreen) * alpha);
		const float b = (float)(prevBlue + (blue - prevBlue) * alpha);

		SDL_SetRenderDrawColorFloat(renderer, r, g, b, SDL_ALPHA_OPAQUE_FLOAT);
		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	return 0;
}