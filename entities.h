#ifndef ENTITIES_H
#define ENTITIES_H

#include <stdbool.h>
#include <stdint.h>

/* Entity limits */
#define MAX_ENEMIES 50
#define MAX_BULLETS 100
#define MAX_POWERUPS 5
#define MAX_STARS 50

/* Player constants */
#define PLAYER_STARTING_LIVES 3
#define PLAYER_STARTING_HEALTH 3
#define PLAYER_MAX_HEALTH 3

typedef enum
{
    ENEMY_BEE,
    ENEMY_BUTTERFLY,
    ENEMY_BOSS
} EnemyType;

typedef enum
{
    ENEMY_STATE_INACTIVE,
    ENEMY_STATE_FORMATION,
    ENEMY_STATE_DIVING,
    ENEMY_STATE_RETURNING,
    ENEMY_STATE_CAPTURED_ESCORT
} EnemyState;

typedef enum
{
    POWERUP_DUAL_SHOT,
    POWERUP_SHIELD,
    POWERUP_SPEED,
    POWERUP_MEGA_LASER,
    POWERUP_BOMB,
    POWERUP_HOMING,
    POWERUP_LIGHTNING,
    POWERUP_REFLECT_SHIELD,
    POWERUP_TIME_SLOW,
    POWERUP_ALLY_DRONE
} PowerUpType;

typedef struct
{
    float x, y;
    float vx, vy;
    int lives;
    int health;
    int max_health;
    bool captured;
    bool dual_fighter;
    float shoot_cooldown;
    bool has_shield;
    float shield_timer;
    bool has_speed;
    float speed_timer;
    bool has_dual_shot;
    float dual_shot_timer;
    bool god_mode;
    float invincibility_timer;

    /* New powerups */
    bool has_mega_laser;
    float mega_laser_timer;
    int bomb_count;
    bool has_homing;
    float homing_timer;
    bool has_lightning;
    float lightning_timer;
    bool has_reflect_shield;
    float reflect_shield_timer;
    bool has_time_slow;
    float time_slow_timer;
    bool has_ally_drone;
    float ally_drone_timer;
    float ally_drone_x;
    float ally_drone_y;

    /* Combo system */
    int combo_count;
    float combo_timer;
    int score_multiplier;

    /* Special weapon charge */
    float special_charge;
    bool special_ready;
} Player;

typedef enum
{
    BULLET_NORMAL,
    BULLET_MEGA_LASER,
    BULLET_HOMING,
    BULLET_LIGHTNING
} BulletType;

typedef struct
{
    float x, y;
    float vx, vy;
    bool active;
    bool is_player_bullet;
    BulletType type;
    int pierce_count;      /* For mega laser */
    float target_enemy_id; /* For homing missiles */
    int chain_count;       /* For lightning */
} Bullet;

typedef struct
{
    float x, y;
    float vx, vy;
    EnemyType type;
    EnemyState state;
    bool active;
    int formation_index;
    float formation_x;
    float formation_y;
    float dive_timer;
    float shoot_cooldown;
    int dive_path_index;
    bool has_captured_player;
    int animation_frame;
    float animation_timer;
} Enemy;

typedef struct
{
    float x, y;
    float vy;
    PowerUpType type;
    bool active;
    float lifetime;
} PowerUp;

typedef struct
{
    int x, y;
    char character;
} Star;

void player_init(Player *player, int screen_width, int screen_height);
void player_update(Player *player, float dt, int screen_width, int screen_height);
void player_shoot(Player *player, Bullet bullets[], int max_bullets);
void player_hit(Player *player);
void player_capture(Player *player);
void player_free(Player *player);

void bullet_init(Bullet *bullet, float x, float y, float vx, float vy, bool is_player);
void bullet_init_special(Bullet *bullet, float x, float y, float vx, float vy, bool is_player, BulletType type);
void bullet_update(Bullet *bullet, float dt, int screen_height);

void enemy_init(Enemy *enemy, EnemyType type, int formation_index, float form_x, float form_y);
void enemy_update(Enemy *enemy, float dt);
void enemy_shoot(Enemy *enemy, Bullet bullets[], int max_bullets);

void powerup_init(PowerUp *powerup, float x, float y, PowerUpType type);
void powerup_update(PowerUp *powerup, float dt, int screen_height);
void powerup_apply(PowerUp *powerup, Player *player);

void stars_init(Star stars[], int count, int screen_width, int screen_height);

#endif
