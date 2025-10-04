#include "bonus_stage.h"
#include <math.h>

#define BONUS_ENEMY_SPEED 20.0f
#define SPAWN_INTERVAL 0.5f

void bonus_stage_init(BonusStage *bonus, int screen_width)
{
    bonus->timer = BONUS_STAGE_DURATION;
    bonus->enemies_destroyed = 0;
    bonus->active = true;
    bonus->spawn_timer = 0.0f;
    bonus->enemies_spawned = 0;

    for (int i = 0; i < BONUS_ENEMIES; i++)
    {
        bonus->enemies[i].active = false;
    }

    (void)screen_width;
}

void bonus_stage_update(BonusStage *bonus, float dt, int screen_width)
{
    if (!bonus->active)
    {
        return;
    }

    bonus->timer -= dt;
    bonus->spawn_timer -= dt;

    if (bonus->spawn_timer <= 0.0f && bonus->enemies_spawned < BONUS_ENEMIES)
    {
        Enemy *enemy = &bonus->enemies[bonus->enemies_spawned];

        int side = bonus->enemies_spawned % 2;
        float start_x = (side == 0) ? 5.0f : screen_width - 5.0f;
        float start_y = 5.0f + (bonus->enemies_spawned / 2) * 2.0f;

        enemy_init(enemy, ENEMY_BEE, bonus->enemies_spawned, start_x, start_y);
        enemy->x = start_x;
        enemy->y = start_y;
        enemy->vx = (side == 0) ? BONUS_ENEMY_SPEED : -BONUS_ENEMY_SPEED;
        enemy->vy = 0.0f;

        bonus->enemies_spawned++;
        bonus->spawn_timer = SPAWN_INTERVAL;
    }

    for (int i = 0; i < BONUS_ENEMIES; i++)
    {
        Enemy *enemy = &bonus->enemies[i];
        if (enemy->active)
        {
            enemy->x += enemy->vx * dt;
            enemy->y = enemy->formation_y + sinf(enemy->x * 0.2f) * 3.0f;

            if (enemy->x < -5.0f || enemy->x > screen_width + 5.0f)
            {
                enemy->active = false;
            }
        }
    }

    if (bonus->timer <= 0.0f)
    {
        bonus->active = false;
    }
}

bool bonus_stage_is_complete(BonusStage *bonus)
{
    return !bonus->active;
}

int bonus_stage_calculate_score(BonusStage *bonus)
{
    int base_score = bonus->enemies_destroyed * 500;

    if (bonus->enemies_destroyed == BONUS_ENEMIES)
    {
        base_score += 10000;
    }

    return base_score;
}
