#include "Enemy.h"

void Enemy::ai_activate(Entity* player, float delta_time)
{
    switch (m_ai_type)
    {
    case FLOATER:
        ai_float(player, delta_time);
        break;

    case JUMPER:
        ai_jump(player, delta_time);
        break;

    case SHOOTER:
        ai_shoot(player, delta_time);
        break;

    default:
        break;
    }
}

void Enemy::ai_float(Entity* player, float delta_time)
{
    
}

void Enemy::ai_jump(Entity* player, float delta_time)
{
}

void Enemy::ai_shoot(Entity* player, float delta_time)
{
    
}

void const Enemy::check_collision_y(Entity* collidable_entities, int collidable_entity_count)
{
    return;
}

void const Enemy::check_collision_x(Entity* collidable_entities, int collidable_entity_count)
{
	return;
}

void Enemy::update(float delta_time, Entity* player, Entity* objects, int object_count, Map* map)
{
    ai_activate(player, delta_time);
    if (m_animation_indices != NULL)
    {
        if (glm::length(m_movement) != 0)
        {
            m_animation_time += delta_time;
            float seconds_per_frame = 1.0f / FRAMES_PER_SECOND;

            if (m_animation_time >= seconds_per_frame)
            {
                m_animation_time = 0.0f;
                m_animation_index++;

                if (m_animation_index >= m_animation_frames)
                {
                    m_animation_index = 0;
                }
            }
        }
    }

    m_base_velocity += m_acceleration * delta_time;
    m_velocity = m_base_velocity + (m_movement * m_speed);

    m_position.x += m_velocity.x * delta_time;
    Entity::check_collision_x(map);
    m_position.y += m_velocity.y * delta_time;
    Entity::check_collision_y(map);

    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    m_model_matrix = glm::scale(m_model_matrix, m_scale);
}
