#include <SDL3/SDL.h>
#include <SDl3/SDL_main.h>
#include <SDL3/SDL_video.h>
#include <windows.h>

struct Vector2 {
	int x;
	int y;
};

enum Action {
	None,
	Up = SDL_SCANCODE_W,
	Down = SDL_SCANCODE_S,
	Left = SDL_SCANCODE_A,
	Right = SDL_SCANCODE_D
};

struct AppState {
	SDL_Window* window;
	SDL_Renderer* renderer;
	Vector2 snakeStartPos;
};

void SetupGameBoard(AppState& appState);
void UpdateGameBoard(SDL_Renderer* renderer, Vector2 dir);
Vector2 Direction();

int SDL_main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
	AppState appState = {};
	appState.window = SDL_CreateWindow("Snake Game", 800, 600, SDL_WINDOW_KEYBOARD_GRABBED);
	appState.renderer = SDL_CreateRenderer(appState.window, NULL);

	// game loop
	SetupGameBoard(appState);

	bool running = true;
	while (running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_EVENT_QUIT:
				running = false;
				break; 
			}
		}
		
		// update game state
		Vector2 dir = Direction();

		// render here
		UpdateGameBoard(appState.renderer, dir);
	}
	
	SDL_DestroyRenderer(appState.renderer);
	SDL_DestroyWindow(appState.window);
	SDL_Quit();
	return 0;
}

void SetupGameBoard(AppState& appState) {
	bool getWinSize = SDL_GetWindowSize(appState.window, &appState.snakeStartPos.x, &appState.snakeStartPos.y);
	SDL_RenderClear(appState.renderer);

	// border
	SDL_SetRenderDrawColor(appState.renderer, 255, 255, 255, 255);
	SDL_RenderRect(appState.renderer, NULL);
	// snake
	SDL_FRect snakeRect = { appState.snakeStartPos.x / 2, appState.snakeStartPos.y / 2, 20, 60 };
	SDL_SetRenderDrawColor(appState.renderer, 128, 128, 128, 255);
	SDL_RenderFillRect(appState.renderer, &snakeRect);
	SDL_RenderPresent(appState.renderer);
}

void UpdateGameBoard(SDL_Renderer* renderer, Vector2 dir) {
	SDL_FRect snakeRect = { 100 + dir.x, 100 + dir.y, 20, 60 };
	SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
	SDL_RenderFillRect(renderer, &snakeRect);
}

Vector2 Direction() {
	const bool* keyboardState = SDL_GetKeyboardState(NULL);
	Vector2 dir = {};

	if (keyboardState[Action::Up]) {
		dir.y--;
	}
	
	if (keyboardState[Action::Down]) {
		dir.y++;
	}

	if (keyboardState[Action::Left]) {
		dir.x--;
	}

	if (keyboardState[Action::Right]) {
		dir.x++;
	}

	return dir;
}