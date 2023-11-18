#include "Player.h"

void const Player::check_collision_y(Entity* collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity* collidable_entity = &collidable_entities[i];

        if (check_collision(collidable_entity))
        {
            if (m_velocity.y >= 0) {
                deactivate();
            }
            else if (m_velocity.y < 0) {
                collidable_entity->deactivate();
                m_base_velocity.y = m_jumping_power / 2.0f; //mini jump when stomping
            }
        }

        if (collidable_entity->get_projectile() != nullptr) {

            if (check_collision(collidable_entity->get_projectile()))
            {
                deactivate();
            }
        }
    }
}

void const Player::check_collision_x(Entity* collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity* collidable_entity = &collidable_entities[i];

        if (check_collision(collidable_entity))
        {
            deactivate();
        }

        if (collidable_entity->get_projectile() != nullptr) {

            if (check_collision(collidable_entity->get_projectile()))
            {
                deactivate();
            }
        }
    }
}

void Player::update(float delta_time, Entity* player, Entity* objects, int object_count, Map* map)
{
    if (!m_is_active) return;

    m_collided_top = false;
    m_collided_bottom = false;
    m_collided_left = false;
    m_collided_right = false;



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

    // We make two calls to our check_collision methods, one for the collidable objects and one for
    // the map.
    m_position.x += m_velocity.x * delta_time;
    check_collision_x(objects, object_count);
    Entity::check_collision_x(map);
    m_position.y += m_velocity.y * delta_time;
    check_collision_y(objects, object_count);
    Entity::check_collision_y(map);

    if (m_is_jumping)
    {
        m_is_jumping = false;

        m_base_velocity.y += m_jumping_power;
    }

    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    m_model_matrix = glm::scale(m_model_matrix, m_scale);
}
