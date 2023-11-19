#pragma once
#include "Entity.h"

class Player : public Entity {
public:
    void update(float delta_time, Entity* player, Entity* objects, int object_count, Map* map);
};
