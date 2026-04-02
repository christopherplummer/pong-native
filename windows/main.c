#include <stdio.h>

#include "../common/pong.h"

int main(void) {
    struct PongState state;
    pong_init(&state);

    /*
     * Scaffold only.
     *
     * A future Windows implementation can create a native Win32 window,
     * gather keyboard input, call the shared assembly logic, and render
     * the resulting state each frame.
     *
     * Likely future options:
     * - Win32 + software rendering
     * - SDL2 on Windows
     *
     * Expected usage pattern:
     * - pong_update_player1(&state, direction);
     * - pong_update_ai_player2(&state);
     * - pong_step(&state);
     * - draw paddles, ball, and scores from state
     */

    printf("Windows scaffold for pong-native.\n");
    printf("Shared core initialized: %dx%d, score %d-%d\n",
           state.screen_width,
           state.screen_height,
           state.player1_score,
           state.player2_score);
    return 0;
}
