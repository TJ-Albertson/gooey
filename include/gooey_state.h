#ifndef GOOEY_STATE_H
#define GOOEY_STATE_H

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "my_math/matrix.h"
#include "shader.h"
#include <wavefront.h>

unsigned int GOOEY_SCREEN_WIDTH = 640;
unsigned int GOOEY_SCREEN_HEIGHT = 480;

unsigned int gooey_window_shader, gooey_text_shader, gooey_vector_shader;
unsigned int VAO, VBO;
WaveFront wave_test, corner, right_corner, top_left_corner, top_right_corner, bottom_right_corner, bottom_left_corner, square;

typedef enum {
    BLUE,
    RED
} GooeyColor;


typedef struct {
    Vector2D min;
    Vector2D max;
    char title[64];
} Window;

short window_state = 0;
const unsigned int MAX_WINDOWS = 16;
Window windows[16];

unsigned int num_windows;
unsigned int focused_window_index = -1;

Mat4* gooey_projection_matrix;

int gooey_initialize(int width, int height) 
{
    gooey_window_shader = createShader("resources/shaders/window.vs", "resources/shaders/window.fs");
    gooey_text_shader = createShader("resources/shaders/text.vs", "resources/shaders/text.fs");
    
    gooey_projection_matrix = ortho(0.0f, width, 0.0f, height, 0.0f, 100.0f);
    glUseProgram(gooey_text_shader);
    setShaderMat4(gooey_text_shader, "projection", gooey_projection_matrix);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    gooey_vector_shader = createShader("resources/shaders/basic.vs", "resources/shaders/vector.fs");

    wave_test = load_wave("./resources/models/untitled2.obj");
    corner = load_wave("./resources/models/untitled3.obj");
    right_corner = load_wave("./resources/models/right_corner.obj");

    top_left_corner = load_wave("./resources/models/top_left_corner.obj");
    top_right_corner = load_wave("./resources/models/top_right_corner.obj");
    bottom_right_corner = load_wave("./resources/models/bottom_right_corner.obj");
    bottom_left_corner = load_wave("./resources/models/bottom_left_corner.obj");
    square = load_wave("./resources/models/square.obj");

    return 1;
}


void gooey_screen_resize(int x, int y)
{
    GOOEY_SCREEN_WIDTH = x;
    GOOEY_SCREEN_HEIGHT = y;
    gooey_projection_matrix = ortho(0.0f, x, 0.0f, y, 0.0f, 100.0f);
}






Vector4D gooey_color_to_vector3(GooeyColor color)
{
    float alpha = 0.6f;

    switch (color) {
        case BLUE: {
            Vector4D vector_color = {0.16078f, 0.45882f, 0.73725f, alpha};
            return vector_color;
        }
           
        case RED: {
            Vector4D vector_color = {0.0f, 0.0f, 1.0f, alpha};
            return vector_color;
        }
            
        default: {
            Vector4D vector_color = {0.0f, 0.0f, 1.0f, alpha};
            return vector_color;
        }
    }
}

#endif