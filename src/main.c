#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <util/assertf.h>
#include <util/colors.h>
#include <util/types.h>

#include <SDL2/SDL.h>

#define ALLOC(_siz) calloc(1, (_siz))
#define ARR_LEN(_arr) sizeof(_arr)/sizeof(*(_arr))

#define DIR_UP    0
#define DIR_DOWN  1
#define DIR_LEFT  2
#define DIR_RIGHT 3

#pragma region Platform specific stuff
#if defined(_WIN32)
#include <windows.h>
#define MSLEEP(_ms) Sleep(_ms)
typedef SSIZE_T ssize_t;
#elif defined(__unix__)
#define __USE_MISC 1
#include <unistd.h>
#define MSLEEP(_ms) usleep((_ms) * 1000)
#else
#error unsupported OS
#endif
#pragma endregion

typedef struct {SDL_Rect rect; u8 dir;} PlayerSegment_s;

u32 init_rand(void);
void init_window(void);
void init_player(void);
void init_food(void);

void window_event(void);
void render(void);
void keyboard(void);
void update(void);

bool collide_rect(const SDL_Rect *r1, const SDL_Rect *r2);
i32 rand_range(i32 min, i32 max);

#pragma region Config constansts
// Flags for initialization
const u32			init_flags		    = SDL_INIT_VIDEO | SDL_INIT_AUDIO; 
const u32			wn_flags            = SDL_WINDOW_SHOWN;
const u32           rn_flags		    = 0;

// Appearance
const u32  			grid_size    	    = 20;
const char 			wn_title[]          = "snake";
const v2  			wn_size      	    = {800, 600};
const v2  			wn_pos    		    = {SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED};
const SDL_Color 	snake_color1 	    = {0x00, 0x7F, 0x00, 0xFF};
const SDL_Color		snake_color2	    = {0x00, 0xCF, 0x00, 0xFF};
const SDL_Color		food_color		    = {0xBA, 0x00, 0x00, 0xFF};
const SDL_Color		bg_color		    = {0x00, 0x00, 0x00, 0xFF};

// [0] == top border
// [1] == bottom border
// [2] == left border
// [3] == right border
const SDL_Color     border_colors[4]    = {
	{COLOR_GREY}, {COLOR_GREY}, {COLOR_GREY}, {COLOR_GREY}};
#pragma endregion

struct {
	bool should_close;

	clock_t delta_time;
	SDL_Window *wn;
	SDL_Renderer *rn;

	SDL_Rect food;

	struct {
		size_t len;
		PlayerSegment_s *body;
	} player;
} state = {0};

int main(int argc, char *argv[]) {
	// make windows use utf-8 encoding
#if defined(_WIN32)
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
#endif

	init_rand();
	init_window();
	init_player();
	init_food();

	u8 player_cooldown = 10; // 10 frame cooldown

	while (!state.should_close) {
		window_event();

		if (player_cooldown == 0) {
			keyboard();
			update();
			player_cooldown = 10;
		} else {
			--player_cooldown;
		}

		render();
		MSLEEP(10); // run at 100 fps
	}
	
	return 0;
}

u32 init_rand(void) {
	// put the seed into a variable so that it can be returned
	unsigned int seed = (unsigned int)(time(NULL) ^ clock());

	srand(seed);

	return seed;
}

void init_window(void) {
	ASSERTF(SDL_Init(init_flags) == 0,
		    "Failed to init SDL2: %s\n", SDL_GetError());

	state.wn = SDL_CreateWindow(wn_title, 
								wn_pos.x, 
								wn_pos.y, 
								wn_size.x, 
								wn_size.y,
								wn_flags
							   );

	ASSERTF(state.wn, "Failed to create window: %s\n", SDL_GetError());

	state.rn = SDL_CreateRenderer(state.wn, -1, rn_flags);
	ASSERTF(state.rn, "Failed to create renderer: %s\n", SDL_GetError());
	SDL_SetRenderDrawBlendMode(state.rn, SDL_BLENDMODE_BLEND);
}

void init_player(void) {
	state.player.len = 1;
	state.player.body = realloc(state.player.body, sizeof(*state.player.body));
	
	ASSERTF(state.player.body, "Failed to init player\n");

	state.player.body[0].rect = (SDL_Rect){wn_size.x/2, wn_size.y/2, grid_size/2, grid_size/2};
}

void init_food(void) {
	state.food = (SDL_Rect){
		rand_range(1, wn_size.x/grid_size - 1) * grid_size,
		rand_range(1, wn_size.x/grid_size - 1) * grid_size,
		grid_size/2,
		grid_size/2
	};
}

void window_event(void) {
	for (SDL_Event ev; SDL_PollEvent(&ev);) {
		switch (ev.type) {
			case SDL_QUIT:
				state.should_close = true;
				break;

			default:
				break;
		}

		switch (ev.window.event) {
			// I don't know why but kde (plasma) let me maximize the window
			// so I'm putting this here just in case
			case SDL_WINDOWEVENT_MAXIMIZED:
				SDL_HideWindow(state.wn);
				SDL_SetWindowSize(state.wn, wn_size.x, wn_size.y);
				SDL_ShowWindow(state.wn);
				break;

			default:
				break;
		}
	}
}

