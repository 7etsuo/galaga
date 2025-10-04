#include "entities.h"
#include <stdlib.h>
#include <math.h>

/* Movement speeds */
#define PLAYER_SPEED_X 35.0f
#define PLAYER_SPEED_Y 20.0f
#define PLAYER_SPEED_BOOST_X 50.0f
#define PLAYER_SPEED_BOOST_Y 30.0f
#define BULLET_SPEED 30.0f
#define ENEMY_BULLET_SPEED 20.0f
#define POWERUP_FALL_SPEED 10.0f

/* Cooldowns and timers */
#define PLAYER_SHOOT_COOLDOWN 0.15f
#define ENEMY_SHOOT_COOLDOWN_MIN 1.5f
#define ENEMY_SHOOT_COOLDOWN_RANDOM_RANGE 2.0f
#define INVINCIBILITY_TIME_HIT 1.0f
#define INVINCIBILITY_TIME_DEATH 2.0f

/* Power-up durations */
#define POWERUP_LIFETIME 8.0f
#define SHIELD_DURATION 5.0f
#define SPEED_BOOST_DURATION 10.0f
#define DUAL_SHOT_DURATION 15.0f
#define MEGA_LASER_DURATION 10.0f
#define HOMING_DURATION 12.0f
#define LIGHTNING_DURATION 8.0f
#define REFLECT_SHIELD_DURATION 7.0f
#define TIME_SLOW_DURATION 6.0f
#define ALLY_DRONE_DURATION 20.0f
#define COMBO_WINDOW 2.0f

/* Animation */
#define ENEMY_ANIMATION_FRAME_TIME 0.2f
#define ENEMY_ANIMATION_FRAMES 2

/* Player positioning constraints */
#define PLAYER_EDGE_MARGIN 1.0f
#define PLAYER_TOP_MARGIN 3.0f               /* Keep below HUD at top */
#define PLAYER_SPRITE_LEFT_OFFSET 1.0f       /* Sprite extends 1 left (x-1) */
#define PLAYER_SPRITE_RIGHT_OFFSET 1.0f      /* Sprite extends 1 right (x+1) */
#define PLAYER_SPRITE_BOTTOM_OFFSET 1.0f     /* Sprite extends 1 down (y+1) */
#define PLAYER_BOTTOM_SAFETY_MARGIN 3.0f     /* Keep sprite away from last line */
#define PLAYER_DUAL_FIGHTER_EXTRA_WIDTH 2.0f /* Dual adds 2 more right (to x+3) */

void player_init(Player *player, int screen_width, int screen_height)
{
    /* Calculate safe starting position within boundaries */
    float min_x = PLAYER_SPRITE_LEFT_OFFSET + PLAYER_EDGE_MARGIN;
    float max_x = screen_width - PLAYER_SPRITE_RIGHT_OFFSET - PLAYER_EDGE_MARGIN - 1.0f;
    float max_y = screen_height - PLAYER_SPRITE_BOTTOM_OFFSET - PLAYER_BOTTOM_SAFETY_MARGIN;

    player->x = (min_x + max_x) / 2.0f; /* Center horizontally */
    player->y = max_y - 1.0f;           /* Near bottom, safely within bounds */
    player->vx = 0.0f;
    player->vy = 0.0f;
    player->lives = PLAYER_STARTING_LIVES;
    player->health = PLAYER_STARTING_HEALTH;
    player->max_health = PLAYER_MAX_HEALTH;
    player->captured = false;
    player->dual_fighter = false;
    player->shoot_cooldown = 0.0f;
    player->has_shield = false;
    player->shield_timer = 0.0f;
    player->has_speed = false;
    player->speed_timer = 0.0f;
    player->has_dual_shot = false;
    player->dual_shot_timer = 0.0f;
    player->god_mode = false;
    player->invincibility_timer = 0.0f;

    /* New powerups */
    player->has_mega_laser = false;
    player->mega_laser_timer = 0.0f;
    player->bomb_count = 0;
    player->has_homing = false;
    player->homing_timer = 0.0f;
    player->has_lightning = false;
    player->lightning_timer = 0.0f;
    player->has_reflect_shield = false;
    player->reflect_shield_timer = 0.0f;
    player->has_time_slow = false;
    player->time_slow_timer = 0.0f;
    player->has_ally_drone = false;
    player->ally_drone_timer = 0.0f;
    player->ally_drone_x = player->x + 3.0f;
    player->ally_drone_y = player->y;

    /* Combo system */
    player->combo_count = 0;
    player->combo_timer = 0.0f;
    player->score_multiplier = 1;

    /* Special weapon */
    player->special_charge = 0.0f;
    player->special_ready = false;
}

