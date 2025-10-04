#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "entities.h"
#include "enemy_ai.h"
#include <stdbool.h>

typedef enum
{
    GAME_STATE_MENU,
    GAME_STATE_PLAYING,
    GAME_STATE_BONUS_STAGE,
    GAME_STATE_WAVE_TRANSITION,
    GAME_STATE_GAME_OVER,
    GAME_STATE_PAUSED
} GameStateType;

typedef struct
{
    GameStateType state;
    int current_wave;
    int score;
    bool wave_complete;
    float wave_transition_timer;
    float game_over_timer;
    int enemies_killed_this_wave;
    bool perfect_wave;
} GameState;

void game_state_init(GameState *state);
void game_state_update(GameState *state, float dt);
void game_state_start_wave(GameState *state, EnemyFormation *formation, int screen_width);
void game_state_complete_wave(GameState *state, Player *player);
void game_state_add_score(GameState *state, int points);
void game_state_player_died(GameState *state);
bool game_state_should_spawn_bonus_stage(GameState *state);
bool game_state_is_game_over(GameState *state, Player *player);

#endif
