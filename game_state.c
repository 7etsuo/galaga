#include "game_state.h"

#define WAVE_TRANSITION_TIME 3.0f
#define GAME_OVER_TIME 3.0f

void game_state_init(GameState *state)
{
    state->state = GAME_STATE_MENU;
    state->current_wave = 0;
    state->score = 0;
    state->wave_complete = false;
    state->wave_transition_timer = 0.0f;
    state->game_over_timer = 0.0f;
    state->enemies_killed_this_wave = 0;
    state->perfect_wave = true;
}

void game_state_update(GameState *state, float dt)
{
    switch (state->state)
    {
    case GAME_STATE_WAVE_TRANSITION:
        state->wave_transition_timer -= dt;
        if (state->wave_transition_timer <= 0.0f)
        {
            state->state = GAME_STATE_PLAYING;
        }
        break;

    case GAME_STATE_GAME_OVER:
        state->game_over_timer -= dt;
        break;

    default:
        break;
    }
}

void game_state_start_wave(GameState *state, EnemyFormation *formation, int screen_width)
{
    state->current_wave++;
    state->wave_complete = false;
    state->enemies_killed_this_wave = 0;
    state->perfect_wave = true;

    if (game_state_should_spawn_bonus_stage(state))
    {
        state->state = GAME_STATE_BONUS_STAGE;
    }
    else
    {
        enemy_ai_init_formation(formation, screen_width, state->current_wave);
        state->state = GAME_STATE_WAVE_TRANSITION;
        state->wave_transition_timer = WAVE_TRANSITION_TIME;
    }
}

void game_state_complete_wave(GameState *state, Player *player)
{
    state->wave_complete = true;

    int wave_bonus = 1000 * state->current_wave;
    game_state_add_score(state, wave_bonus);

    if (state->perfect_wave)
    {
        game_state_add_score(state, wave_bonus);
    }

    (void)player;
}

void game_state_add_score(GameState *state, int points)
{
    state->score += points;
}

void game_state_player_died(GameState *state)
{
    state->perfect_wave = false;
}

bool game_state_should_spawn_bonus_stage(GameState *state)
{
    return (state->current_wave > 0 && state->current_wave % 3 == 0);
}

bool game_state_is_game_over(GameState *state, Player *player)
{
    if (player->lives <= 0 && state->state != GAME_STATE_GAME_OVER)
    {
        state->state = GAME_STATE_GAME_OVER;
        state->game_over_timer = GAME_OVER_TIME;
        return true;
    }
    return state->state == GAME_STATE_GAME_OVER && state->game_over_timer <= 0.0f;
}
