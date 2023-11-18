#pragma once
#include "Entity.h"

class Goal : public Entity {
    bool m_win = false;
public:
    void update(float delta_time, Entity* player, Entity* objects, int object_count, Map* map);

    bool level_win() { return m_win; }
};