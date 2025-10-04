#include "renderer.h"
#include <stdio.h>
#include <string.h>

void renderer_draw_player(TerminalBuffer *buf, Player *player)
{
    if (player->captured)
        return;

    /* Flash effect during invincibility - skip drawing on odd frames */
    if (player->invincibility_timer > 0.0f && !player->god_mode)
    {
        if (((int)(player->invincibility_timer * 10)) % 2 != 0)
            return; /* Don't draw on odd frames to create flashing */
    }

    int x = (int)player->x;
    int y = (int)player->y;

    /* Determine color based on player state */
    uint8_t color = COLOR_CYAN;
    if (player->god_mode)
        color = COLOR_RED;
    else if (player->has_reflect_shield)
        color = COLOR_BLUE;
    else if (player->has_shield)
        color = COLOR_YELLOW;
    else if (player->has_mega_laser)
        color = COLOR_PURPLE;

    /* Draw player sprite */
    terminal_buffer_set_char(buf, x, y, '^', color);
    terminal_buffer_set_char(buf, x - 1, y + 1, '<', color);
    terminal_buffer_set_char(buf, x, y + 1, '|', color);
    terminal_buffer_set_char(buf, x + 1, y + 1, '>', color);

    /* Draw dual fighter if active */
    if (player->dual_fighter)
    {
        terminal_buffer_set_char(buf, x + 2, y, '^', color);
        terminal_buffer_set_char(buf, x + 1, y + 1, '<', color);
        terminal_buffer_set_char(buf, x + 2, y + 1, '|', color);
        terminal_buffer_set_char(buf, x + 3, y + 1, '>', color);
    }

    /* Draw ally drone if active */
    if (player->has_ally_drone)
    {
        int dx = (int)player->ally_drone_x;
        int dy = (int)player->ally_drone_y;
        terminal_buffer_set_char(buf, dx, dy, 'v', COLOR_MAGENTA);
    }
}

void renderer_draw_enemy(TerminalBuffer *buf, Enemy *enemy)
{
    if (!enemy->active)
    {
        return;
    }

    int x = (int)enemy->x;
    int y = (int)enemy->y;

    uint8_t color = COLOR_WHITE;
    char sprite[4] = "???";

    switch (enemy->type)
    {
    case ENEMY_BOSS:
        color = COLOR_RED;
        if (enemy->animation_frame == 0)
        {
            strcpy(sprite, "<O>");
        }
        else
        {
            strcpy(sprite, ">O<");
        }
        break;
    case ENEMY_BUTTERFLY:
        color = COLOR_MAGENTA;
        if (enemy->animation_frame == 0)
        {
            strcpy(sprite, "/X\\");
        }
        else
        {
            strcpy(sprite, "\\X/");
        }
        break;
    case ENEMY_BEE:
        color = COLOR_GREEN;
        if (enemy->animation_frame == 0)
        {
            strcpy(sprite, "-M-");
        }
        else
        {
            strcpy(sprite, "~M~");
        }
        break;
    }

    terminal_buffer_set_char(buf, x - 1, y, sprite[0], color);
    terminal_buffer_set_char(buf, x, y, sprite[1], color);
    terminal_buffer_set_char(buf, x + 1, y, sprite[2], color);
}

void renderer_draw_bullet(TerminalBuffer *buf, Bullet *bullet)
{
    if (!bullet->active)
    {
        return;
    }

    int x = (int)bullet->x;
    int y = (int)bullet->y;

    if (bullet->is_player_bullet)
    {
        char ch = '|';
        uint8_t color = COLOR_YELLOW;

        switch (bullet->type)
        {
        case BULLET_MEGA_LASER:
            ch = '#';
            color = COLOR_PURPLE;
            break;
        case BULLET_HOMING:
            ch = 'o';
            color = COLOR_GREEN;
            break;
        case BULLET_LIGHTNING:
            ch = '~';
            color = COLOR_WHITE;
            break;
        default:
            break;
        }

        terminal_buffer_set_char(buf, x, y, ch, color);
    }
    else
    {
        terminal_buffer_set_char(buf, x, y, '*', COLOR_WHITE);
    }
}

