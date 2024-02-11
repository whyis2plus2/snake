#include <float.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// use assertf assert instead of libc assert
#define ASSERTF_USE_CUSTOM_ASSERT 1
#include <assertf.h>
#include <colors.h>

#include <SDL2/SDL.h>

#define GRID_SIZE 20
 
#if defined(_WIN32) || defined(__CYGWIN__)
#include <windows.h>
#define MSLEEP(_m) Sleep(_m)
#elif defined(__linux__)
#include <unistd.h>
#define MSLEEP(_m) usleep((_m) * 1000)
#else // fallback, use busy wait
#define MSLEEP(_m) mbwait(_m)
#endif

typedef SDL_Point IntVector2;

typedef enum {
	BODY_DIR_UP = 0,
	BODY_DIR_DOWN,
	BODY_DIR_LEFT,
	BODY_DIR_RIGHT
} BodyDir;

void init(void);
void event(void);
void keyboard(void);
void update(void);
void render(void);

// millisecond busy wait function
void mbwait(uint32_t milliseconds);

bool collide_rect(const SDL_Rect *r1, const SDL_Rect *r2);
int randrange(int min, int max);

struct {
	bool should_close;
	float input_cooldown;

	SDL_Window *window;
	SDL_Renderer *renderer;
	
	struct {
		IntVector2 pos;
	} food;

	struct {
		IntVector2 vel;
		size_t length;
		SDL_Rect *body;
		BodyDir *body_dirs;
	} player;
} state = {0};


int main(int argc, char *argv[]) {
	// make windows use utf-8 encoding
#if defined(_WIN32) || defined(__CYGWIN__)
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
#endif

	init();

	state.player.vel.y = -GRID_SIZE;

	while (!state.should_close) {
		event();
		keyboard();

		if (SDL_fabsf(state.input_cooldown) < FLT_EPSILON) {
			if (state.player.length > 1) for (size_t i = state.player.length - 1; i >= 1; --i) {
				state.player.body[i] = state.player.body[i - 1];
				state.player.body_dirs[i] = state.player.body_dirs[i - 1];
			}

			state.player.body[0].x += state.player.vel.x;
			state.player.body[0].y += state.player.vel.y;
			state.input_cooldown = 1.0f;
		} else {
			state.input_cooldown -= 0.1f;
		}

		update();
		render();
		MSLEEP(10);
	}

	SDL_DestroyWindow(state.window);
	SDL_DestroyRenderer(state.renderer);
	SDL_Quit();

	free(state.player.body);

	return 0;
}

