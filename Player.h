#pragma once
#include "Entity.h"

class Player : public Entity {
    void const check_collision_y(Entity* collidable_entities, int collidable_entity_count) override;
    void const check_collision_x(Entity* collidable_entities, int collidable_entity_count) override;
public:
    void update(float delta_time, Entity* player, Entity* objects, int object_count, Map* map);
};
