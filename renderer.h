#ifndef RENDERER_H
#define RENDERER_H

#include "terminal.h"
#include "entities.h"
#include "game_state.h"
#include "enemy_ai.h"
#include "bonus_stage.h"

void renderer_draw_player(TerminalBuffer *buf, Player *player);
void renderer_draw_enemy(TerminalBuffer *buf, Enemy *enemy);
void renderer_draw_bullet(TerminalBuffer *buf, Bullet *bullet);
void renderer_draw_powerup(TerminalBuffer *buf, PowerUp *powerup);
void renderer_draw_stars(TerminalBuffer *buf, Star stars[], int count);
void renderer_draw_hud(TerminalBuffer *buf, Player *player, GameState *state);
void renderer_draw_game_over(TerminalBuffer *buf, GameState *state, int screen_width, int screen_height);
void renderer_draw_wave_transition(TerminalBuffer *buf, GameState *state, int screen_width, int screen_height);
void renderer_draw_menu(TerminalBuffer *buf, int screen_width, int screen_height);
void renderer_draw_bonus_stage_hud(TerminalBuffer *buf, BonusStage *bonus, GameState *state);

#endif