void renderer_draw_powerup(TerminalBuffer *buf, PowerUp *powerup)
{
    if (!powerup->active)
    {
        return;
    }

    int x = (int)powerup->x;
    int y = (int)powerup->y;

    char icon;
    uint8_t color;

    switch (powerup->type)
    {
    case POWERUP_DUAL_SHOT:
        icon = 'D';
        color = COLOR_CYAN;
        break;
    case POWERUP_SHIELD:
        icon = 'S';
        color = COLOR_YELLOW;
        break;
    case POWERUP_SPEED:
        icon = 'F';
        color = COLOR_ORANGE;
        break;
    case POWERUP_MEGA_LASER:
        icon = 'M';
        color = COLOR_PURPLE;
        break;
    case POWERUP_BOMB:
        icon = 'B';
        color = COLOR_RED;
        break;
    case POWERUP_HOMING:
        icon = 'H';
        color = COLOR_GREEN;
        break;
    case POWERUP_LIGHTNING:
        icon = 'L';
        color = COLOR_WHITE;
        break;
    case POWERUP_REFLECT_SHIELD:
        icon = 'R';
        color = COLOR_BLUE;
        break;
    case POWERUP_TIME_SLOW:
        icon = 'T';
        color = COLOR_GRAY;
        break;
    case POWERUP_ALLY_DRONE:
        icon = 'A';
        color = COLOR_MAGENTA;
        break;
    default:
        icon = '?';
        color = COLOR_WHITE;
        break;
    }

    terminal_buffer_set_char(buf, x, y, icon, color);
}

void renderer_draw_stars(TerminalBuffer *buf, Star stars[], int count)
{
    for (int i = 0; i < count; i++)
    {
        terminal_buffer_set_char(buf, stars[i].x, stars[i].y, stars[i].character, COLOR_GRAY);
    }
}

void renderer_draw_hud(TerminalBuffer *buf, Player *player, GameState *state)
{
    char text[64];

    /* Top left: Lives and HP */
    snprintf(text, sizeof(text), "LIVES: %d  HP: %d/%d", player->lives, player->health, player->max_health);
    terminal_buffer_set_string(buf, 2, 0, text, COLOR_WHITE);

    /* Score with multiplier */
    if (player->score_multiplier > 1)
    {
        snprintf(text, sizeof(text), "SCORE: %d [x%d]", state->score, player->score_multiplier);
        terminal_buffer_set_string(buf, 25, 0, text, COLOR_YELLOW);
    }
    else
    {
        snprintf(text, sizeof(text), "SCORE: %d", state->score);
        terminal_buffer_set_string(buf, 30, 0, text, COLOR_WHITE);
    }

    /* Special weapon charge bar */
    int bar_x = 50;
    terminal_buffer_set_string(buf, bar_x, 0, "SPECIAL[", COLOR_GRAY);
    int charge_bars = (int)(player->special_charge / 10.0f);
    for (int i = 0; i < 10; i++)
    {
        if (i < charge_bars)
            terminal_buffer_set_char(buf, bar_x + 8 + i, 0, '=', player->special_ready ? COLOR_GREEN : COLOR_CYAN);
        else
            terminal_buffer_set_char(buf, bar_x + 8 + i, 0, '-', COLOR_GRAY);
    }
    terminal_buffer_set_char(buf, bar_x + 18, 0, ']', COLOR_GRAY);

    /* Top right: Wave and bombs */
    snprintf(text, sizeof(text), "WAVE: %d", state->current_wave);
    terminal_buffer_set_string(buf, buf->width - 15, 0, text, COLOR_WHITE);

    if (player->bomb_count > 0)
    {
        snprintf(text, sizeof(text), "BOMBS: %d", player->bomb_count);
        terminal_buffer_set_string(buf, buf->width - 15, 1, text, COLOR_RED);
    }

    /* Active powerups on line 2 */
    int power_x = 2;
    if (player->god_mode)
    {
        terminal_buffer_set_string(buf, power_x, 1, "[GOD]", COLOR_RED);
        power_x += 6;
    }
    if (player->has_shield)
    {
        terminal_buffer_set_string(buf, power_x, 1, "[S]", COLOR_YELLOW);
        power_x += 4;
    }
    if (player->has_dual_shot)
    {
        terminal_buffer_set_string(buf, power_x, 1, "[D]", COLOR_CYAN);
        power_x += 4;
    }
    if (player->has_speed)
    {
        terminal_buffer_set_string(buf, power_x, 1, "[F]", COLOR_ORANGE);
        power_x += 4;
    }
    if (player->has_mega_laser)
    {
        terminal_buffer_set_string(buf, power_x, 1, "[M]", COLOR_PURPLE);
        power_x += 4;
    }
    if (player->has_homing)
    {
        terminal_buffer_set_string(buf, power_x, 1, "[H]", COLOR_GREEN);
        power_x += 4;
    }
    if (player->has_lightning)
    {
        terminal_buffer_set_string(buf, power_x, 1, "[L]", COLOR_WHITE);
        power_x += 4;
    }
    if (player->has_reflect_shield)
    {
        terminal_buffer_set_string(buf, power_x, 1, "[R]", COLOR_BLUE);
        power_x += 4;
    }
    if (player->has_time_slow)
    {
        terminal_buffer_set_string(buf, power_x, 1, "[T]", COLOR_GRAY);
        power_x += 4;
    }
    if (player->has_ally_drone)
    {
        terminal_buffer_set_string(buf, power_x, 1, "[A]", COLOR_MAGENTA);
        power_x += 4;
    }

    /* Combo counter */
    if (player->combo_count > 0)
    {
        snprintf(text, sizeof(text), "COMBO: %d", player->combo_count);
        terminal_buffer_set_string(buf, 40, 1, text, COLOR_YELLOW);
    }
}

