#include "enemy_ai.h"
#include <stdlib.h>
#include <math.h>

#define FORMATION_SPACING_X 6.0f
#define FORMATION_SPACING_Y 3.0f
#define FORMATION_START_Y 3.0f
#define FORMATION_MOVE_SPEED 15.0f
#define DIVE_SPEED 25.0f
#define DIVE_INTERVAL_BASE 3.0f
#define CAPTURE_INTERVAL 15.0f

static void create_dive_path(Enemy *enemy, float target_x, float target_y, int pattern);

void enemy_ai_init_formation(EnemyFormation *formation, int screen_width, int wave)
{
    formation->active_count = 0;
    formation->formation_offset_x = 0.0f;
    formation->formation_direction = 1.0f;
    formation->dive_spawn_timer = DIVE_INTERVAL_BASE;
    formation->capture_beam_timer = CAPTURE_INTERVAL;
    formation->difficulty_level = wave;

    int formation_width = FORMATION_COLS * FORMATION_SPACING_X;
    float start_x = (screen_width - formation_width) / 2.0f;

    int enemy_index = 0;

    for (int row = 0; row < FORMATION_ROWS && enemy_index < MAX_ENEMIES; row++)
    {
        EnemyType type;
        if (row == 0)
        {
            type = ENEMY_BOSS;
        }
        else if (row <= 2)
        {
            type = ENEMY_BUTTERFLY;
        }
        else
        {
            type = ENEMY_BEE;
        }

        for (int col = 0; col < FORMATION_COLS && enemy_index < MAX_ENEMIES; col++)
        {
            float form_x = start_x + col * FORMATION_SPACING_X;
            float form_y = FORMATION_START_Y + row * FORMATION_SPACING_Y;

            enemy_init(&formation->enemies[enemy_index], type, enemy_index, form_x, form_y);
            enemy_index++;
            formation->active_count++;
        }
    }
}

void enemy_ai_update_formation(EnemyFormation *formation, float dt, int screen_width)
{
    (void)screen_width;

    float speed_multiplier = 1.0f + (formation->difficulty_level * 0.1f);
    formation->formation_offset_x += formation->formation_direction * FORMATION_MOVE_SPEED * speed_multiplier * dt;

    if (formation->formation_offset_x > 10.0f || formation->formation_offset_x < -10.0f)
    {
        formation->formation_direction *= -1.0f;
    }

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        Enemy *enemy = &formation->enemies[i];
        if (enemy->active && enemy->state == ENEMY_STATE_FORMATION)
        {
            float oscillation = sinf(enemy->formation_x * 0.5f + formation->formation_offset_x * 0.1f) * 1.5f;
            enemy->x = enemy->formation_x + formation->formation_offset_x + oscillation;
            enemy->y = enemy->formation_y + cosf(formation->formation_offset_x * 0.3f) * 0.5f;
        }
    }
}

void enemy_ai_trigger_dive(EnemyFormation *formation, Player *player, int screen_height)
{
    (void)screen_height;

    formation->dive_spawn_timer -= 0.016f;

    if (formation->dive_spawn_timer > 0.0f)
    {
        return;
    }

    float dive_interval = DIVE_INTERVAL_BASE - (formation->difficulty_level * 0.2f);
    if (dive_interval < 0.5f)
    {
        dive_interval = 0.5f;
    }
    formation->dive_spawn_timer = dive_interval;

    int available_enemies[MAX_ENEMIES];
    int available_count = 0;

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (formation->enemies[i].active && formation->enemies[i].state == ENEMY_STATE_FORMATION)
        {
            available_enemies[available_count++] = i;
        }
    }

    if (available_count == 0)
    {
        return;
    }

    int dive_count = 1 + (rand() % 3);
    if (formation->difficulty_level > 3)
    {
        dive_count = 2 + (rand() % 3);
    }

    for (int d = 0; d < dive_count && available_count > 0; d++)
    {
        int random_index = rand() % available_count;
        int enemy_index = available_enemies[random_index];

        Enemy *enemy = &formation->enemies[enemy_index];
        enemy->state = ENEMY_STATE_DIVING;
        enemy->dive_timer = 0.0f;
        enemy->dive_path_index = 0;

        create_dive_path(enemy, player->x, player->y, rand() % 3);

        available_enemies[random_index] = available_enemies[--available_count];
    }
}

