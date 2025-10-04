#ifndef BONUS_STAGE_H
#define BONUS_STAGE_H

#include "entities.h"
#include "game_state.h"

#define BONUS_STAGE_DURATION 20.0f
#define BONUS_ENEMIES 20

typedef struct
{
    Enemy enemies[BONUS_ENEMIES];
    float timer;
    int enemies_destroyed;
    bool active;
    float spawn_timer;
    int enemies_spawned;
} BonusStage;

void bonus_stage_init(BonusStage *bonus, int screen_width);
void bonus_stage_update(BonusStage *bonus, float dt, int screen_width);
bool bonus_stage_is_complete(BonusStage *bonus);
int bonus_stage_calculate_score(BonusStage *bonus);

#endif