void init(void) {
	srand(SDL_static_cast(unsigned int, clock()));

	assertf(SDL_Init(SDL_INIT_VIDEO) == 0, "Failed to init SDL2: %s\n", SDL_GetError());

	state.window = SDL_CreateWindow("snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);

	assertf(state.window, "Failed to create window: %s\n", SDL_GetError());

	state.renderer = SDL_CreateRenderer(state.window, -1, 0);

	assertf(state.renderer, "Failed to create renderer: %s\n", SDL_GetError());

	IntVector2 window_dim = {0}; SDL_GetWindowSizeInPixels(state.window, &window_dim.x, &window_dim.y);

	state.player.length = 1;
	state.player.body = realloc(state.player.body, sizeof(*state.player.body) * state.player.length);
	
	assertf(state.player.body != NULL, "Failed to create player\n");

	state.player.body[0].x = window_dim.x/2; state.player.body[0].x -= GRID_SIZE/2;
	state.player.body[0].y = window_dim.y/2; state.player.body[0].y -= GRID_SIZE/2;

	state.player.body_dirs = realloc(state.player.body_dirs, sizeof(*state.player.body_dirs) * state.player.length);
	assertf(state.player.body_dirs != NULL, "Failed to create array of player segment dirs\n");
	state.player.body_dirs[0] = BODY_DIR_DOWN;

	state.food.pos.x = randrange((GRID_SIZE/2) / GRID_SIZE, (window_dim.x - GRID_SIZE/2) / GRID_SIZE) * GRID_SIZE - GRID_SIZE/2;
	state.food.pos.y = randrange((GRID_SIZE/2) / GRID_SIZE, (window_dim.y - GRID_SIZE/2) / GRID_SIZE) * GRID_SIZE + GRID_SIZE/2;
}

void event(void) {
	for (SDL_Event ev; SDL_PollEvent(&ev);) {
		switch (ev.type) {
			case SDL_QUIT:
				state.should_close = true;
				break;

			default:
				break;
		}
	}
}

void keyboard(void) {
	SDL_PumpEvents();

	int len_keys = 0;
	const uint8_t *keys = SDL_GetKeyboardState(&len_keys);

	for (size_t i = 0; i < len_keys; ++i) {
		if (!keys[i]) {continue;}

		const SDL_Keycode key = SDL_GetKeyFromScancode(i);
		
		switch (key) {
			case (SDLK_a):
				if (state.player.vel.x != GRID_SIZE) {
					state.player.vel.x = -GRID_SIZE; state.player.body_dirs[0] = BODY_DIR_LEFT;}
				state.player.vel.y = 0;
				break;

			case (SDLK_d):
				if (state.player.vel.x != -GRID_SIZE) {
					state.player.vel.x = GRID_SIZE; state.player.body_dirs[0] = BODY_DIR_RIGHT;}
				state.player.vel.y = 0;
				break;

			case (SDLK_w):
				state.player.vel.x = 0;
				if (state.player.vel.y != GRID_SIZE) {
					state.player.vel.y = -GRID_SIZE; state.player.body_dirs[0] = BODY_DIR_UP;}
				break;

			case (SDLK_s):
				state.player.vel.x = 0;
				if (state.player.vel.y != -GRID_SIZE) {
					state.player.vel.y = GRID_SIZE; state.player.body_dirs[0] = BODY_DIR_DOWN;}
				break;

			default:
				break;
		}
	}
}

void update(void) {
	const SDL_Rect food_rect = {state.food.pos.x, state.food.pos.y, GRID_SIZE/2, GRID_SIZE/2};
	IntVector2 window_dim = {0}; SDL_GetWindowSizeInPixels(state.window, &window_dim.x, &window_dim.y);

	if (collide_rect(&state.player.body[0], &food_rect)) {
		state.food.pos.x = randrange((GRID_SIZE/2) / GRID_SIZE, (window_dim.x - GRID_SIZE/2) / GRID_SIZE) * GRID_SIZE - GRID_SIZE/2;
		state.food.pos.y = randrange((GRID_SIZE/2) / GRID_SIZE, (window_dim.y - GRID_SIZE/2) / GRID_SIZE) * GRID_SIZE + GRID_SIZE/2;

		++state.player.length;
		state.player.body = realloc(state.player.body, sizeof(*state.player.body) * state.player.length);
		state.player.body[state.player.length - 1].x = -100 * GRID_SIZE;
		state.player.body[state.player.length - 1].y = -100 * GRID_SIZE;

		state.player.body_dirs = realloc(state.player.body_dirs, sizeof(*state.player.body_dirs) * state.player.length);
	}

	bool is_player_oob = state.player.body[0].x > window_dim.x || \
						 state.player.body[0].y > window_dim.y || \
						 state.player.body[0].x < 0 || \
						 state.player.body[0].y < 0;

	bool is_food_oob = state.food.pos.x > window_dim.x || \
					   state.food.pos.y > window_dim.y || \
					   state.food.pos.x < 0 || \
					   state.food.pos.y < 0;

	if (is_food_oob) {
		state.food.pos.x = randrange((GRID_SIZE/2) / GRID_SIZE, (window_dim.x - GRID_SIZE/2) / GRID_SIZE) * GRID_SIZE - GRID_SIZE/2;
		state.food.pos.y = randrange((GRID_SIZE/2) / GRID_SIZE, (window_dim.y - GRID_SIZE/2) / GRID_SIZE) * GRID_SIZE + GRID_SIZE/2;
	}

	if (is_player_oob) {
		state.player.length = 1;
		state.player.body = realloc(state.player.body, sizeof(*state.player.body) * state.player.length);
		state.player.body[0].x = window_dim.x/2; state.player.body[0].x -= GRID_SIZE/2;
		state.player.body[0].y = window_dim.y/2; state.player.body[0].y -= GRID_SIZE/2;
	}

	for (size_t i = 1; i < state.player.length; ++i) {
		if (state.player.body[0].x == state.player.body[i].x && state.player.body[0].y == state.player.body[i].y) {
			state.player.length = 1;
			state.player.body = realloc(state.player.body, sizeof(*state.player.body) * state.player.length);
			state.player.body[0].x = window_dim.x/2; state.player.body[0].x -= GRID_SIZE/2;
			state.player.body[0].y = window_dim.y/2; state.player.body[0].y -= GRID_SIZE/2;
			break;
		}
	}

	for (size_t i = 0; i < state.player.length; ++i) {
		if (state.player.body[i].w != GRID_SIZE/2) state.player.body[i].w = GRID_SIZE/2;
		if (state.player.body[i].h != GRID_SIZE/2) state.player.body[i].h = GRID_SIZE/2;
	}
}

void render(void) {
	const SDL_Rect food_rect = {state.food.pos.x, state.food.pos.y, GRID_SIZE/2, GRID_SIZE/2};

	SDL_SetRenderDrawColor(state.renderer, COLOR_BLACK);
	SDL_RenderClear(state.renderer);

	for (size_t i = 0; i < state.player.length; ++i) {
		SDL_Rect r = state.player.body[i];
		SDL_Color c = (i % 2 == 1) ? (SDL_Color){0x00, 0xCF, 0x00} : (SDL_Color){0x00, 0x7F, 0x00};

		if (i > 0) switch (state.player.body_dirs[i]) {
			case BODY_DIR_DOWN:
				r.h = GRID_SIZE;
				break;

			case BODY_DIR_UP:
				r.h = GRID_SIZE;
				r.y -= GRID_SIZE/2;
				break;

			case BODY_DIR_RIGHT:
				r.w = GRID_SIZE;
				break;

			case BODY_DIR_LEFT:
				r.w = GRID_SIZE;
				r.x -= GRID_SIZE/2;
		}

		SDL_SetRenderDrawColor(state.renderer, c.r, c.g, c.b, 0xFF);
		SDL_RenderDrawRect(state.renderer, &r);
		SDL_RenderFillRect(state.renderer, &r);
	}

	SDL_SetRenderDrawColor(state.renderer, COLOR_RED);
	SDL_RenderDrawRect(state.renderer, &food_rect);
	SDL_RenderFillRect(state.renderer, &food_rect);

	SDL_RenderPresent(state.renderer);
}

void mbwait(uint32_t milliseconds) {
	const float rate = CLOCKS_PER_SEC/1000.0f;
	const clock_t time = clock();

	while ((float)(clock() - time) < milliseconds * rate) {
		// prevent gcc from removing the loop using volatile empty inline asm
		__asm__ volatile ("");
	};
}

bool collide_rect(const SDL_Rect *r1, const SDL_Rect *r2) {
	return \
		(r1->x + r1->w >= r2->x) && \
		(r1->x <= r2->x + r2->w) && \
		(r1->y + r1->h >= r2->y) && \
		(r1->y <= r2->y + r2->h);
}

int randrange(int min, int max) {
	assertf(min <= max, "Failed to perform randrange: min (%d) > max (%d)\n", min, max);
	assertf(max <= RAND_MAX, "Failed to perform randrange: max (%d) > RAND_MAX (%d)\n", max, RAND_MAX);

	return (rand() % (max - min + 1)) + min; 
}