void renderer_draw_game_over(TerminalBuffer *buf, GameState *state, int screen_width, int screen_height)
{
    char text[64];
    int center_y = screen_height / 2;

    const char *game_over = "GAME OVER";
    int len = strlen(game_over);
    terminal_buffer_set_string(buf, (screen_width - len) / 2, center_y, game_over, COLOR_RED);

    snprintf(text, sizeof(text), "FINAL SCORE: %d", state->score);
    len = strlen(text);
    terminal_buffer_set_string(buf, (screen_width - len) / 2, center_y + 2, text, COLOR_WHITE);

    snprintf(text, sizeof(text), "WAVES COMPLETED: %d", state->current_wave);
    len = strlen(text);
    terminal_buffer_set_string(buf, (screen_width - len) / 2, center_y + 4, text, COLOR_WHITE);
}

void renderer_draw_wave_transition(TerminalBuffer *buf, GameState *state, int screen_width, int screen_height)
{
    char text[64];
    int center_y = screen_height / 2;

    snprintf(text, sizeof(text), "WAVE %d", state->current_wave);
    int len = strlen(text);
    terminal_buffer_set_string(buf, (screen_width - len) / 2, center_y, text, COLOR_CYAN);

    const char *ready = "GET READY!";
    len = strlen(ready);
    terminal_buffer_set_string(buf, (screen_width - len) / 2, center_y + 2, ready, COLOR_WHITE);
}

void renderer_draw_menu(TerminalBuffer *buf, int screen_width, int screen_height)
{
    int center_y = screen_height / 2 - 4;

    const char *title = "G A L A G A";
    int len = strlen(title);
    terminal_buffer_set_string(buf, (screen_width - len) / 2, center_y, title, COLOR_RED);

    const char *controls1 = "CONTROLS:";
    len = strlen(controls1);
    terminal_buffer_set_string(buf, (screen_width - len) / 2, center_y + 3, controls1, COLOR_CYAN);

    const char *controls2 = "Arrow Keys or WASD - Move";
    len = strlen(controls2);
    terminal_buffer_set_string(buf, (screen_width - len) / 2, center_y + 5, controls2, COLOR_WHITE);

    const char *controls3 = "SPACE - Shoot";
    len = strlen(controls3);
    terminal_buffer_set_string(buf, (screen_width - len) / 2, center_y + 6, controls3, COLOR_WHITE);

    const char *controls4 = "Q or ESC - Quit";
    len = strlen(controls4);
    terminal_buffer_set_string(buf, (screen_width - len) / 2, center_y + 7, controls4, COLOR_WHITE);

    const char *start = "Press SPACE to Start";
    len = strlen(start);
    terminal_buffer_set_string(buf, (screen_width - len) / 2, center_y + 10, start, COLOR_YELLOW);
}

void renderer_draw_bonus_stage_hud(TerminalBuffer *buf, BonusStage *bonus, GameState *state)
{
    char text[64];

    const char *bonus_text = "*** BONUS STAGE ***";
    int len = strlen(bonus_text);
    terminal_buffer_set_string(buf, (buf->width - len) / 2, 0, bonus_text, COLOR_YELLOW);

    snprintf(text, sizeof(text), "TIME: %.1f", bonus->timer);
    terminal_buffer_set_string(buf, 2, 1, text, COLOR_WHITE);

    snprintf(text, sizeof(text), "DESTROYED: %d/%d", bonus->enemies_destroyed, BONUS_ENEMIES);
    terminal_buffer_set_string(buf, buf->width - 25, 1, text, COLOR_WHITE);

    snprintf(text, sizeof(text), "SCORE: %d", state->score);
    terminal_buffer_set_string(buf, (buf->width - 20) / 2, 1, text, COLOR_WHITE);
}
