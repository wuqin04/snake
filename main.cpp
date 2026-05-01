#include <SDL3/SDL.h>
#include <SDl3/SDL_main.h>
#include <SDL3/SDL_video.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <windows.h>
#include <cstdlib>	
#include <time.h>
#include <vector>
#include <string>

constexpr int WIDTH = 768;
constexpr int HEIGHT = 960;
constexpr int GAME_BOARD_WIDTH = 768;
constexpr int GAME_BOARD_HEIGHT = 768;
constexpr int TILE_SIZE = 48;
constexpr double DELAY = 0.1; 

struct Vector2 {
	int x;
	int y;

	bool operator==(const Vector2& other) const {
		return (x == other.x) && (y == other.y);
	}

	bool operator!=(const Vector2& other) const {
		return !(*this == other);
	}
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
	Vector2 headPos;
	std::vector<Vector2> bodyPos;
	Action action;
};

struct AppState {
	SDL_Window* window;
	SDL_Renderer* renderer;
	TTF_Font* font;
	int lastTick;
	int currTick;
	double deltaTime;
	double delayTime;
	bool isRunning;
	int score;
};

void SetupGameBoard(AppState& appState, Snake& snake);
void InputEvent(Snake& snake, SDL_Scancode scancode);
void CalcSnakePos(Snake& snake, AppState& appState);
void CalcSnakeBodyPos(Snake& snake);
void SpawnFood(Food& food, AppState& appState, Snake& snake);
void UpdateUI(AppState& appState);
void UpdateGameBoard(AppState& appState, Snake& snake, Food& food);
void MoveSnake(AppState& appState, Snake& snake);
bool IsCollide(Snake& snake);
void SpawnFirstBody(Snake& snake);
void SpawnNextBody(Snake& snake);

int SDL_main(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);

	AppState appState = {};
	appState.window = SDL_CreateWindow("Snake Game", WIDTH, HEIGHT, SDL_WINDOW_KEYBOARD_GRABBED);
	appState.renderer = SDL_CreateRenderer(appState.window, NULL);
	appState.score = 0;

	Snake snake = {};
	snake.bodyPos.reserve(100);
	snake.action = Action::None;

	Food food = {};
	food.isSpawn = false;
	srand(time(NULL));

	// game loop
	SetupGameBoard(appState, snake);

	appState.isRunning = true;
	appState.lastTick = SDL_GetTicks();
	while (appState.isRunning) {

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_EVENT_QUIT:
				appState.isRunning = false;
				break;

			case SDL_EVENT_KEY_DOWN:
				InputEvent(snake, event.key.scancode);
			}
		}

		// Clear the screen
		SDL_RenderClear(appState.renderer);

		// render background 
		SDL_SetRenderDrawColor(appState.renderer, 0, 0, 0, 255);
		SDL_RenderFillRect(appState.renderer, NULL);

		// render objects
		UpdateUI(appState);

		UpdateGameBoard(appState, snake, food);

		SDL_RenderPresent(appState.renderer);

		// calculate the delta time for the next frame and create a delay to control the snake's movement speed
		appState.currTick = SDL_GetTicks();
		appState.deltaTime = (appState.currTick - appState.lastTick) / 1000.0; // convert to seconds
		appState.lastTick = appState.currTick;
		appState.delayTime += appState.deltaTime;
	}

	SDL_DestroyRenderer(appState.renderer);
	SDL_DestroyWindow(appState.window);
	SDL_Quit();
	return 0;
}

void SetupGameBoard(AppState& appState, Snake& snake) {
	SDL_RenderClear(appState.renderer);

	// snake starting position
	snake.headPos.x = GAME_BOARD_WIDTH / 2;
	snake.headPos.y = GAME_BOARD_HEIGHT / 2;

	// spawn the first snake body
	SpawnFirstBody(snake);

	SDL_FRect snakeRect = { snake.headPos.x, snake.headPos.y, TILE_SIZE, TILE_SIZE };
	SDL_SetRenderDrawColor(appState.renderer, 128, 128, 128, 255);
	SDL_RenderFillRect(appState.renderer, &snakeRect);

	SDL_RenderPresent(appState.renderer);
}

void InputEvent(Snake& snake, SDL_Scancode scancode) {
	int checkSnakeHeadPosX, checkSnakeHeadPosY;
	switch (scancode) {
		case SDL_SCANCODE_W:
			checkSnakeHeadPosY = snake.headPos.y - TILE_SIZE;

			if (checkSnakeHeadPosY != snake.bodyPos[0].y) {
				snake.action = Action::Up;
			}

			break;
		case SDL_SCANCODE_S:
			checkSnakeHeadPosY = snake.headPos.y + TILE_SIZE;

			if (checkSnakeHeadPosY != snake.bodyPos[0].y) {
				snake.action = Action::Down;
			}

			break;
		case SDL_SCANCODE_A:
			checkSnakeHeadPosX = snake.headPos.x - TILE_SIZE;

			if (checkSnakeHeadPosX != snake.bodyPos[0].x) {
				snake.action = Action::Left;
			}

			break;
		case SDL_SCANCODE_D:
			checkSnakeHeadPosX = snake.headPos.x + TILE_SIZE;

			if (checkSnakeHeadPosX != snake.bodyPos[0].x) {
				snake.action = Action::Right;
			}

			break;
	}
}

