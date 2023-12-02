/**
* Author: Maximilian Ta
* Assignment: Platformer
* Date due: 2023-12-02, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
*
* Original File created by Sebastián Romero Cruz
**/

#include "LevelB.h"
#include "Utility.h"

#define LEVEL_WIDTH 50
#define LEVEL_HEIGHT 10

unsigned int LEVELB_DATA[] =
{
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 6, 0, 0, 6, 7, 7, 7, 6, 0, 0, 0, 0, 0, 0, 0, 0, 8, 
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 6, 7, 7, 7, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 7, 6, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 6, 0, 0, 0, 0, 0, 0, 8, 
    8, 0, 0, 0, 0, 6, 7, 7, 7, 6, 0, 0, 9, 9, 0, 1, 0, 0, 0, 1, 0, 0, 3, 3, 3, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 6, 7, 7, 7, 6, 0, 1, 0, 0, 0, 0, 0, 0, 8, 
    8, 0, 0, 0, 9, 1, 0, 0, 0, 1, 0, 0, 9, 9, 9, 1, 0, 6, 6, 1, 0, 0, 2, 4, 2, 0, 0, 0, 0, 9, 0, 1, 0, 6, 7, 6, 1, 0, 0, 0, 1, 0, 1, 0, 6, 7, 6, 0, 0, 8, 
    8, 3, 3, 3, 3, 1, 0, 0, 0, 1, 0, 6, 7, 7, 7, 6, 0, 1, 1, 1, 9, 0, 2, 2, 2, 0, 0, 0, 0, 9, 9, 1, 0, 1, 0, 1, 1, 0, 6, 7, 7, 6, 1, 0, 1, 0, 1, 0, 9, 8, 
    8, 2, 4, 4, 2, 1, 0, 6, 7, 6, 0, 1, 0, 0, 0, 1, 0, 1, 1, 6, 7, 7, 2, 4, 2, 0, 0, 6, 7, 7, 7, 6, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 3, 3, 3, 
    8, 2, 2, 2, 2, 1, 0, 1, 0, 1, 6, 7, 7, 7, 7, 7, 6, 1, 1, 1, 0, 0, 2, 2, 2, 0, 0, 1, 0, 0, 0, 1, 0, 6, 7, 6, 1, 0, 1, 0, 0, 1, 1, 0, 1, 9, 1, 2, 4, 2, 
    8, 2, 4, 4, 2, 1, 0, 1, 9, 1, 1, 0, 0, 0, 0, 0, 1, 1, 3, 3, 3, 3, 2, 2, 2, 9, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 3, 3, 3, 3, 2, 2, 2, 
    8, 2, 2, 2, 2, 1, 0, 5, 5, 5, 1, 0, 0, 0, 0, 0, 1, 9, 2, 4, 4, 2, 4, 4, 2, 9, 9, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 3, 3, 3, 3, 1, 2, 4, 2, 4, 2, 4, 2, 
    8, 2, 4, 4, 2, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 0, 0, 3, 3, 3, 3, 0, 3, 3, 3, 2, 4, 4, 2, 1, 2, 2, 2, 2, 2, 2, 2, 
};

LevelB::~LevelB()
{
    delete[] m_state.enemies;
    delete    m_state.player;
    delete    m_state.map;
    Mix_FreeChunk(m_state.jump_sfx);
    Mix_FreeMusic(m_state.bgm);
}

