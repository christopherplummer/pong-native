#include <SDL2/SDL.h>
#include <stdio.h>

#include "../common/pong.h"

enum {
    WINDOW_SCALE = 1,
    LEFT_PADDLE_X = 24,
    RIGHT_PADDLE_PADDING = 24,
    TARGET_FPS = 60,
    FRAME_DELAY_MS = 1000 / TARGET_FPS,
    RESTART_BUTTON_WIDTH = 180,
    RESTART_BUTTON_HEIGHT = 52
};

static void draw_digit(SDL_Renderer* renderer, int digit, int x, int y, int scale) {
    static const unsigned char digits[10][15] = {
        {1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1},
        {0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1},
        {1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1},
        {1, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1},
        {1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1},
        {1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1},
        {1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1},
        {1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0},
        {1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1},
        {1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1}
    };

    if (digit < 0 || digit > 9) {
        return;
    }

    for (int row = 0; row < 5; ++row) {
        for (int col = 0; col < 3; ++col) {
            if (!digits[digit][row * 3 + col]) {
                continue;
            }

            SDL_Rect rect = {
                x + (col * scale),
                y + (row * scale),
                scale,
                scale
            };
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

static void draw_score(SDL_Renderer* renderer, int score, int x, int y, int scale) {
    if (score < 0) {
        score = 0;
    }

    if (score < 10) {
        draw_digit(renderer, score, x, y, scale);
        return;
    }

    int tens = (score / 10) % 10;
    int ones = score % 10;
    draw_digit(renderer, tens, x, y, scale);
    draw_digit(renderer, ones, x + (4 * scale), y, scale);
}

static void draw_number(SDL_Renderer* renderer, int value, int x, int y, int scale) {
    if (value < 0) {
        value = 0;
    }

    if (value < 10) {
        draw_digit(renderer, value, x, y, scale);
        return;
    }

    int digits[10];
    int count = 0;
    while (value > 0 && count < 10) {
        digits[count++] = value % 10;
        value /= 10;
    }

    for (int i = count - 1; i >= 0; --i) {
        draw_digit(renderer, digits[i], x, y, scale);
        x += 4 * scale;
    }
}

static void draw_colon(SDL_Renderer* renderer, int x, int y, int scale) {
    SDL_Rect top = {x, y + scale, scale, scale};
    SDL_Rect bottom = {x, y + (3 * scale), scale, scale};
    SDL_RenderFillRect(renderer, &top);
    SDL_RenderFillRect(renderer, &bottom);
}

static void draw_letter(SDL_Renderer* renderer, char letter, int x, int y, int scale) {
    static const unsigned char glyph_a[15] = {
        0, 1, 0,
        1, 0, 1,
        1, 1, 1,
        1, 0, 1,
        1, 0, 1
    };
    static const unsigned char glyph_c[15] = {
        1, 1, 1,
        1, 0, 0,
        1, 0, 0,
        1, 0, 0,
        1, 1, 1
    };
    static const unsigned char glyph_e[15] = {
        1, 1, 1,
        1, 0, 0,
        1, 1, 0,
        1, 0, 0,
        1, 1, 1
    };
    static const unsigned char glyph_f[15] = {
        1, 1, 1,
        1, 0, 0,
        1, 1, 0,
        1, 0, 0,
        1, 0, 0
    };
    static const unsigned char glyph_i[15] = {
        1, 1, 1,
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,
        1, 1, 1
    };
    static const unsigned char glyph_l[15] = {
        1, 0, 0,
        1, 0, 0,
        1, 0, 0,
        1, 0, 0,
        1, 1, 1
    };
    static const unsigned char glyph_n[15] = {
        1, 0, 1,
        1, 1, 1,
        1, 1, 1,
        1, 0, 1,
        1, 0, 1
    };
    static const unsigned char glyph_o[15] = {
        1, 1, 1,
        1, 0, 1,
        1, 0, 1,
        1, 0, 1,
        1, 1, 1
    };
    static const unsigned char glyph_p[15] = {
        1, 1, 0,
        1, 0, 1,
        1, 1, 0,
        1, 0, 0,
        1, 0, 0
    };
    static const unsigned char glyph_r[15] = {
        1, 1, 0,
        1, 0, 1,
        1, 1, 0,
        1, 1, 0,
        1, 0, 1
    };
    static const unsigned char glyph_s[15] = {
        1, 1, 1,
        1, 0, 0,
        1, 1, 1,
        0, 0, 1,
        1, 1, 1
    };
    static const unsigned char glyph_t[15] = {
        1, 1, 1,
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,
        0, 1, 0
    };
    static const unsigned char glyph_w[15] = {
        1, 0, 1,
        1, 0, 1,
        1, 1, 1,
        1, 1, 1,
        1, 0, 1
    };
    static const unsigned char glyph_y[15] = {
        1, 0, 1,
        1, 0, 1,
        0, 1, 0,
        0, 1, 0,
        0, 1, 0
    };

    const unsigned char* glyph = NULL;

    switch (letter) {
        case 'A': glyph = glyph_a; break;
        case 'C': glyph = glyph_c; break;
        case 'E': glyph = glyph_e; break;
        case 'F': glyph = glyph_f; break;
        case 'I': glyph = glyph_i; break;
        case 'L': glyph = glyph_l; break;
        case 'N': glyph = glyph_n; break;
        case 'O': glyph = glyph_o; break;
        case 'P': glyph = glyph_p; break;
        case 'R': glyph = glyph_r; break;
        case 'S': glyph = glyph_s; break;
        case 'T': glyph = glyph_t; break;
        case 'W': glyph = glyph_w; break;
        case 'Y': glyph = glyph_y; break;
        default: break;
    }

    if (!glyph) {
        return;
    }

    for (int row = 0; row < 5; ++row) {
        for (int col = 0; col < 3; ++col) {
            if (!glyph[row * 3 + col]) {
                continue;
            }

            SDL_Rect rect = {
                x + (col * scale),
                y + (row * scale),
                scale,
                scale
            };
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

static void draw_word(SDL_Renderer* renderer, const char* word, int x, int y, int scale) {
    for (const char* cursor = word; *cursor; ++cursor) {
        if (*cursor == ' ') {
            x += 4 * scale;
            continue;
        }

        if (*cursor >= '0' && *cursor <= '9') {
            draw_digit(renderer, *cursor - '0', x, y, scale);
            x += 4 * scale;
            continue;
        }

        if (*cursor == ':') {
            draw_colon(renderer, x, y, scale);
            x += 2 * scale;
            continue;
        }

        draw_letter(renderer, *cursor, x, y, scale);
        x += 4 * scale;
    }
}

static void draw_center_divider(SDL_Renderer* renderer, int screen_width, int screen_height) {
    int x = screen_width / 2 - 2;
    for (int y = 0; y < screen_height; y += 24) {
        SDL_Rect dash = {x, y, 4, 14};
        SDL_RenderFillRect(renderer, &dash);
    }
}

static SDL_Rect restart_button_rect(const struct PongState* state) {
    SDL_Rect button = {
        (state->screen_width - RESTART_BUTTON_WIDTH) / 2,
        (state->screen_height / 2) + 70,
        RESTART_BUTTON_WIDTH,
        RESTART_BUTTON_HEIGHT
    };
    return button;
}

static int point_in_rect(int x, int y, const SDL_Rect* rect) {
    return x >= rect->x &&
           x < rect->x + rect->w &&
           y >= rect->y &&
           y < rect->y + rect->h;
}

static void reset_match(struct PongState* state) {
    pong_init(state);
}

static void draw_game_over_overlay(SDL_Renderer* renderer, const struct PongState* state) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 5, 5, 5, 200);
    SDL_Rect overlay = {0, 0, state->screen_width, state->screen_height};
    SDL_RenderFillRect(renderer, &overlay);

    SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
    if (state->winner == 1) {
        draw_word(renderer, "PLAYER 1 WINS", state->screen_width / 2 - 130, state->screen_height / 2 - 110, 8);
    } else {
        draw_word(renderer, "PLAYER 2 WINS", state->screen_width / 2 - 130, state->screen_height / 2 - 110, 8);
    }

    draw_word(renderer, "FINAL SCORE", state->screen_width / 2 - 95, state->screen_height / 2 - 35, 6);
    draw_number(renderer, state->player1_score, state->screen_width / 2 - 52, state->screen_height / 2 + 15, 8);
    draw_colon(renderer, state->screen_width / 2 - 4, state->screen_height / 2 + 15, 8);
    draw_number(renderer, state->player2_score, state->screen_width / 2 + 20, state->screen_height / 2 + 15, 8);

    SDL_Rect button = restart_button_rect(state);
    SDL_SetRenderDrawColor(renderer, 30, 110, 70, 255);
    SDL_RenderFillRect(renderer, &button);
    SDL_SetRenderDrawColor(renderer, 230, 230, 230, 255);
    SDL_RenderDrawRect(renderer, &button);
    draw_word(renderer, "RESTART", button.x + 28, button.y + 12, 6);
}

int main(void) {
    struct PongState state;
    pong_init(&state);
    int app_running = 1;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Pong Native",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        state.screen_width * WINDOW_SCALE,
        state.screen_height * WINDOW_SCALE,
        SDL_WINDOW_SHOWN
    );
    if (!window) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!renderer) {
        fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    Uint32 previous_tick = SDL_GetTicks();

    while (app_running) {
        Uint32 frame_start = SDL_GetTicks();
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                app_running = 0;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                app_running = 0;
            }
            if (state.game_over && event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r) {
                reset_match(&state);
            }
            if (state.game_over && event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                SDL_Rect button = restart_button_rect(&state);
                if (point_in_rect(event.button.x, event.button.y, &button)) {
                    reset_match(&state);
                }
            }
        }

        if (!state.game_over) {
            const Uint8* keyboard = SDL_GetKeyboardState(NULL);
            int direction = 0;
            if (keyboard[SDL_SCANCODE_W] || keyboard[SDL_SCANCODE_UP]) {
                direction = -1;
            } else if (keyboard[SDL_SCANCODE_S] || keyboard[SDL_SCANCODE_DOWN]) {
                direction = 1;
            }

            pong_update_player1(&state, direction);
            pong_update_ai_player2(&state);

            Uint32 now = SDL_GetTicks();
            if (now - previous_tick >= FRAME_DELAY_MS) {
                previous_tick = now;
                pong_step(&state);
            }
        }

        SDL_SetRenderDrawColor(renderer, 12, 12, 12, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 235, 235, 235, 255);
        draw_center_divider(renderer, state.screen_width, state.screen_height);

        SDL_Rect player1 = {
            LEFT_PADDLE_X,
            state.player1_y,
            state.paddle_width,
            state.paddle_height
        };
        SDL_RenderFillRect(renderer, &player1);

        int right_paddle_x = state.screen_width - RIGHT_PADDLE_PADDING - state.paddle_width;
        SDL_Rect player2 = {
            right_paddle_x,
            state.player2_y,
            state.paddle_width,
            state.paddle_height
        };
        SDL_RenderFillRect(renderer, &player2);

        SDL_Rect ball = {
            state.ball_x,
            state.ball_y,
            state.ball_size,
            state.ball_size
        };
        SDL_RenderFillRect(renderer, &ball);

        draw_score(renderer, state.player1_score, state.screen_width / 2 - 80, 32, 8);
        draw_score(renderer, state.player2_score, state.screen_width / 2 + 40, 32, 8);

        if (state.game_over) {
            draw_game_over_overlay(renderer, &state);
        }

        SDL_RenderPresent(renderer);

        Uint32 frame_time = SDL_GetTicks() - frame_start;
        if (frame_time < FRAME_DELAY_MS) {
            SDL_Delay(FRAME_DELAY_MS - frame_time);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
