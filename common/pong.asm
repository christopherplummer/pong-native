; pong.asm
; Shared x86_64 NASM game logic for Pong.
;
; ABI notes:
; - System V AMD64 ABI is used on Linux and other Unix-like x86_64 targets.
; - rdi = PongState* for single-pointer functions
; - rsi = int direction for pong_update_player1
; - The struct layout must match common/pong.h exactly: 16 contiguous 32-bit ints.

default rel

global pong_init
global pong_reset_round
global pong_update_player1
global pong_update_ai_player2
global pong_step

section .text

%define SCREEN_WIDTH      0
%define SCREEN_HEIGHT     4
%define PADDLE_WIDTH      8
%define PADDLE_HEIGHT     12
%define BALL_SIZE         16
%define PLAYER1_Y         20
%define PLAYER2_Y         24
%define BALL_X            28
%define BALL_Y            32
%define BALL_VEL_X        36
%define BALL_VEL_Y        40
%define PLAYER1_SCORE     44
%define PLAYER2_SCORE     48
%define RUNNING           52
%define GAME_OVER         56
%define WINNER            60

%define DEFAULT_SCREEN_W  800
%define DEFAULT_SCREEN_H  600
%define DEFAULT_PADDLE_W  12
%define DEFAULT_PADDLE_H  96
%define DEFAULT_BALL_SIZE 12
%define PADDLE_STEP       8
%define AI_STEP           5
%define BALL_SPEED_X      5
%define BALL_SPEED_Y      4
%define LEFT_PADDLE_X     24
%define RIGHT_PADDLE_PAD  24
%define WINNING_SCORE     10

pong_init:
    mov dword [rdi + SCREEN_WIDTH], DEFAULT_SCREEN_W
    mov dword [rdi + SCREEN_HEIGHT], DEFAULT_SCREEN_H
    mov dword [rdi + PADDLE_WIDTH], DEFAULT_PADDLE_W
    mov dword [rdi + PADDLE_HEIGHT], DEFAULT_PADDLE_H
    mov dword [rdi + BALL_SIZE], DEFAULT_BALL_SIZE
    mov dword [rdi + PLAYER1_SCORE], 0
    mov dword [rdi + PLAYER2_SCORE], 0
    mov dword [rdi + RUNNING], 1
    mov dword [rdi + GAME_OVER], 0
    mov dword [rdi + WINNER], 0
    jmp pong_reset_round

pong_reset_round:
    ; Center paddles vertically.
    mov eax, [rdi + SCREEN_HEIGHT]
    sub eax, [rdi + PADDLE_HEIGHT]
    sar eax, 1
    mov [rdi + PLAYER1_Y], eax
    mov [rdi + PLAYER2_Y], eax

    ; Center the ball.
    mov eax, [rdi + SCREEN_WIDTH]
    sub eax, [rdi + BALL_SIZE]
    sar eax, 1
    mov [rdi + BALL_X], eax

    mov eax, [rdi + SCREEN_HEIGHT]
    sub eax, [rdi + BALL_SIZE]
    sar eax, 1
    mov [rdi + BALL_Y], eax

    ; Always restart moving toward player 2 with a mild downward bias.
    mov dword [rdi + BALL_VEL_X], BALL_SPEED_X
    mov dword [rdi + BALL_VEL_Y], BALL_SPEED_Y
    ret

pong_update_player1:
    ; direction: -1 up, 0 none, 1 down
    cmp esi, 0
    je .done

    mov eax, [rdi + PLAYER1_Y]
    cmp esi, 0
    jl .move_up

    add eax, PADDLE_STEP
    jmp .clamp

.move_up:
    sub eax, PADDLE_STEP

.clamp:
    cmp eax, 0
    jge .check_bottom
    xor eax, eax
    jmp .store

.check_bottom:
    mov edx, [rdi + SCREEN_HEIGHT]
    sub edx, [rdi + PADDLE_HEIGHT]
    cmp eax, edx
    jle .store
    mov eax, edx

.store:
    mov [rdi + PLAYER1_Y], eax

.done:
    ret

pong_update_ai_player2:
    ; AI follows the ball center with a small dead zone and slower step.
    mov eax, [rdi + PLAYER2_Y]
    mov ecx, [rdi + PADDLE_HEIGHT]
    sar ecx, 1
    add eax, ecx                  ; eax = paddle center

    mov edx, [rdi + BALL_Y]
    mov ecx, [rdi + BALL_SIZE]
    sar ecx, 1
    add edx, ecx                  ; edx = ball center

    cmp edx, eax
    jg .ball_below

    mov ecx, eax
    sub ecx, edx
    cmp ecx, 6
    jle .done

    mov eax, [rdi + PLAYER2_Y]
    sub eax, AI_STEP
    cmp eax, 0
    jge .store_ai
    xor eax, eax
    jmp .store_ai

