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
    switch (m_ai_state) {
    case IDLE:
        m_ai_state = WALKING;
        break;

    case WALKING:
        if (glm::abs(m_position.x - player->get_position().x) < 0.5f) {
            m_ai_state = ATTACKING;
            m_movement = glm::vec3(0.0f);
            m_attack_timer = 2.0f;
            return;
        }

        if (m_position.x > player->get_position().x) {
            m_movement = glm::vec3(-1.0f, 0.0f, 0.0f);
            m_animation_indices = m_walking[LEFT];
        }
        if (m_position.x < player->get_position().x) {
            m_movement = glm::vec3(1.0f, 0.0f, 0.0f);
            m_animation_indices = m_walking[RIGHT];
        }
        break;

    case ATTACKING:
        if (glm::abs(m_position.x - player->get_position().x) > 1.0f) {
            m_ai_state = WALKING;
            return;
        }

        m_attack_timer += delta_time;
        if (m_attack_timer > 3.0f) {
            delete m_projectile_pointer;
            shoot_projectile(glm::normalize(player->get_position() - m_position), 1.0f, glm::vec3(0.25f), glm::vec3(0.25f));
            m_attack_timer = 0;
        }

    default:
        break;
    }
}

void Enemy::ai_jump(Entity* player, float delta_time)
{
    m_attack_timer += delta_time;
    if (m_attack_timer > 3.0f) {
        float dir = 1.0f;
        if (m_position.x > player->get_position().x) {
            dir = -1.0f;
            m_animation_indices = m_walking[LEFT];
        }
        else {
            m_animation_indices = m_walking[RIGHT];
        }

        float player_distance = m_position.x - player->get_position().x;

        if (glm::abs(player_distance) > 1.0f) {
            m_base_velocity.y = m_jumping_power;
            m_movement = glm::vec3(dir, 0.0f, 0.0f);
            m_speed = 2.0f;
        }
        else {
            m_base_velocity.y = m_jumping_power;
            m_movement = glm::vec3(dir, 0.0f, 0.0f);
            m_speed = 1.0f;
        }
        m_attack_timer = 0;
    }
    else if (m_collided_bottom) {
        m_movement = glm::vec3(0.0f);
        m_animation_index = 0;
    }
}

void Enemy::ai_shoot(Entity* player, float delta_time)
{
    switch (m_ai_state) {
    case IDLE:
        m_movement = glm::vec3(-1.0f, 0.0f, 0.0f);
        shoot_projectile(m_movement, 3.0f, glm::vec3(0.25f), glm::vec3(0.25f));
        m_animation_indices = m_walking[LEFT];
        m_ai_state = ATTACKING;

        break;
    case ATTACKING:
        m_attack_timer += delta_time;
        if (m_attack_timer > 2.0f) {
            delete m_projectile_pointer;
            m_movement *= -1.0f;
            shoot_projectile(m_movement, 3.0f, glm::vec3(0.25f), glm::vec3(0.25f));
            m_animation_indices = m_walking[(int)((m_movement.x / 2.0f) + 0.5f)];
            m_attack_timer = 0;
        }
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
}
