#ifndef PONG_NATIVE_PONG_H
#define PONG_NATIVE_PONG_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Shared game state used by all frontends.
 * The NASM code relies on this exact field order and 32-bit integer layout.
 */
struct PongState {
    int screen_width;
    int screen_height;
    int paddle_width;
    int paddle_height;
    int ball_size;
    int player1_y;
    int player2_y;
    int ball_x;
    int ball_y;
    int ball_velocity_x;
    int ball_velocity_y;
    int player1_score;
    int player2_score;
    int running;
    int game_over;
    int winner;
};

void pong_init(struct PongState* state);
void pong_reset_round(struct PongState* state);
void pong_update_player1(struct PongState* state, int direction);
void pong_update_ai_player2(struct PongState* state);
void pong_step(struct PongState* state);

#ifdef __cplusplus
}
#endif

#endif