void render(void) {
	SDL_SetRenderDrawColor(state.rn, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
	SDL_RenderClear(state.rn);

	for (size_t i = 0; i < state.player.len; ++i) {
		SDL_Rect r = state.player.body[i].rect;
		
		if (i > 0) {
			switch (state.player.body[i].dir) {
				case DIR_UP:
					r.h = grid_size;
					r.y -= grid_size/4;
					break;

				case DIR_DOWN:
					r.h = grid_size;
					r.y += grid_size/4;
					break;
				
				case DIR_LEFT:
					r.w = grid_size;
					r.x -= grid_size/4;
					break;

				case DIR_RIGHT:
					r.w = grid_size;
					r.x += grid_size/4;
					break;

				default:
					break;
			}
		}

		r.x -= r.w/2; r.y -= r.h/2;

		SDL_Color c = (i % 2) ? snake_color1 : snake_color2;

		SDL_SetRenderDrawColor(state.rn, c.r, c.g, c.b, c.a);
		SDL_RenderDrawRect(state.rn, &r);
		SDL_RenderFillRect(state.rn, &r);
	}

	for (size_t i = 0; i < ARR_LEN(border_colors); ++i) {
		SDL_Color c = border_colors[i];
		SDL_SetRenderDrawColor(state.rn, c.r, c.g, c.b, c.a);

		SDL_Rect r = {0};

		switch (i) {
			case DIR_UP:
				r = (SDL_Rect){0, 0, wn_size.x, grid_size/2};
				break;

			case DIR_DOWN:
				r = (SDL_Rect){0, wn_size.y - grid_size/2, wn_size.x, grid_size/2};
				break;

			case DIR_LEFT:
				r = (SDL_Rect){0, 0, grid_size/2, wn_size.y};
				break;

			case DIR_RIGHT:
				r = (SDL_Rect){wn_size.x - grid_size/2, 0, grid_size/2, wn_size.y};
				break;

			default:
				break;
		}

		SDL_RenderDrawRect(state.rn, &r);
		SDL_RenderFillRect(state.rn, &r);
	}

	{SDL_Rect r = state.food;
	r.x -= r.w/2; r.y -= r.h/2;
	SDL_SetRenderDrawColor(state.rn, food_color.r, food_color.g, food_color.b, food_color.a);
	SDL_RenderDrawRect(state.rn, &r);
	SDL_RenderFillRect(state.rn, &r);}

	SDL_RenderPresent(state.rn);
}

void keyboard(void) {
	SDL_PumpEvents();
	
	int len_keys = 0;
	const u8 *keys = SDL_GetKeyboardState(&len_keys);

	for (size_t i = 0; i < len_keys; ++i) {
		if (!keys[i]) {continue;}

		const SDL_Keycode key = SDL_GetKeyFromScancode(i);

		u8 *dir = &state.player.body[0].dir;
		switch (key) {
			case SDLK_w:
				if (*dir == DIR_DOWN) {break;}

				*dir = DIR_UP;
				break;

			case SDLK_s:
				if (*dir == DIR_UP) {break;}

				*dir = DIR_DOWN;
				break;

			case SDLK_a:
				if (*dir == DIR_RIGHT) {break;}

				*dir = DIR_LEFT;
				break;

			case SDLK_d:
				if (*dir == DIR_LEFT) {break;}

				*dir = DIR_RIGHT;
				break;

			default:
				break;
		}
	}
}

void update(void) {
	if (state.food.x >= wn_size.x || state.food.x <= 0 || \
	    state.food.y >= wn_size.y || state.food.y <= 0) {

			init_food();
	}

	const PlayerSegment_s *player_head = &state.player.body[0];

	for (ssize_t i = state.player.len - 1; i >= 1; --i) {
		state.player.body[i] = state.player.body[i - 1];
	}

	switch (state.player.body[0].dir) {
		case DIR_UP:
			state.player.body[0].rect.y -= grid_size;
			break;

		case DIR_DOWN:
			state.player.body[0].rect.y += grid_size;
			break;
		
		case DIR_LEFT:
			state.player.body[0].rect.x -= grid_size;
			break;

		case DIR_RIGHT:
			state.player.body[0].rect.x += grid_size;
			break;

		default:
			break;
	}

	for (size_t i = 1; i < state.player.len; ++i) {
		if (collide_rect(&player_head->rect, &state.player.body[i].rect)) {

			init_player();
		}
	}

	if (player_head->rect.x >= wn_size.x || player_head->rect.x <= 0 || \
	    player_head->rect.y >= wn_size.y || player_head->rect.y <= 0) {

			init_player();
		}

	if (collide_rect(&player_head->rect, &state.food)) {
		++state.player.len;
		state.player.body = realloc(state.player.body, sizeof(*state.player.body) * state.player.len);
		memset(&state.player.body[state.player.len - 1], 0, sizeof(*state.player.body));

		init_food();
	}
}

bool collide_rect(const SDL_Rect *r1, const SDL_Rect *r2) {
	return \
		(r1->x + r1->w >= r2->x) && \
		(r1->x <= r2->x + r2->w) && \
		(r1->y + r1->h >= r2->y) && \
		(r1->y <= r2->y + r2->h);
}

i32 rand_range(i32 min, i32 max) {
	if(min > max) { 
		fprintf(stderr, 
				"Failed to operate function '%s': min (%"PRId32") > max (%"PRId32")", 
		        __func__, min, max
		       );

		return -RAND_MAX;
	}

	if (max > RAND_MAX) {
		fprintf(stderr,
				"Failed to operate function '%s': max (%"PRId32") > RAND_MAX (%d)", 
				__func__, max, RAND_MAX
				);

		return -RAND_MAX;
	}

	if (min < -RAND_MAX) {
	    fprintf(stderr,
				"Failed to operate function '%s': min (%"PRId32") < -RAND_MAX (%d)", 
				__func__, min, -RAND_MAX
		       );

		return -RAND_MAX;
	}

	if (min < 0) {
		max -= min;
		return rand() % (max + 1) + min;
	}

	return rand() % (max + 1 - min) + min;
}
