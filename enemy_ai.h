#ifndef ENEMY_AI_H
#define ENEMY_AI_H

#include "entities.h"

#define FORMATION_COLS 10
#define FORMATION_ROWS 5

typedef struct
{
    Enemy enemies[MAX_ENEMIES];
    int active_count;
    float formation_offset_x;
    float formation_direction;
    float dive_spawn_timer;
    float capture_beam_timer;
    int difficulty_level;
} EnemyFormation;

void enemy_ai_init_formation(EnemyFormation *formation, int screen_width, int wave);
void enemy_ai_update_formation(EnemyFormation *formation, float dt, int screen_width);
void enemy_ai_trigger_dive(EnemyFormation *formation, Player *player, int screen_height);
void enemy_ai_trigger_capture(EnemyFormation *formation, Player *player);
void enemy_ai_update_dives(EnemyFormation *formation, float dt, Player *player, int screen_height);
int enemy_ai_count_active(EnemyFormation *formation);

#endif
