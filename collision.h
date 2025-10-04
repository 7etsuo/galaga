#ifndef COLLISION_H
#define COLLISION_H

#include "entities.h"
#include <stdbool.h>

typedef struct
{
    float x, y;
    float width, height;
} BoundingBox;

bool collision_check_aabb(BoundingBox a, BoundingBox b);
bool collision_check_point_box(float px, float py, BoundingBox box);

BoundingBox collision_get_player_box(Player *player);
BoundingBox collision_get_enemy_box(Enemy *enemy);
BoundingBox collision_get_bullet_box(Bullet *bullet);
BoundingBox collision_get_powerup_box(PowerUp *powerup);

bool collision_player_bullet(Player *player, Bullet *bullet);
bool collision_enemy_bullet(Enemy *enemy, Bullet *bullet);
bool collision_player_enemy(Player *player, Enemy *enemy);
bool collision_player_powerup(Player *player, PowerUp *powerup);

#endif
