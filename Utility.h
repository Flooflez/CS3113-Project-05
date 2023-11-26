#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

class Utility {
public:
    //
    static ShaderProgram* program;

    // ————— METHODS ————— //
    static GLuint load_texture(const char* filepath);
    static void draw_text(GLuint font_texture_id, std::string text, float screen_size, float spacing, glm::vec3 position);

    static void set_shader_program(ShaderProgram* new_program) { program = new_program; }
};
