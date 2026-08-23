#include <SDL3/SDL.h>
#include <glad/glad.h>
#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_opengl3.h"

int main(int argc, char* argv[]) {

	SDL_Init(SDL_INIT_VIDEO);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	SDL_Window* window = SDL_CreateWindow(
		"ZooEngine",
		800, 600,
		SDL_WINDOW_OPENGL
	);

	SDL_GLContext glContext = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, glContext);

	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		SDL_Log("Failed to initialize glad");
		return -1;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui::StyleColorsDark();

	ImGui_ImplSDL3_InitForOpenGL(window, glContext);
	ImGui_ImplOpenGL3_Init("#version 330 core");

	bool running = true;
	const double dt = 1.0 / 60.0;
	double accumulator = 0.0;
	double simTime = 0.0;
	Uint64 lastTicks = SDL_GetTicks();

	float red = 0.0f, green = 0.0f, blue = 0.0f;
	float prevRed = 0.0f, prevGreen = 0.0f, prevBlue = 0.0f;

	const int FRAME_SAMPLE_COUNT = 100;
	double frameTimesMs[FRAME_SAMPLE_COUNT] = {};
	int frameSampleIndex = 0;
	int frameSamplesFilled = 0;
	double frameTimeSum = 0.0;

	Uint64 updateCount = 0;


	while (running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL3_ProcessEvent(&event);
			if (event.type == SDL_EVENT_QUIT)
				running = false;
		}

		const Uint64 currentTicks = SDL_GetTicks();
		double frameTime = (currentTicks - lastTicks) / 1000.0;
		lastTicks = currentTicks;

		if (frameTime > 0.25) frameTime = 0.25;

		const double frameMs = frameTime * 1000.0;
		frameTimeSum -= frameTimesMs[frameSampleIndex];
		frameTimesMs[frameSampleIndex] = frameMs;
		frameTimeSum += frameMs;
		frameSampleIndex = (frameSampleIndex + 1) % FRAME_SAMPLE_COUNT;
		if (frameSamplesFilled < FRAME_SAMPLE_COUNT) frameSamplesFilled++;

		const double avgFrameMs = frameTimeSum / frameSamplesFilled;
		const double fps = avgFrameMs > 0.0 ? (1000.0 / avgFrameMs) : 0.0;

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
			updateCount++;
		}

		const double alpha = accumulator / dt;
		const float r = (float)(prevRed + (red - prevRed) * alpha);
		const float g = (float)(prevGreen + (green - prevGreen) * alpha);
		const float b = (float)(prevBlue + (blue - prevBlue) * alpha);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Debug Overlay");
		ImGui::Text("Frame: %.2f ms", avgFrameMs);
		ImGui::Text("FPS: %.1f", fps);
		ImGui::Text("Update count: %llu", (unsigned long long)updateCount);
		ImGui::End();

		ImGui::Render();

		glClearColor(r, g, b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DestroyContext(glContext);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}