void player_update(Player *player, float dt, int screen_width, int screen_height)
{
    float speed_x = player->has_speed ? PLAYER_SPEED_BOOST_X : PLAYER_SPEED_X;
    float speed_y = player->has_speed ? PLAYER_SPEED_BOOST_Y : PLAYER_SPEED_Y;

    /* Update position with separate X/Y speeds to account for terminal aspect ratio */
    player->x += player->vx * speed_x * dt;
    player->y += player->vy * speed_y * dt;

    /* Clamp to screen boundaries accounting for sprite size */
    /* Normal: 1 left, 1 right, 0 up, 1 down; Dual: +2 right extra */
    float right_offset = PLAYER_SPRITE_RIGHT_OFFSET;
    if (player->dual_fighter)
        right_offset += PLAYER_DUAL_FIGHTER_EXTRA_WIDTH;

    float min_x = PLAYER_SPRITE_LEFT_OFFSET + PLAYER_EDGE_MARGIN;
    float max_x = screen_width - right_offset - PLAYER_EDGE_MARGIN;
    float min_y = PLAYER_TOP_MARGIN;
    float max_y = screen_height - PLAYER_SPRITE_BOTTOM_OFFSET - PLAYER_BOTTOM_SAFETY_MARGIN;

    if (player->x < min_x)
        player->x = min_x;
    if (player->x > max_x)
        player->x = max_x;
    if (player->y < min_y)
        player->y = min_y;
    if (player->y > max_y)
        player->y = max_y;

    /* Update cooldowns and timers */
    if (player->shoot_cooldown > 0.0f)
        player->shoot_cooldown -= dt;

    if (player->invincibility_timer > 0.0f)
        player->invincibility_timer -= dt;

    /* Update power-up timers */
    if (player->has_shield)
    {
        player->shield_timer -= dt;
        if (player->shield_timer <= 0.0f)
            player->has_shield = false;
    }

    if (player->has_speed)
    {
        player->speed_timer -= dt;
        if (player->speed_timer <= 0.0f)
            player->has_speed = false;
    }

    if (player->has_dual_shot)
    {
        player->dual_shot_timer -= dt;
        if (player->dual_shot_timer <= 0.0f)
            player->has_dual_shot = false;
    }

    if (player->has_mega_laser)
    {
        player->mega_laser_timer -= dt;
        if (player->mega_laser_timer <= 0.0f)
            player->has_mega_laser = false;
    }

    if (player->has_homing)
    {
        player->homing_timer -= dt;
        if (player->homing_timer <= 0.0f)
            player->has_homing = false;
    }

    if (player->has_lightning)
    {
        player->lightning_timer -= dt;
        if (player->lightning_timer <= 0.0f)
            player->has_lightning = false;
    }

    if (player->has_reflect_shield)
    {
        player->reflect_shield_timer -= dt;
        if (player->reflect_shield_timer <= 0.0f)
            player->has_reflect_shield = false;
    }

    if (player->has_time_slow)
    {
        player->time_slow_timer -= dt;
        if (player->time_slow_timer <= 0.0f)
            player->has_time_slow = false;
    }

    if (player->has_ally_drone)
    {
        player->ally_drone_timer -= dt;
        if (player->ally_drone_timer <= 0.0f)
            player->has_ally_drone = false;
        else
        {
            /* Update drone position to follow player */
            player->ally_drone_x += (player->x + 3.0f - player->ally_drone_x) * 5.0f * dt;
            player->ally_drone_y += (player->y - player->ally_drone_y) * 5.0f * dt;
        }
    }

    /* Update combo timer */
    if (player->combo_timer > 0.0f)
    {
        player->combo_timer -= dt;
        if (player->combo_timer <= 0.0f)
        {
            player->combo_count = 0;
            player->score_multiplier = 1;
        }
    }

    /* Charge special weapon over time */
    if (!player->special_ready && player->special_charge < 100.0f)
    {
        player->special_charge += 10.0f * dt;
        if (player->special_charge >= 100.0f)
        {
            player->special_charge = 100.0f;
            player->special_ready = true;
        }
    }
}

