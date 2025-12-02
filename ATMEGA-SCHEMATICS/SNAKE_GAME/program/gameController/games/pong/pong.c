#include "pong.h"
#include "uart.h"
#include "shared_memory.h"

#if (GAME == PONG)

void pongClear(void){
    bufferClear();
}


void pongInit(void){
    pong_state = &pong_sram_map.state;
    
    // Initialize game state
    pong_state->ball_x = SCREEN_WIDTH / 2;
    pong_state->ball_y = SCREEN_HEIGHT / 2;
    pong_state->ball_vx = 1;
    pong_state->ball_vy = 1;
    pong_state->left_paddle_y = SCREEN_HEIGHT / 2 - PADDLE_LENGTH / 2;
    pong_state->right_paddle_y = SCREEN_HEIGHT / 2 - PADDLE_LENGTH / 2;
    pong_state->paddle_length = PADDLE_LENGTH;
    pong_state->left_score = 0;
    pong_state->right_score = 0;
    pong_state->game_active = true;
    
    pongClear();
    pongRender();
    displayGrid();
}


void pongReset(void){
    pong_state->ball_x = SCREEN_WIDTH / 2;
    pong_state->ball_y = SCREEN_HEIGHT / 2;
    pong_state->ball_vx = (rand() % 2) ? 1 : -1;
    pong_state->ball_vy = (rand() % 2) ? 1 : -1;
    pong_state->left_paddle_y = SCREEN_HEIGHT / 2 - PADDLE_LENGTH / 2;
    pong_state->right_paddle_y = SCREEN_HEIGHT / 2 - PADDLE_LENGTH / 2;
}