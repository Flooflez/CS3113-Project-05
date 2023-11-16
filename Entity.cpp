/**
* Author: Maximilian Ta
* Assignment: Rise of the AI
* Date due: 2023-11-18, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
*
* Original File created by Sebastián Romero Cruz
**/

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Entity.h"
#include "Utility.h"


Entity::Entity()
{
    // ––––– PHYSICS ––––– //
    m_position = glm::vec3(0.0f);
    m_velocity = glm::vec3(0.0f);
    m_base_velocity = glm::vec3(0.0f);
    m_acceleration = glm::vec3(0.0f);

    // ––––– TRANSLATION ––––– //
    m_movement = glm::vec3(0.0f);
    m_speed = 0;
    m_model_matrix = glm::mat4(1.0f);
}

Entity::~Entity()
{
    delete m_projectile_pointer;
    delete[] m_animation_up;
    delete[] m_animation_down;
    delete[] m_animation_left;
    delete[] m_animation_right;
    delete[] m_walking;
}

void Entity::draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index)
{
    // Step 1: Calculate the UV location of the indexed frame
    float u_coord = (float)(index % m_animation_cols) / (float)m_animation_cols;
    float v_coord = (float)(index / m_animation_cols) / (float)m_animation_rows;

    // Step 2: Calculate its UV size
    float width = 1.0f / (float)m_animation_cols;
    float height = 1.0f / (float)m_animation_rows;

    // Step 3: Just as we have done before, match the texture coordinates to the vertices
    float tex_coords[] =
    {
        u_coord, v_coord + height, u_coord + width, v_coord + height, u_coord + width, v_coord,
        u_coord, v_coord + height, u_coord + width, v_coord, u_coord, v_coord
    };

    float vertices[] =
    {
        -0.5, -0.5, 0.5, -0.5,  0.5, 0.5,
        -0.5, -0.5, 0.5,  0.5, -0.5, 0.5
    };

    // Step 4: And render
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());

    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

void Entity::activate() { 
    m_is_active = true; 
    
};
void Entity::deactivate() { 
    m_is_active = false; 
    if (m_projectile_pointer != nullptr) {
        delete m_projectile_pointer;
        m_projectile_pointer = nullptr;
    }
};

void Entity::ai_activate(Entity* player, float delta_time)
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

void Entity::ai_float(Entity* player, float delta_time)
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

void Entity::ai_jump(Entity* player, float delta_time)
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
            m_movement = glm::vec3(dir, 0.0f,0.0f);
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

void Entity::ai_shoot(Entity* player, float delta_time)
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

void Entity::shoot_projectile(glm::vec3 direction, float speed, glm::vec3 scale, glm::vec3 size)
{
    m_projectile_pointer = new Entity();
    m_projectile_pointer->m_texture_id = m_projectile_texture_id;
    m_projectile_pointer->set_speed(speed);
    m_projectile_pointer->set_movement(direction);
    m_projectile_pointer->set_position(m_position);
    m_projectile_pointer->set_scale(scale);
    m_projectile_pointer->set_height(size.y);
    m_projectile_pointer->set_width(size.x);
}


void Entity::update(float delta_time, Entity* player, Entity* objects, int object_count, Map* map)
{
    if (!m_is_active) return;


    if (m_entity_type == ENEMY) ai_activate(player, delta_time);
    m_collided_top = false;
    m_collided_bottom = false;
    m_collided_left = false;
    m_collided_right = false;

    

    if (m_projectile_pointer != nullptr) {
        m_projectile_pointer->update(delta_time, player, objects, object_count, map);
    }

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
    check_collision_x(map);
    m_position.y += m_velocity.y * delta_time;
    check_collision_y(objects, object_count);
    check_collision_y(map);
    


    if (m_is_jumping)
    {
        m_is_jumping = false;

        m_base_velocity.y += m_jumping_power;
    }

    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    m_model_matrix = glm::scale(m_model_matrix, m_scale);
}

void const Entity::check_collision_y(Entity* collidable_entities, int collidable_entity_count)
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
                m_base_velocity.y = m_jumping_power/2.0f; //mini jump when stomping
            }
        }

        if (collidable_entity->m_projectile_pointer != nullptr) {

            if (check_collision(collidable_entity->m_projectile_pointer))
            {
                deactivate();
            }
        }
    }
}

void const Entity::check_collision_x(Entity* collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity* collidable_entity = &collidable_entities[i];

        if (check_collision(collidable_entity))
        {
            deactivate();
        }

        if (collidable_entity->m_projectile_pointer != nullptr) {

            if (check_collision(collidable_entity->m_projectile_pointer))
            {
                deactivate();
            }
        }
    }
}


void const Entity::check_collision_y(Map* map)
{
    // Probes for tiles above
    glm::vec3 top = glm::vec3(m_position.x, m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_left = glm::vec3(m_position.x - (m_width / 2), m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_right = glm::vec3(m_position.x + (m_width / 2), m_position.y + (m_height / 2), m_position.z);

    // Probes for tiles below
    glm::vec3 bottom = glm::vec3(m_position.x, m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_left = glm::vec3(m_position.x - (m_width / 2), m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_right = glm::vec3(m_position.x + (m_width / 2), m_position.y - (m_height / 2), m_position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    // If the map is solid, check the top three points
    if (map->is_solid(top, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_base_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_left, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_base_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_right, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_base_velocity.y = 0;
        m_collided_top = true;
    }

    // And the bottom three points
    if (map->is_solid(bottom, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_base_velocity.y = 0;
        m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_left, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_base_velocity.y = 0;
        m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_right, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_base_velocity.y = 0;
        m_collided_bottom = true;

    }
}

void const Entity::check_collision_x(Map* map)
{
    // Probes for tiles; the x-checking is much simpler
    glm::vec3 left = glm::vec3(m_position.x - (m_width / 2), m_position.y, m_position.z);
    glm::vec3 right = glm::vec3(m_position.x + (m_width / 2), m_position.y, m_position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    if (map->is_solid(left, &penetration_x, &penetration_y) && m_velocity.x < 0)
    {
        m_position.x += penetration_x + 0.01f;
        m_base_velocity.x = 0;
        m_collided_left = true;
    }
    if (map->is_solid(right, &penetration_x, &penetration_y) && m_velocity.x > 0)
    {
        m_position.x -= penetration_x + 0.01f;
        m_base_velocity.x = 0;
        m_collided_right = true;
    }
}

void Entity::render_projectile(ShaderProgram* program) {
    if (m_projectile_pointer != nullptr) {
        m_projectile_pointer->render(program);
    }
}

void Entity::render(ShaderProgram* program)
{
    if (!m_is_active) return;

    program->set_model_matrix(m_model_matrix);

    if (m_animation_indices != NULL)
    {
        draw_sprite_from_texture_atlas(program, m_texture_id, m_animation_indices[m_animation_index]);
        return;
    }

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float tex_coords[] = { 0.0,  1.0, 1.0,  1.0, 1.0, 0.0,  0.0,  1.0, 1.0, 0.0,  0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, m_texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}


bool const Entity::check_collision(Entity* other) const
{
    if (other == this) return false;
    // If either entity is inactive, there shouldn't be any collision
    if (!m_is_active || !other->m_is_active) return false;

    float x_distance = fabs(m_position.x - other->m_position.x) - ((m_width + other->m_width) / 2.0f);
    float y_distance = fabs(m_position.y - other->m_position.y) - ((m_height + other->m_height) / 2.0f);

    return x_distance < 0.0f && y_distance < 0.0f;
}
