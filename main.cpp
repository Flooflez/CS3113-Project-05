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

#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL_WIDTH 50
#define LEVEL_HEIGHT 10
#define LEVEL_LEFT_EDGE 5.0f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "LevelA.h"
#include "LevelB.h"
#include "LevelC.h"
#include "Menu.h"
#include "Win.h"

// ————— CONSTANTS ————— //
const int   WINDOW_WIDTH    = 640,
            WINDOW_HEIGHT   = 480;

const float BG_RED      = 0.09f,
            BG_BLUE     = 0.45f,
            BG_GREEN    = 0.20f,
            BG_OPACITY  = 1.0f;

const int   VIEWPORT_X = 0,
            VIEWPORT_Y = 0,
            VIEWPORT_WIDTH = WINDOW_WIDTH,
            VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char  V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
            F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

const char  FONT_FILEPATH[] = "assets/images/font1.png";

const float MILLISECONDS_IN_SECOND = 1000.0;

const std::string DEATH_MESSAGE = "YOU LOSE! BETTER LUCK NEXT TIME!";

// ————— GLOBAL VARIABLES ————— //
Scene*  g_current_scene;
int g_current_scene_index = 0;
Menu* g_menu;
LevelA* g_level_a;
LevelB* g_level_b;
LevelC* g_level_c;
Win* g_win_scene;

SDL_Window* g_display_window;
bool g_game_is_running = true;
bool g_game_over = false;
bool g_game_win = false;
bool g_game_start = false;
std::string g_display_message;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks  = 0.0f;
float g_accumulator     = 0.0f;

GLuint g_text_texture_id;

Scene* g_levels[5];

int g_lives = 3;

void lose_game() {
    g_game_over = true;
    g_display_message = DEATH_MESSAGE;
}



void switch_to_scene(Scene* scene)
{
    g_current_scene = scene;
    g_current_scene->initialise();
}

void initialise()
{
    // ————— VIDEO ————— //
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Stomp on 'em!",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif

    // ————— GENERAL ————— //
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    Utility::set_shader_program(&g_shader_program);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);

    // ————— LEVEL SETUP ————— //

    g_menu = new Menu();
    g_level_a = new LevelA();
    g_level_b = new LevelB();
    g_level_c = new LevelC();
    g_win_scene = new Win();

    g_levels[0] = g_menu;
    g_levels[1] = g_level_a;
    g_levels[2] = g_level_b;
    g_levels[3] = g_level_c;
    g_levels[4] = g_win_scene;

    // Start at menu
    switch_to_scene(g_levels[0]);

    Mix_PlayMusic(Mix_LoadMUS("assets/audio/slight_better_song.wav"), -1);
    Mix_VolumeMusic(3.0f);

    

    // ————— BLENDING ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ----- TEXT ----- //
    g_text_texture_id = Utility::load_texture(FONT_FILEPATH);
}

void process_input()
{
    g_current_scene->m_state.player->set_movement(glm::vec3(0.0f));

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        // ————— KEYSTROKES ————— //
        switch (event.type) {
            // ————— END GAME ————— //
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_game_is_running = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_q:
                // Quit the game with a keystroke
                g_game_is_running = false;
                break;

            case SDLK_SPACE:
                // ————— JUMPING ————— //
                if (g_current_scene->m_state.player->m_collided_bottom)
                {
                    g_current_scene->m_state.player->m_is_jumping = true;
                    Mix_PlayChannel(-1, g_current_scene->m_state.jump_sfx, 0);
                    Mix_Volume(-1, MIX_MAX_VOLUME / 15);
                }
                break;
            case SDLK_RETURN:
                //start game
                if (!g_game_start) {
                    g_game_start = true;
                    g_current_scene->m_state.next_scene_id = 1;
                }
                break;
            default:
                break;
            }

        default:
            break;
        }
    }

    // ————— KEY HOLD ————— //
    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_LEFT])
    {
        g_current_scene->m_state.player->move_left();
        g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_walking[g_current_scene->m_state.player->LEFT];
    }
    else if (key_state[SDL_SCANCODE_RIGHT])
    {
        g_current_scene->m_state.player->move_right();
        g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_walking[g_current_scene->m_state.player->RIGHT];
    }

    // ————— NORMALISATION ————— //
    if (glm::length(g_current_scene->m_state.player->get_movement()) > 1.0f)
    {
        g_current_scene->m_state.player->set_movement(glm::normalize(g_current_scene->m_state.player->get_movement()));
    }
}

void update()
{
    // ————— DELTA TIME / FIXED TIME STEP CALCULATION ————— //
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    delta_time += g_accumulator;

    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }

    while (delta_time >= FIXED_TIMESTEP) {
        // ————— UPDATING THE SCENE (i.e. map, character, enemies...) ————— //
        g_current_scene->update(FIXED_TIMESTEP);

        if (!g_current_scene->m_state.player->get_active()) {
            g_lives--;
            if (g_lives <= 0) {
                //game over!
                lose_game();
            }
            else {
                //restart level
                switch_to_scene(g_levels[g_current_scene_index]);
            }
        }
        else {
            if (g_current_scene->m_state.player->get_position().y < -10.5f) {
                g_current_scene->m_state.player->deactivate();
            }
        }

        delta_time -= FIXED_TIMESTEP;
    }

    g_accumulator = delta_time;


    // ————— PLAYER CAMERA ————— //
    g_view_matrix = glm::mat4(1.0f);

    float cam_x = -5;
    float cam_y = 5.75f;

    if (g_current_scene->get_state().player->get_position().x > LEVEL_LEFT_EDGE) {
        cam_x = -g_current_scene->m_state.player->get_position().x;
    }

    if (g_current_scene->get_state().player->get_position().y > -5.5f) {
        cam_y = -g_current_scene->get_state().player->get_position().y + 0.1f;
    }

    g_view_matrix = glm::translate(g_view_matrix, glm::vec3(cam_x, cam_y, 0));

}

void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);

    glClear(GL_COLOR_BUFFER_BIT);

    // ————— RENDERING THE SCENE (i.e. map, character, enemies...) ————— //
    g_current_scene->render(&g_shader_program);

    // ----- TEXT ----- //
    if (g_game_over) {
        float draw_x = 0.5167255f;
        if (g_current_scene->get_state().player->get_position().x > LEVEL_LEFT_EDGE) {
            draw_x = g_current_scene->m_state.player->get_position().x - 4.5f;
            
        }
        float draw_y = -5.0f;
        if (g_current_scene->get_state().player->get_position().y > -5.5f) {
            draw_y = -g_current_scene->get_state().player->get_position().y + 2.0f;
        }


        Utility::draw_text(g_text_texture_id, g_display_message, 0.4f, -0.1f, glm::vec3(draw_x, draw_y, 0));
        
    }

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();

    // ————— DELETING LEVEL A DATA (i.e. map, character, enemies...) ————— //
    delete g_level_a;
}

// ————— GAME LOOP ————— //
int main(int argc, char* argv[])
{
    initialise();

    while (g_game_is_running)
    {
        process_input();
        update();

        if (g_current_scene->m_state.next_scene_id >= 0) {
            g_current_scene_index = g_current_scene->m_state.next_scene_id;
            switch_to_scene(g_levels[g_current_scene_index]);
        }

        render();
    }

    shutdown();
    return 0;
}