void player_shoot(Player *player, Bullet bullets[], int max_bullets)
{
    if (player->shoot_cooldown > 0.0f)
        return;

    /* Determine bullet type based on powerups */
    BulletType bullet_type = BULLET_NORMAL;
    if (player->has_mega_laser)
        bullet_type = BULLET_MEGA_LASER;
    else if (player->has_homing)
        bullet_type = BULLET_HOMING;
    else if (player->has_lightning)
        bullet_type = BULLET_LIGHTNING;

    for (int i = 0; i < max_bullets; i++)
    {
        if (!bullets[i].active)
        {
            /* Fire primary bullet */
            bullet_init_special(&bullets[i], player->x, player->y - 1.0f, 0.0f, -BULLET_SPEED, true, bullet_type);
            player->shoot_cooldown = PLAYER_SHOOT_COOLDOWN;

            /* Fire dual-shot bullet if power-up is active */
            if (player->has_dual_shot && i + 1 < max_bullets)
            {
                bullet_init_special(&bullets[i + 1], player->x - 1.0f, player->y - 1.0f, 0.0f, -BULLET_SPEED, true,
                                    bullet_type);
                i++;
            }

            /* Fire dual-fighter bullet if active */
            if (player->dual_fighter && i + 1 < max_bullets)
            {
                bullet_init_special(&bullets[i + 1], player->x + 2.0f, player->y - 1.0f, 0.0f, -BULLET_SPEED, true,
                                    bullet_type);
                i++;
            }

            /* Ally drone shoots if active */
            if (player->has_ally_drone && i + 1 < max_bullets)
            {
                bullet_init(&bullets[i + 1], player->ally_drone_x, player->ally_drone_y - 1.0f, 0.0f, -BULLET_SPEED,
                            true);
                i++;
            }

            break;
        }
    }
}

void player_hit(Player *player)
{
    /* Check for invincibility */
    if (player->god_mode || player->has_shield || player->invincibility_timer > 0.0f)
        return;

    player->health--;
    player->invincibility_timer = INVINCIBILITY_TIME_HIT;

    /* Check if life is lost */
    if (player->health <= 0)
    {
        player->lives--;
        player->health = player->max_health;

        /* Reset power-ups on death */
        player->dual_fighter = false;
        player->has_dual_shot = false;
        player->has_speed = false;

        player->invincibility_timer = INVINCIBILITY_TIME_DEATH;
    }
}

void player_capture(Player *player)
{
    player->captured = true;
}

void player_free(Player *player)
{
    player->captured = false;
    player->dual_fighter = true;
}

void bullet_init(Bullet *bullet, float x, float y, float vx, float vy, bool is_player)
{
    bullet->x = x;
    bullet->y = y;
    bullet->vx = vx;
    bullet->vy = vy;
    bullet->active = true;
    bullet->is_player_bullet = is_player;
    bullet->type = BULLET_NORMAL;
    bullet->pierce_count = 0;
    bullet->target_enemy_id = -1;
    bullet->chain_count = 0;
}

void bullet_init_special(Bullet *bullet, float x, float y, float vx, float vy, bool is_player, BulletType type)
{
    bullet_init(bullet, x, y, vx, vy, is_player);
    bullet->type = type;

    /* Set special properties based on type */
    if (type == BULLET_MEGA_LASER)
        bullet->pierce_count = 3; /* Can pierce through 3 enemies */
    else if (type == BULLET_LIGHTNING)
        bullet->chain_count = 4; /* Can chain to 4 enemies */
}

void bullet_update(Bullet *bullet, float dt, int screen_height)
{
    if (!bullet->active)
        return;

    bullet->x += bullet->vx * dt;
    bullet->y += bullet->vy * dt;

    /* Deactivate if off-screen */
    if (bullet->y < 0.0f || bullet->y >= (float)screen_height)
        bullet->active = false;
}

void enemy_init(Enemy *enemy, EnemyType type, int formation_index, float form_x, float form_y)
{
    enemy->x = form_x;
    enemy->y = form_y;
    enemy->vx = 0.0f;
    enemy->vy = 0.0f;
    enemy->type = type;
    enemy->state = ENEMY_STATE_FORMATION;
    enemy->active = true;
    enemy->formation_index = formation_index;
    enemy->formation_x = form_x;
    enemy->formation_y = form_y;
    enemy->dive_timer = 0.0f;
    enemy->shoot_cooldown = 0.0f;
    enemy->dive_path_index = 0;
    enemy->has_captured_player = false;
    enemy->animation_frame = 0;
    enemy->animation_timer = 0.0f;
}