void enemy_ai_trigger_capture(EnemyFormation *formation, Player *player)
{
    if (player->captured || player->dual_fighter)
    {
        return;
    }

    formation->capture_beam_timer -= 0.016f;

    if (formation->capture_beam_timer > 0.0f)
    {
        return;
    }

    formation->capture_beam_timer = CAPTURE_INTERVAL;

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        Enemy *enemy = &formation->enemies[i];
        if (enemy->active && enemy->type == ENEMY_BOSS && enemy->state == ENEMY_STATE_FORMATION)
        {
            enemy->state = ENEMY_STATE_DIVING;
            enemy->dive_timer = 0.0f;
            enemy->dive_path_index = 0;
            create_dive_path(enemy, player->x, player->y, 3);
            break;
        }
    }
}

void enemy_ai_update_dives(EnemyFormation *formation, float dt, Player *player, int screen_height)
{
    float speed_multiplier = 1.0f + (formation->difficulty_level * 0.15f);

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        Enemy *enemy = &formation->enemies[i];

        if (!enemy->active)
        {
            continue;
        }

        if (enemy->state == ENEMY_STATE_DIVING)
        {
            enemy->dive_timer += dt;

            float progress = enemy->dive_timer * speed_multiplier * 0.5f;

            if (progress < 1.0f)
            {
                float target_x = player->x;
                float target_y = screen_height - 5.0f;

                float start_x = enemy->formation_x;
                float start_y = enemy->formation_y;

                float t = progress;
                float arc = sinf(t * 3.14159f) * 15.0f;

                if (enemy->type == ENEMY_BOSS && !enemy->has_captured_player)
                {
                    enemy->x = start_x + (target_x - start_x) * t;
                    enemy->y = start_y + (target_y - start_y) * t;

                    if (fabsf(enemy->x - player->x) < 2.0f && fabsf(enemy->y - player->y) < 2.0f)
                    {
                        if (!player->captured && !player->dual_fighter)
                        {
                            player_capture(player);
                            enemy->has_captured_player = true;
                        }
                    }
                }
                else
                {
                    float direction = (enemy->x < target_x) ? 1.0f : -1.0f;
                    enemy->x = start_x + (target_x - start_x) * t + arc * direction;
                    enemy->y = start_y + (target_y - start_y) * t;
                }
            }
            else if (progress < 1.5f)
            {
                float return_progress = (progress - 1.0f) * 2.0f;
                float target_y = screen_height + 5.0f;
                enemy->y += (target_y - enemy->y) * dt * 2.0f;

                if (return_progress > 0.5f)
                {
                    enemy->state = ENEMY_STATE_RETURNING;
                    enemy->dive_timer = 0.0f;
                }
            }
        }
        else if (enemy->state == ENEMY_STATE_RETURNING)
        {
            float dx = enemy->formation_x - enemy->x;
            float dy = enemy->formation_y - enemy->y;

            enemy->x += dx * dt * 2.0f;
            enemy->y += dy * dt * 2.0f;

            if (fabsf(dx) < 1.0f && fabsf(dy) < 1.0f)
            {
                enemy->state = ENEMY_STATE_FORMATION;
                enemy->x = enemy->formation_x;
                enemy->y = enemy->formation_y;
            }
        }
    }
}

int enemy_ai_count_active(EnemyFormation *formation)
{
    int count = 0;
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (formation->enemies[i].active)
        {
            count++;
        }
    }
    return count;
}

static void create_dive_path(Enemy *enemy, float target_x, float target_y, int pattern)
{
    (void)target_x;
    (void)target_y;
    (void)pattern;
    enemy->dive_timer = 0.0f;
}
