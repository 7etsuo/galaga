#include "collision.h"
#include <math.h>

bool collision_check_aabb(BoundingBox a, BoundingBox b)
{
    return (a.x < b.x + b.width && a.x + a.width > b.x && a.y < b.y + b.height && a.y + a.height > b.y);
}

bool collision_check_point_box(float px, float py, BoundingBox box)
{
    return (px >= box.x && px < box.x + box.width && py >= box.y && py < box.y + box.height);
}

BoundingBox collision_get_player_box(Player *player)
{
    BoundingBox box;
    box.x = player->x - 1.0f;
    box.y = player->y - 1.0f;
    box.width = 3.0f;
    box.height = 2.0f;
    return box;
}

BoundingBox collision_get_enemy_box(Enemy *enemy)
{
    BoundingBox box;
    box.x = enemy->x - 1.0f;
    box.y = enemy->y - 1.0f;
    box.width = 3.0f;
    box.height = 2.0f;
    return box;
}

BoundingBox collision_get_bullet_box(Bullet *bullet)
{
    BoundingBox box;
    box.x = bullet->x;
    box.y = bullet->y;
    box.width = 1.0f;
    box.height = 1.0f;
    return box;
}

BoundingBox collision_get_powerup_box(PowerUp *powerup)
{
    BoundingBox box;
    box.x = powerup->x - 1.0f;
    box.y = powerup->y;
    box.width = 2.0f;
    box.height = 1.0f;
    return box;
}

bool collision_player_bullet(Player *player, Bullet *bullet)
{
    if (bullet->is_player_bullet || !bullet->active)
    {
        return false;
    }

    BoundingBox player_box = collision_get_player_box(player);
    BoundingBox bullet_box = collision_get_bullet_box(bullet);

    return collision_check_aabb(player_box, bullet_box);
}

bool collision_enemy_bullet(Enemy *enemy, Bullet *bullet)
{
    if (!bullet->is_player_bullet || !bullet->active || !enemy->active)
    {
        return false;
    }

    BoundingBox enemy_box = collision_get_enemy_box(enemy);
    BoundingBox bullet_box = collision_get_bullet_box(bullet);

    return collision_check_aabb(enemy_box, bullet_box);
}

bool collision_player_enemy(Player *player, Enemy *enemy)
{
    if (!enemy->active)
    {
        return false;
    }

    BoundingBox player_box = collision_get_player_box(player);
    BoundingBox enemy_box = collision_get_enemy_box(enemy);

    return collision_check_aabb(player_box, enemy_box);
}

bool collision_player_powerup(Player *player, PowerUp *powerup)
{
    if (!powerup->active)
    {
        return false;
    }

    BoundingBox player_box = collision_get_player_box(player);
    BoundingBox powerup_box = collision_get_powerup_box(powerup);

    return collision_check_aabb(player_box, powerup_box);
}
