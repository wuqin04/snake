#include <SDL3/SDL.h>
#include <SDl3/SDL_main.h>
#include <SDL3/SDL_video.h>
#include <windows.h>
#include <cstdlib>
#include <time.h>

constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;
constexpr int TILE_SIZE = 24;

struct Vector2 {
	int x;
	int y;
};

enum Action {
	None,
	Up,
	Down,
	Left,
	Right
};

struct Food {
	Vector2 pos;
	bool isSpawn;
};

struct Snake {
	Vector2 pos;
	int length;
	Action action;
};

struct AppState {
	SDL_Window* window;
	SDL_Renderer* renderer;
};

void SetupGameBoard(AppState& appState, Snake& snake);
void CalcSnakePos(Snake& snake);
void UpdateSnakePos(Snake& snake, AppState& appState);
void spawnFood(Food& food, AppState& appState);

int SDL_main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);

	AppState appState = {};
	appState.window = SDL_CreateWindow("Snake Game", WIDTH, HEIGHT, SDL_WINDOW_KEYBOARD_GRABBED);
	appState.renderer = SDL_CreateRenderer(appState.window, NULL);

	Snake snake = {};
	snake.action = Action::None;

	Food food = {};
	food.isSpawn = false;
	srand(time(NULL));

	// game loop
	SetupGameBoard(appState, snake);

	bool running = true;
	while (running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_EVENT_QUIT:
				running = false;
				break; 

			case SDL_EVENT_KEY_DOWN:
				switch (event.key.key) {
					case SDLK_ESCAPE:
						running = false;
						break;
					
					case SDLK_W:
						if (snake.action == Action::Down) break;

						snake.action = Action::Up;
						break;

					case SDLK_S:
						if (snake.action == Action::Up) break;

						snake.action = Action::Down;
						break;
					
					case SDLK_A:
						if (snake.action == Action::Right) break;

						snake.action = Action::Left;
						break;

					case SDLK_D:
						if (snake.action == Action::Left) break;

						snake.action = Action::Right;
						break;
				}
			}
		}

#pragma region  RenderSection
		// Clear the screen
		SDL_RenderClear(appState.renderer);

		// render background 
		SDL_SetRenderDrawColor(appState.renderer, 0, 0, 0, 255);
		SDL_RenderFillRect(appState.renderer, NULL);

		// render objects
		spawnFood(food, appState);
		UpdateSnakePos(snake, appState);

		SDL_RenderPresent(appState.renderer);
#pragma endregion
	
	}
	
	SDL_DestroyRenderer(appState.renderer);
	SDL_DestroyWindow(appState.window);
	SDL_Quit();
	return 0;
}

void SetupGameBoard(AppState& appState, Snake& snake) {
	SDL_RenderClear(appState.renderer);

	// snake starting position
	snake.pos.x = 384;
	snake.pos.y = 384;

	snake.length = 1 * TILE_SIZE;

	SDL_FRect snakeRect = { snake.pos.x, snake.pos.y, TILE_SIZE, snake.length };
	SDL_SetRenderDrawColor(appState.renderer, 128, 128, 128, 255);
	SDL_RenderFillRect(appState.renderer, &snakeRect);

	SDL_RenderPresent(appState.renderer);
}

void CalcSnakePos(Snake& snake) {
	if (snake.pos.x < 0) {
		snake.pos.x = WIDTH - TILE_SIZE;
	}
	else if (snake.pos.x >= WIDTH) {
		snake.pos.x = 0;
	}

	if (snake.pos.y < 0) {
		snake.pos.y = HEIGHT - TILE_SIZE;
	}
	else if (snake.pos.y >= HEIGHT) {
		snake.pos.y = 0;
	}

	if (snake.action == Action::Up) {
		snake.pos.y -= TILE_SIZE;
	}
	else if (snake.action == Action::Down) {
		snake.pos.y += TILE_SIZE;
	}
	else if (snake.action == Action::Left) {
		snake.pos.x -= TILE_SIZE;
	}
	else if (snake.action == Action::Right) {
		snake.pos.x += TILE_SIZE;
	}

	Sleep(50);
	SDL_Log("Snake Pos: %d, %d", snake.pos.x, snake.pos.y);
}

void UpdateSnakePos(Snake& snake, AppState& appState) {
	CalcSnakePos(snake);

	// snake
	SDL_FRect snakeRect = { snake.pos.x, snake.pos.y, TILE_SIZE, snake.length };
	SDL_SetRenderDrawColor(appState.renderer, 128, 128, 128, 255);
	SDL_RenderFillRect(appState.renderer, &snakeRect);
}

void spawnFood(Food& food, AppState& appState) {
	if (!food.isSpawn) {
		food.pos.x = (rand() % (WIDTH / TILE_SIZE)) * TILE_SIZE;
		food.pos.y = (rand() % (HEIGHT / TILE_SIZE)) * TILE_SIZE;
	}

	SDL_FRect foodRect = { food.pos.x, food.pos.y, TILE_SIZE, TILE_SIZE };
	SDL_SetRenderDrawColor(appState.renderer, 169, 169, 169, 255);
	SDL_RenderFillRect(appState.renderer, &foodRect);
	food.isSpawn = true;
}