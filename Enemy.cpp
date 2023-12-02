/**
* Author: Maximilian Ta
* Assignment: Platformer
* Date due: 2023-12-02, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Enemy.h"

void Enemy::ai_activate(Entity* player, float delta_time)
{
    switch (m_ai_type)
    {
    case BASIC:
        ai_basic(player, delta_time);
        break;

    default:
        break;
    }
}

void Enemy::ai_basic(Entity* player, float delta_time)
{
    m_attack_timer += delta_time;
    if (m_attack_timer > 2.0f) {
        m_attack_timer = 0.0f;
        m_movement *= -1.0f;
        m_animation_indices = m_walking[(int)((m_movement.x / 2.0f) + 0.5f)];
    }
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

    if (check_collision(player))
    {
        player->deactivate();
    }

    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    m_model_matrix = glm::scale(m_model_matrix, m_scale);
}
