#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include "terminal.h"
#include "input.h"
#include "entities.h"
#include "collision.h"
#include "enemy_ai.h"
#include "game_state.h"
#include "bonus_stage.h"
#include "renderer.h"

/* Game timing constants */
#define TARGET_FPS 30
#define FRAME_TIME (1.0f / TARGET_FPS)
#define MAX_FRAME_TIME 0.1f

/* Gameplay constants */
#define POWERUP_DROP_CHANCE 15
#define ENEMY_SHOOT_CHANCE 5
#define ENEMY_SHOOT_CHANCE_DIVISOR 1000
#define BULLET_SPEED 30.0f

static volatile bool running = true;

void signal_handler(int sig)
{
    (void)sig;
    running = false;
}

void setup_signal_handlers(void)
{
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
}

float get_delta_time(struct timespec *last_time)
{
    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time);

    float dt = (current_time.tv_sec - last_time->tv_sec) + (current_time.tv_nsec - last_time->tv_nsec) / 1000000000.0f;

    *last_time = current_time;
    return dt;
}

void spawn_powerup(PowerUp powerups[], int max_powerups, float x, float y)
{
    /* Random chance to drop a power-up */
    if (rand() % 100 < POWERUP_DROP_CHANCE)
    {
        for (int i = 0; i < max_powerups; i++)
        {
            if (!powerups[i].active)
            {
                /* Weighted random selection of powerup types */
                int roll = rand() % 100;
                PowerUpType type;

                if (roll < 15)
                    type = POWERUP_DUAL_SHOT;
                else if (roll < 30)
                    type = POWERUP_SHIELD;
                else if (roll < 40)
                    type = POWERUP_SPEED;
                else if (roll < 50)
                    type = POWERUP_MEGA_LASER;
                else if (roll < 60)
                    type = POWERUP_BOMB;
                else if (roll < 70)
                    type = POWERUP_HOMING;
                else if (roll < 78)
                    type = POWERUP_LIGHTNING;
                else if (roll < 86)
                    type = POWERUP_REFLECT_SHIELD;
                else if (roll < 94)
                    type = POWERUP_TIME_SLOW;
                else
                    type = POWERUP_ALLY_DRONE; /* Rarest powerup */

                powerup_init(&powerups[i], x, y, type);
                break;
            }
        }
    }
}