void CalcSnakePos(Snake& snake, AppState& appState) {
	// if snake head goes out of bounds, wrap it around to the other side of the screen	
	if (snake.headPos.x < 0) {
		snake.headPos.x = GAME_BOARD_WIDTH - TILE_SIZE;
	}
	else if (snake.headPos.x >= GAME_BOARD_WIDTH) {
		snake.headPos.x = 0;
	}

	if (snake.headPos.y < HEIGHT - GAME_BOARD_HEIGHT) {
		snake.headPos.y = HEIGHT - TILE_SIZE;
	}
	else if (snake.headPos.y >= HEIGHT) {
		snake.headPos.y = HEIGHT - GAME_BOARD_HEIGHT;
	}

	// pos formula: new pos = old pos + (direction * speed * delta time)
	// snake.headPos.y = snake.headPos.y - TILE_SIZE + ( TILE_SIZE * delta time)
	if (appState.delayTime >= DELAY) {
		appState.delayTime -= DELAY;

		if (snake.action == Action::Up) {
			CalcSnakeBodyPos(snake);
			snake.headPos.y -= TILE_SIZE;
		}

		if (snake.action == Action::Down) {
			CalcSnakeBodyPos(snake);
			snake.headPos.y += TILE_SIZE;
		}

		if (snake.action == Action::Left) {
			CalcSnakeBodyPos(snake);
			snake.headPos.x -= TILE_SIZE;
		}

		if (snake.action == Action::Right) {
			CalcSnakeBodyPos(snake);
			snake.headPos.x += TILE_SIZE;
		}
	}
}

void CalcSnakeBodyPos(Snake& snake) {
	if (snake.bodyPos.size() > 1) {
		for (size_t i = snake.bodyPos.size() - 1; i > 0; i--) {
			snake.bodyPos[i] = snake.bodyPos[i - 1];
		}
	}

	snake.bodyPos[0] = snake.headPos;
}

void SpawnFood(Food& food, AppState& appState, Snake& snake) {
	while (!food.isSpawn) {
		food.pos.x = (rand() % (GAME_BOARD_WIDTH / TILE_SIZE)) * TILE_SIZE;
		food.pos.y = ((rand() % ((GAME_BOARD_HEIGHT) / TILE_SIZE)) + (HEIGHT - GAME_BOARD_HEIGHT) / TILE_SIZE) * TILE_SIZE;

		if (food.pos == snake.headPos) {
			continue;
		}

		bool isOnSnakeBody = false;
		for (int i = 0; i < snake.bodyPos.size(); i++) {
			if (food.pos == snake.bodyPos[i]) {
				isOnSnakeBody = true;
				break;
			}
		}

		if (isOnSnakeBody) {
			continue;
		}
		else {
			food.isSpawn = true;
		}
	}

	SDL_FRect foodRect = { food.pos.x, food.pos.y, TILE_SIZE, TILE_SIZE };
	SDL_SetRenderDrawColor(appState.renderer, 255, 0, 0, 255);
	SDL_RenderFillRect(appState.renderer, &foodRect);
}

void UpdateUI(AppState& appState) {
	SDL_SetRenderDrawColor(appState.renderer, 255, 255, 255, 255);
	for (int i = 0; i < 20; i++) {
		SDL_RenderLine(appState.renderer, 0, HEIGHT - GAME_BOARD_HEIGHT, WIDTH, HEIGHT - GAME_BOARD_HEIGHT);
	}
}

void UpdateGameBoard(AppState& appState, Snake& snake, Food& food) {
	CalcSnakePos(snake, appState);

	if (snake.headPos == food.pos) {
		food.isSpawn = false;
		SpawnNextBody(snake);
	}	

	SpawnFood(food, appState, snake);
	MoveSnake(appState, snake);

	if (IsCollide(snake)) {

		appState.isRunning = false;
	}
}

void MoveSnake(AppState& appState, Snake& snake) {
	SDL_FRect snakeHeadRect = { snake.headPos.x, snake.headPos.y, TILE_SIZE, TILE_SIZE };

	SDL_SetRenderDrawColor(appState.renderer, 0, 255, 100, 255);
	SDL_RenderFillRect(appState.renderer, &snakeHeadRect);

	SDL_FRect snakeBodyRect = { snake.bodyPos[0].x, snake.bodyPos[0].y, TILE_SIZE, TILE_SIZE };

	SDL_SetRenderDrawColor(appState.renderer, 255, 255, 200, 255);
	SDL_RenderFillRect(appState.renderer, &snakeBodyRect);

	if (snake.bodyPos.size() > 1) {
		for (size_t i = 1; i < snake.bodyPos.size(); i++) {
			SDL_FRect snakeNextBodyRect = { snake.bodyPos[i].x, snake.bodyPos[i].y, TILE_SIZE, TILE_SIZE };
			SDL_RenderFillRect(appState.renderer, &snakeNextBodyRect);
		}
	}
}

bool IsCollide(Snake& snake) {
	for (size_t i = 0; i < snake.bodyPos.size(); i++) {
		if (snake.headPos == snake.bodyPos[i]) {
			return true;
		}
	}
	return false;
}

// randomize pos of first body part to prevent snake from colliding with itself at the start of the game
void SpawnFirstBody(Snake& snake) {
	bool isRandomX = (rand() % 2) == 1;
	int randomX = snake.headPos.x;
	int randomY = snake.headPos.y;

	if (isRandomX) {
		randomX = (rand() % 2) == 1 ? snake.headPos.x + TILE_SIZE : snake.headPos.x - TILE_SIZE;
	}
	else {
		randomY = (rand() % 2) == 1 ? snake.headPos.y + TILE_SIZE : snake.headPos.y - TILE_SIZE;
	}

	Vector2 randomBodyPos = { randomX, randomY };
	snake.bodyPos.push_back(randomBodyPos);
}

void SpawnNextBody(Snake& snake) {
	snake.bodyPos.push_back(snake.bodyPos[snake.bodyPos.size() - 1]);
}