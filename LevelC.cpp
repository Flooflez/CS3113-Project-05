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

#include "LevelC.h"
#include "Utility.h"

#define LEVEL_WIDTH 50
#define LEVEL_HEIGHT 10

unsigned int LEVELC_DATA[] =
{
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 7, 6, 0, 0, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0,
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 9, 0, 0, 1, 0, 1, 0, 6, 7, 7, 7, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0,
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 7, 6, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0,
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 9, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 6, 7, 7, 6, 9, 0, 0, 0, 6, 7, 6, 0, 0, 9, 9, 0, 8,
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 7, 6, 1, 0, 1, 0, 6, 7, 6, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 9, 9, 0, 1, 0, 0, 1, 9, 9, 0, 0, 1, 9, 1, 0, 6, 7, 6, 0, 8,
    8, 0, 0, 0, 0, 0, 0, 0, 0, 9, 1, 0, 1, 9, 0, 1, 0, 1, 0, 1, 0, 9, 0, 0, 0, 1, 0, 0, 3, 3, 3, 3, 3, 1, 0, 0, 1, 6, 7, 6, 0, 1, 9, 1, 0, 1, 0, 1, 0, 8,
    8, 0, 0, 0, 0, 0, 0, 0, 5, 5, 1, 0, 1, 9, 9, 1, 0, 1, 0, 1, 9, 9, 9, 0, 0, 1, 0, 0, 2, 4, 2, 4, 2, 1, 0, 0, 1, 1, 0, 1, 0, 9, 9, 1, 0, 1, 0, 1, 0, 8,
    8, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 1, 5, 5, 5, 5, 5, 5, 0, 1, 0, 0, 2, 4, 2, 4, 2, 1, 0, 0, 9, 1, 0, 3, 3, 3, 3, 3, 0, 9, 9, 1, 0, 8,
    8, 3, 3, 3, 3, 3, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 2, 2, 2, 2, 2, 1, 5, 5, 5, 5, 0, 2, 4, 2, 4, 2, 0, 9, 9, 9, 0, 8,
    8, 2, 4, 2, 4, 2, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 2, 4, 4, 2, 4, 4, 2, 4, 4, 2, 0, 2, 4, 2, 4, 2, 3, 3, 3, 3, 3, 0, 2, 4, 2, 4, 2, 0, 3, 3, 3, 3, 3,
};

LevelC::~LevelC()
{
    delete[] m_state.enemies;
    delete    m_state.player;
    delete    m_state.map;
    Mix_FreeChunk(m_state.jump_sfx);
    Mix_FreeMusic(m_state.bgm);
}

void LevelC::initialise()
{
    GLuint map_texture_id = Utility::load_texture("assets/images/platforms.png");
    m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELC_DATA, map_texture_id, 1.0f, 10, 1);

    // Code from main.cpp's initialise()
    /**
     George's Stuff
     */
     // Existing
    m_state.player = new Player();
    m_state.player->set_position(glm::vec3(5.0f, -2.0f, 0.0f));
    m_state.player->set_movement(glm::vec3(0.0f));
    m_state.player->set_speed(2.5f);
    m_state.player->set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    m_state.player->m_texture_id = Utility::load_texture("assets/images/player.png");

    // Walking
    m_state.player->m_walking[m_state.player->LEFT] = new int[4] { 1, 5, 9, 13 };
    m_state.player->m_walking[m_state.player->RIGHT] = new int[4] { 3, 7, 11, 15 };
    m_state.player->m_walking[m_state.player->UP] = new int[4] { 2, 6, 10, 14 };
    m_state.player->m_walking[m_state.player->DOWN] = new int[4] { 0, 4, 8, 12 };

    m_state.player->m_animation_indices = m_state.player->m_walking[m_state.player->RIGHT];  // start George looking left
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

    //Goal
    m_state.goal = new Goal();
    m_state.goal->m_texture_id = Utility::load_texture("assets/images/shooter.png");
    m_state.goal->set_position(glm::vec3(7.0f, -1.1f, 0.0f));


    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    m_state.bgm = Mix_LoadMUS("assets/audio/badsong.wav");
    Mix_PlayMusic(m_state.bgm, -1);
    //Mix_VolumeMusic(2.3f);
    Mix_VolumeMusic(0.0f);

    m_state.jump_sfx = Mix_LoadWAV("assets/audio/woop.wav");
}


void LevelC::update(float delta_time)
{
    m_state.player->update(delta_time, m_state.player, m_state.enemies, ENEMY_COUNT, m_state.map);
    m_state.goal->update(delta_time, m_state.player, m_state.enemies, ENEMY_COUNT, m_state.map);

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_state.enemies[i].update(delta_time, m_state.player, 0, 0, m_state.map);
    }

    if (m_state.goal->level_win()) {
        m_state.next_scene_id = 1; //go to level B
    }
}


void LevelC::render(ShaderProgram* program)
{
    m_state.map->render(program);
    m_state.player->render(program);
    m_state.goal->render(program);
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_state.enemies[i].render(program);
    }
}