int main(void)
{
    srand(time(NULL));
    setup_signal_handlers();

    terminal_init();
    input_init();

    int screen_width, screen_height;
    terminal_get_size(&screen_width, &screen_height);

    if (!terminal_validate_size())
    {
        terminal_cleanup();
        fprintf(stderr, "Terminal too small. Need at least 80x24.\n");
        return 1;
    }

    TerminalBuffer *buffer = terminal_buffer_create(screen_width, screen_height);
    if (!buffer)
    {
        terminal_cleanup();
        fprintf(stderr, "Failed to create terminal buffer.\n");
        return 1;
    }

    Player player;
    player_init(&player, screen_width, screen_height);

    Bullet bullets[MAX_BULLETS] = {0};
    PowerUp powerups[MAX_POWERUPS] = {0};
    Star stars[MAX_STARS];
    stars_init(stars, MAX_STARS, screen_width, screen_height);

    GameState game_state;
    game_state_init(&game_state);

    EnemyFormation formation = {0};
    BonusStage bonus_stage = {0};

    InputState input = {0};
    input.up_time = 0.0f;
    input.down_time = 0.0f;
    input.left_time = 0.0f;
    input.right_time = 0.0f;
    input.shoot_time = 0.0f;

    struct timespec last_time;
    clock_gettime(CLOCK_MONOTONIC, &last_time);

    bool game_started = false;

    while (running)
    {
        float dt = get_delta_time(&last_time);
        if (dt > MAX_FRAME_TIME)
            dt = MAX_FRAME_TIME;

        input.god_toggle = false;
        input.bomb = false;
        input.special = false;
        input_update_state(&input, dt);

        if (input.god_toggle)
        {
            player.god_mode = !player.god_mode;
        }

        if (input.quit)
        {
            running = false;
            break;
        }

        if (game_state.state == GAME_STATE_MENU)
        {
            if (input.shoot && !game_started)
            {
                game_started = true;
                game_state_start_wave(&game_state, &formation, screen_width);
            }
        }
        else if (game_state.state == GAME_STATE_PLAYING)
        {
            player.vx = 0.0f;
            player.vy = 0.0f;

            if (input.left)
                player.vx = -1.0f;
            if (input.right)
                player.vx = 1.0f;
            if (input.up)
                player.vy = -1.0f;
            if (input.down)
                player.vy = 1.0f;
            if (input.shoot)
                player_shoot(&player, bullets, MAX_BULLETS);

            /* Handle bomb */
            if (input.bomb && player.bomb_count > 0)
            {
                player.bomb_count--;
                /* Clear all enemies on screen */
                for (int i = 0; i < MAX_ENEMIES; i++)
                {
                    if (formation.enemies[i].active)
                    {
                        int score = 50; /* Reduced score for bomb kills */
                        if (formation.enemies[i].type == ENEMY_BUTTERFLY)
                            score = 75;
                        if (formation.enemies[i].type == ENEMY_BOSS)
                            score = 150;
                        formation.enemies[i].active = false;
                        game_state_add_score(&game_state, score);
                    }
                }
                /* Clear all enemy bullets */
                for (int i = 0; i < MAX_BULLETS; i++)
                {
                    if (bullets[i].active && !bullets[i].is_player_bullet)
                        bullets[i].active = false;
                }
            }

            /* Handle special weapon */
            if (input.special && player.special_ready)
            {
                player.special_ready = false;
                player.special_charge = 0.0f;
                /* Fire a spread of mega lasers */
                int spread_count = 0;
                for (int i = 0; i < MAX_BULLETS && spread_count < 5; i++)
                {
                    if (!bullets[i].active)
                    {
                        float angle = -0.4f + (spread_count * 0.2f);
                        bullet_init_special(&bullets[i], player.x, player.y - 1.0f, angle * BULLET_SPEED, -BULLET_SPEED,
                                            true, BULLET_MEGA_LASER);
                        bullets[i].pierce_count = 10; /* Super pierce */
                        spread_count++;
                    }
                }
            }

            player_update(&player, dt, screen_width, screen_height);

            enemy_ai_update_formation(&formation, dt, screen_width);
            enemy_ai_trigger_dive(&formation, &player, screen_height);
            enemy_ai_trigger_capture(&formation, &player);
            enemy_ai_update_dives(&formation, dt, &player, screen_height);

            for (int i = 0; i < MAX_ENEMIES; i++)
            {
                enemy_update(&formation.enemies[i], dt);

                if (formation.enemies[i].active && formation.enemies[i].state == ENEMY_STATE_FORMATION &&
                    rand() % ENEMY_SHOOT_CHANCE_DIVISOR < ENEMY_SHOOT_CHANCE)
                {
                    enemy_shoot(&formation.enemies[i], bullets, MAX_BULLETS);
                }
            }

            for (int i = 0; i < MAX_BULLETS; i++)
            {
                bullet_update(&bullets[i], dt, screen_height);
            }

            for (int i = 0; i < MAX_POWERUPS; i++)
            {
                powerup_update(&powerups[i], dt, screen_height);
            }

            for (int i = 0; i < MAX_BULLETS; i++)
            {
                if (!bullets[i].active)
                    continue;

                if (bullets[i].is_player_bullet)
                {
                    for (int j = 0; j < MAX_ENEMIES; j++)
                    {
                        if (collision_enemy_bullet(&formation.enemies[j], &bullets[i]))
                        {
                            int score = 100;
                            if (formation.enemies[j].type == ENEMY_BUTTERFLY)
                                score = 150;
                            if (formation.enemies[j].type == ENEMY_BOSS)
                                score = 300;

                            if (formation.enemies[j].type == ENEMY_BOSS && formation.enemies[j].has_captured_player)
                            {
                                player_free(&player);
                            }

                            spawn_powerup(powerups, MAX_POWERUPS, formation.enemies[j].x, formation.enemies[j].y);

                            formation.enemies[j].active = false;
                            bullets[i].active = false;

                            /* Update combo system */
                            player.combo_count++;
                            player.combo_timer = 2.0f; /* Reset combo timer */
                            if (player.combo_count >= 5)
                                player.score_multiplier = 4;
                            else if (player.combo_count >= 3)
                                player.score_multiplier = 2;
                            else
                                player.score_multiplier = 1;

                            /* Apply score with multiplier */
                            game_state_add_score(&game_state, score * player.score_multiplier);
                            break;
                        }
                    }
                }
                else
                {
                    if (collision_player_bullet(&player, &bullets[i]))
                    {
                        player_hit(&player);
                        if (player.health <= 0 && !player.god_mode)
                        {
                            game_state_player_died(&game_state);
                        }
                        bullets[i].active = false;
                    }
                }
            }

            for (int i = 0; i < MAX_ENEMIES; i++)
            {
                if (formation.enemies[i].active && collision_player_enemy(&player, &formation.enemies[i]))
                {
                    player_hit(&player);
                    if (player.health <= 0 && !player.god_mode)
                    {
                        game_state_player_died(&game_state);
                    }
                    if (!player.god_mode && !player.has_shield)
                    {
                        formation.enemies[i].active = false;
                    }
                }
            }

            for (int i = 0; i < MAX_POWERUPS; i++)
            {
                if (collision_player_powerup(&player, &powerups[i]))
                {
                    powerup_apply(&powerups[i], &player);
                }
            }

            if (enemy_ai_count_active(&formation) == 0)
            {
                game_state_complete_wave(&game_state, &player);
                game_state_start_wave(&game_state, &formation, screen_width);
            }

            if (game_state_is_game_over(&game_state, &player))
            {
                game_state.state = GAME_STATE_GAME_OVER;
            }
        }
        else if (game_state.state == GAME_STATE_BONUS_STAGE)
        {
            if (bonus_stage.active == false || bonus_stage.timer == BONUS_STAGE_DURATION)
            {
                bonus_stage_init(&bonus_stage, screen_width);
            }

            bonus_stage_update(&bonus_stage, dt, screen_width);

            player.vx = 0.0f;
            player.vy = 0.0f;
            if (input.left)
                player.vx = -1.0f;
            if (input.right)
                player.vx = 1.0f;
            if (input.up)
                player.vy = -1.0f;
            if (input.down)
                player.vy = 1.0f;
            if (input.shoot)
                player_shoot(&player, bullets, MAX_BULLETS);

            player_update(&player, dt, screen_width, screen_height);

            for (int i = 0; i < MAX_BULLETS; i++)
            {
                bullet_update(&bullets[i], dt, screen_height);
            }

            for (int i = 0; i < MAX_BULLETS; i++)
            {
                if (!bullets[i].active || !bullets[i].is_player_bullet)
                    continue;

                for (int j = 0; j < BONUS_ENEMIES; j++)
                {
                    if (collision_enemy_bullet(&bonus_stage.enemies[j], &bullets[i]))
                    {
                        bonus_stage.enemies[j].active = false;
                        bullets[i].active = false;
                        bonus_stage.enemies_destroyed++;
                        game_state_add_score(&game_state, 500);
                        break;
                    }
                }
            }

            if (bonus_stage_is_complete(&bonus_stage))
            {
                int bonus_score = bonus_stage_calculate_score(&bonus_stage);
                game_state_add_score(&game_state, bonus_score);
                game_state_start_wave(&game_state, &formation, screen_width);
            }
        }

        game_state_update(&game_state, dt);

        terminal_buffer_clear(buffer);

        if (game_state.state == GAME_STATE_MENU)
        {
            renderer_draw_menu(buffer, screen_width, screen_height);
        }
        else if (game_state.state == GAME_STATE_WAVE_TRANSITION)
        {
            renderer_draw_stars(buffer, stars, MAX_STARS);
            renderer_draw_wave_transition(buffer, &game_state, screen_width, screen_height);
        }
        else if (game_state.state == GAME_STATE_PLAYING)
        {
            renderer_draw_stars(buffer, stars, MAX_STARS);

            for (int i = 0; i < MAX_ENEMIES; i++)
            {
                renderer_draw_enemy(buffer, &formation.enemies[i]);
            }

            for (int i = 0; i < MAX_BULLETS; i++)
            {
                renderer_draw_bullet(buffer, &bullets[i]);
            }

            for (int i = 0; i < MAX_POWERUPS; i++)
            {
                renderer_draw_powerup(buffer, &powerups[i]);
            }

            renderer_draw_player(buffer, &player);
            renderer_draw_hud(buffer, &player, &game_state);
        }
        else if (game_state.state == GAME_STATE_BONUS_STAGE)
        {
            renderer_draw_stars(buffer, stars, MAX_STARS);

            for (int i = 0; i < BONUS_ENEMIES; i++)
            {
                renderer_draw_enemy(buffer, &bonus_stage.enemies[i]);
            }

            for (int i = 0; i < MAX_BULLETS; i++)
            {
                renderer_draw_bullet(buffer, &bullets[i]);
            }

            renderer_draw_player(buffer, &player);
            renderer_draw_bonus_stage_hud(buffer, &bonus_stage, &game_state);
        }
        else if (game_state.state == GAME_STATE_GAME_OVER)
        {
            renderer_draw_stars(buffer, stars, MAX_STARS);
            renderer_draw_game_over(buffer, &game_state, screen_width, screen_height);
        }

        terminal_buffer_flush(buffer);

        /* Sleep to maintain target framerate */
        struct timespec sleep_time = {.tv_sec = 0, .tv_nsec = (long)(FRAME_TIME * 1000000000)};
        nanosleep(&sleep_time, NULL);
    }

    terminal_buffer_destroy(buffer);
    input_cleanup();
    terminal_cleanup();

    return 0;
}