void enemy_update(Enemy *enemy, float dt)
{
    if (!enemy->active)
        return;

    /* Update animation */
    enemy->animation_timer += dt;
    if (enemy->animation_timer >= ENEMY_ANIMATION_FRAME_TIME)
    {
        enemy->animation_frame = (enemy->animation_frame + 1) % ENEMY_ANIMATION_FRAMES;
        enemy->animation_timer = 0.0f;
    }

    /* Update position */
    enemy->x += enemy->vx * dt;
    enemy->y += enemy->vy * dt;

    /* Update shoot cooldown */
    if (enemy->shoot_cooldown > 0.0f)
        enemy->shoot_cooldown -= dt;
}

void enemy_shoot(Enemy *enemy, Bullet bullets[], int max_bullets)
{
    if (enemy->shoot_cooldown > 0.0f || !enemy->active)
        return;

    for (int i = 0; i < max_bullets; i++)
    {
        if (!bullets[i].active)
        {
            bullet_init(&bullets[i], enemy->x, enemy->y + 1.0f, 0.0f, ENEMY_BULLET_SPEED, false);

            /* Randomize cooldown for variety */
            float random_offset = (rand() % 100) / (100.0f / ENEMY_SHOOT_COOLDOWN_RANDOM_RANGE);
            enemy->shoot_cooldown = ENEMY_SHOOT_COOLDOWN_MIN + random_offset;
            break;
        }
    }
}

void powerup_init(PowerUp *powerup, float x, float y, PowerUpType type)
{
    powerup->x = x;
    powerup->y = y;
    powerup->vy = POWERUP_FALL_SPEED;
    powerup->type = type;
    powerup->active = true;
    powerup->lifetime = POWERUP_LIFETIME;
}

void powerup_update(PowerUp *powerup, float dt, int screen_height)
{
    if (!powerup->active)
        return;

    powerup->y += powerup->vy * dt;
    powerup->lifetime -= dt;

    /* Deactivate if off-screen or expired */
    if (powerup->y >= (float)screen_height || powerup->lifetime <= 0.0f)
        powerup->active = false;
}

void powerup_apply(PowerUp *powerup, Player *player)
{
    switch (powerup->type)
    {
    case POWERUP_DUAL_SHOT:
        player->has_dual_shot = true;
        player->dual_shot_timer = DUAL_SHOT_DURATION;
        break;
    case POWERUP_SHIELD:
        player->has_shield = true;
        player->shield_timer = SHIELD_DURATION;
        break;
    case POWERUP_SPEED:
        player->has_speed = true;
        player->speed_timer = SPEED_BOOST_DURATION;
        break;
    case POWERUP_MEGA_LASER:
        player->has_mega_laser = true;
        player->mega_laser_timer = MEGA_LASER_DURATION;
        break;
    case POWERUP_BOMB:
        player->bomb_count += 3; /* Give 3 bombs */
        if (player->bomb_count > 9)
            player->bomb_count = 9; /* Max 9 bombs */
        break;
    case POWERUP_HOMING:
        player->has_homing = true;
        player->homing_timer = HOMING_DURATION;
        break;
    case POWERUP_LIGHTNING:
        player->has_lightning = true;
        player->lightning_timer = LIGHTNING_DURATION;
        break;
    case POWERUP_REFLECT_SHIELD:
        player->has_reflect_shield = true;
        player->reflect_shield_timer = REFLECT_SHIELD_DURATION;
        break;
    case POWERUP_TIME_SLOW:
        player->has_time_slow = true;
        player->time_slow_timer = TIME_SLOW_DURATION;
        break;
    case POWERUP_ALLY_DRONE:
        player->has_ally_drone = true;
        player->ally_drone_timer = ALLY_DRONE_DURATION;
        player->ally_drone_x = player->x + 3.0f;
        player->ally_drone_y = player->y;
        break;
    }
    powerup->active = false;
}

void stars_init(Star stars[], int count, int screen_width, int screen_height)
{
#define STAR_BOTTOM_MARGIN 5
#define STAR_TYPE_DIVISOR 3

    for (int i = 0; i < count; i++)
    {
        stars[i].x = rand() % screen_width;
        stars[i].y = rand() % (screen_height - STAR_BOTTOM_MARGIN);
        stars[i].character = (rand() % STAR_TYPE_DIVISOR == 0) ? '*' : '.';
    }
}
