#include <SDL3/SDL.h>

int main(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow(
		"ZooEngine",
		800, 600,
		SDL_WINDOW_OPENGL
	);

	SDL_Event event;
	bool running = true;

	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT)
				running = false;
		}
	}

	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}