void LevelB::initialise()
{
    GLuint map_texture_id = Utility::load_texture("assets/images/platforms.png");
    m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELB_DATA, map_texture_id, 1.0f, 10, 1);

    // Code from main.cpp's initialise()
    /**
     George's Stuff
     */
     // Existing
    m_state.player = new Player();
    m_state.player->set_position(glm::vec3(3.0f, -2.0f, 0.0f));
    m_state.player->set_movement(glm::vec3(0.0f));
    m_state.player->set_speed(2.5f);
    m_state.player->set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    m_state.player->m_texture_id = Utility::load_texture("assets/images/player.png");

    // Walking
    m_state.player->m_walking[m_state.player->LEFT] = new int[4] { 1, 5, 9, 13 };
    m_state.player->m_walking[m_state.player->RIGHT] = new int[4] { 3, 7, 11, 15 };
    m_state.player->m_walking[m_state.player->UP] = new int[4] { 2, 6, 10, 14 };
    m_state.player->m_walking[m_state.player->DOWN] = new int[4] { 0, 4, 8, 12 };

    m_state.player->m_animation_indices = m_state.player->m_walking[m_state.player->RIGHT];
    m_state.player->m_animation_frames = 4;
    m_state.player->m_animation_index = 0;
    m_state.player->m_animation_time = 0.0f;
    m_state.player->m_animation_cols = 4;
    m_state.player->m_animation_rows = 4;
    m_state.player->set_height(0.8f);
    m_state.player->set_width(0.3f);

    // Jumping
    m_state.player->m_jumping_power = 5.0f;

    /**
     Enemies' stuff */

    m_state.enemies = new Enemy[ENEMY_COUNT];

    //enemy 1
    m_state.enemies[0].m_walking[m_state.enemies[0].LEFT] = new int[4] { 1, 5, 9, 13 };
    m_state.enemies[0].m_walking[m_state.enemies[0].RIGHT] = new int[4] { 3, 7, 11, 15 };
    m_state.enemies[0].m_walking[m_state.enemies[0].UP] = new int[4] { 2, 6, 10, 14 };
    m_state.enemies[0].m_walking[m_state.enemies[0].DOWN] = new int[4] { 0, 4, 8, 12 };
    m_state.enemies[0].m_animation_indices = m_state.enemies[0].m_walking[m_state.enemies[0].RIGHT];
    m_state.enemies[0].m_animation_frames = 4;
    m_state.enemies[0].m_animation_index = 0;
    m_state.enemies[0].m_animation_time = 0.0f;
    m_state.enemies[0].m_animation_cols = 4;
    m_state.enemies[0].m_animation_rows = 4;

    m_state.enemies[0].set_ai_type(BASIC);
    m_state.enemies[0].set_ai_state(WALKING);
    m_state.enemies[0].m_texture_id = Utility::load_texture("assets/images/enemy.png");
    m_state.enemies[0].set_position(glm::vec3(10.5f, -5.1f, 0.0f));
    m_state.enemies[0].set_speed(3.0f);
    m_state.enemies[0].set_movement(glm::vec3(1.0f, 0.0f, 0.0f));
    m_state.enemies[0].set_width(0.3f);
    m_state.enemies[0].set_height(0.8f);


    //enemy 2
    m_state.enemies[1].m_walking[m_state.enemies[1].LEFT] = new int[4] { 1, 5, 9, 13 };
    m_state.enemies[1].m_walking[m_state.enemies[1].RIGHT] = new int[4] { 3, 7, 11, 15 };
    m_state.enemies[1].m_walking[m_state.enemies[1].UP] = new int[4] { 2, 6, 10, 14 };
    m_state.enemies[1].m_walking[m_state.enemies[1].DOWN] = new int[4] { 0, 4, 8, 12 };
    m_state.enemies[1].m_animation_indices = m_state.enemies[1].m_walking[m_state.enemies[1].RIGHT];
    m_state.enemies[1].m_animation_frames = 4;
    m_state.enemies[1].m_animation_index = 0;
    m_state.enemies[1].m_animation_time = 0.0f;
    m_state.enemies[1].m_animation_cols = 4;
    m_state.enemies[1].m_animation_rows = 4;

    m_state.enemies[1].set_ai_type(BASIC);
    m_state.enemies[1].set_ai_state(WALKING);
    m_state.enemies[1].m_texture_id = Utility::load_texture("assets/images/enemy.png");
    m_state.enemies[1].set_position(glm::vec3(28.2f, -4.1f, 0.0f));
    m_state.enemies[1].set_speed(1.0f);
    m_state.enemies[1].set_movement(glm::vec3(1.0f, 0.0f, 0.0f));
    m_state.enemies[1].set_width(0.3f);
    m_state.enemies[1].set_height(0.8f);


    //Goal
    m_state.goal = new Goal();
    m_state.goal->m_walking[m_state.goal->LEFT] = new int[4] { 1, 5, 9, 13 };
    m_state.goal->m_walking[m_state.goal->RIGHT] = new int[4] { 3, 7, 11, 15 };
    m_state.goal->m_walking[m_state.goal->UP] = new int[4] { 2, 6, 10, 14 };
    m_state.goal->m_walking[m_state.goal->DOWN] = new int[4] { 0, 4, 8, 12 };
    m_state.goal->m_animation_frames = 4;
    m_state.goal->m_animation_index = 0;
    m_state.goal->m_animation_time = 0.0f;
    m_state.goal->m_animation_cols = 4;
    m_state.goal->m_animation_rows = 4;
    m_state.goal->m_texture_id = Utility::load_texture("assets/images/cat.png");
    m_state.goal->set_position(glm::vec3(30.0f, 0.9f, 0.0f));
    m_state.goal->set_speed(0.0f);
    m_state.goal->set_movement(glm::vec3(1.0f, 0.0f, 0.0f));
    m_state.goal->m_animation_indices = m_state.goal->m_walking[m_state.goal->RIGHT];
    m_state.enemies->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));


    m_state.jump_sfx = Mix_LoadWAV("assets/audio/woop.wav");
}


void LevelB::update(float delta_time)
{
    m_state.player->update(delta_time, m_state.player, m_state.enemies, ENEMY_COUNT, m_state.map);
    m_state.goal->update(delta_time, m_state.player, m_state.enemies, ENEMY_COUNT, m_state.map);

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_state.enemies[i].update(delta_time, m_state.player, 0, 0, m_state.map);
    }

    if (m_state.goal->level_win()) {
        m_state.next_scene_id = 3; //go to level C
    }
}


void LevelB::render(ShaderProgram* program)
{
    m_state.map->render(program);
    m_state.player->render(program);
    m_state.goal->render(program);
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_state.enemies[i].render(program);
    }
}