.ball_below:
    mov ecx, edx
    sub ecx, eax
    cmp ecx, 6
    jle .done

    mov eax, [rdi + PLAYER2_Y]
    add eax, AI_STEP
    mov edx, [rdi + SCREEN_HEIGHT]
    sub edx, [rdi + PADDLE_HEIGHT]
    cmp eax, edx
    jle .store_ai
    mov eax, edx

.store_ai:
    mov [rdi + PLAYER2_Y], eax

.done:
    ret

pong_step:
    cmp dword [rdi + GAME_OVER], 0
    jne .done

    ; Advance ball.
    mov eax, [rdi + BALL_X]
    add eax, [rdi + BALL_VEL_X]
    mov [rdi + BALL_X], eax

    mov eax, [rdi + BALL_Y]
    add eax, [rdi + BALL_VEL_Y]
    mov [rdi + BALL_Y], eax

    ; Bounce off top wall.
    mov eax, [rdi + BALL_Y]
    cmp eax, 0
    jge .check_bottom_wall
    mov dword [rdi + BALL_Y], 0
    mov eax, [rdi + BALL_VEL_Y]
    neg eax
    mov [rdi + BALL_VEL_Y], eax

.check_bottom_wall:
    mov eax, [rdi + BALL_Y]
    mov ecx, [rdi + SCREEN_HEIGHT]
    sub ecx, [rdi + BALL_SIZE]
    cmp eax, ecx
    jle .check_left_score
    mov [rdi + BALL_Y], ecx
    mov eax, [rdi + BALL_VEL_Y]
    neg eax
    mov [rdi + BALL_VEL_Y], eax

.check_left_score:
    mov eax, [rdi + BALL_X]
    mov ecx, [rdi + BALL_SIZE]
    neg ecx
    cmp eax, ecx
    jg .check_right_score
    add dword [rdi + PLAYER2_SCORE], 1
    cmp dword [rdi + PLAYER2_SCORE], WINNING_SCORE
    jl .reset_after_p2_score
    mov dword [rdi + GAME_OVER], 1
    mov dword [rdi + WINNER], 2
    jmp .done

.reset_after_p2_score:
    jmp pong_reset_round

.check_right_score:
    mov eax, [rdi + BALL_X]
    mov ecx, [rdi + SCREEN_WIDTH]
    cmp eax, ecx
    jl .check_left_paddle
    add dword [rdi + PLAYER1_SCORE], 1
    cmp dword [rdi + PLAYER1_SCORE], WINNING_SCORE
    jl .reset_after_p1_score
    mov dword [rdi + GAME_OVER], 1
    mov dword [rdi + WINNER], 1
    jmp .done

.reset_after_p1_score:
    jmp pong_reset_round

.check_left_paddle:
    ; Left paddle occupies x in [LEFT_PADDLE_X, LEFT_PADDLE_X + paddle_width]
    mov eax, [rdi + BALL_X]
    cmp eax, LEFT_PADDLE_X
    jl .check_right_paddle

    mov ecx, LEFT_PADDLE_X
    add ecx, [rdi + PADDLE_WIDTH]
    cmp eax, ecx
    jg .check_right_paddle

    mov edx, [rdi + BALL_Y]
    mov eax, [rdi + PLAYER1_Y]
    cmp edx, eax
    jl .check_right_paddle

    mov ecx, eax
    add ecx, [rdi + PADDLE_HEIGHT]
    cmp edx, ecx
    jg .check_right_paddle

    mov eax, [rdi + BALL_VEL_X]
    cmp eax, 0
    jle .bounce_left_paddle
    jmp .check_right_paddle

.bounce_left_paddle:
    neg eax
    mov [rdi + BALL_VEL_X], eax
    mov eax, LEFT_PADDLE_X
    add eax, [rdi + PADDLE_WIDTH]
    mov [rdi + BALL_X], eax
    jmp .done

.check_right_paddle:
    ; Right paddle x = screen_width - RIGHT_PADDLE_PAD - paddle_width
    mov ecx, [rdi + SCREEN_WIDTH]
    sub ecx, RIGHT_PADDLE_PAD
    sub ecx, [rdi + PADDLE_WIDTH] ; ecx = right paddle x

    mov eax, [rdi + BALL_X]
    mov edx, [rdi + BALL_SIZE]
    add edx, eax                  ; edx = ball right edge
    cmp edx, ecx
    jl .done

    mov eax, ecx
    add eax, [rdi + PADDLE_WIDTH]
    cmp edx, eax
    jg .done

    mov edx, [rdi + BALL_Y]
    mov eax, [rdi + PLAYER2_Y]
    cmp edx, eax
    jl .done

    mov esi, eax
    add esi, [rdi + PADDLE_HEIGHT]
    cmp edx, esi
    jg .done

    mov eax, [rdi + BALL_VEL_X]
    cmp eax, 0
    jge .bounce_right_paddle
    jmp .done

.bounce_right_paddle:
    neg eax
    mov [rdi + BALL_VEL_X], eax
    mov eax, ecx
    sub eax, [rdi + BALL_SIZE]
    mov [rdi + BALL_X], eax

.done:
    ret

section .note.GNU-stack noalloc noexec nowrite progbits
