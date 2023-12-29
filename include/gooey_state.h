#ifndef GOOEY_STATE_H
#define GOOEY_STATE_H

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "my_math/matrix.h"
#include "shader.h"
#include <wavefront.h>

unsigned int SCREEN_WIDTH = 640;
unsigned int SCREEN_HEIGHT = 480;

unsigned int window_shader, text_shader, vector_shader;
unsigned int VAO, VBO;
WaveFront wave_test, corner, right_corner;

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



int gooey_initialize() {

    window_shader = createShader("resources/shaders/window.vs", "resources/shaders/window.fs");
    text_shader = createShader("resources/shaders/text.vs", "resources/shaders/text.fs");
    
    Mat4* projection = ortho(0.0f, SCREEN_WIDTH, 0.0f, SCREEN_HEIGHT, 0.0f, 100.0f);
    glUseProgram(text_shader);
    setShaderMat4(text_shader, "projection", projection);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    vector_shader = createShader("resources/shaders/basic.vs", "resources/shaders/vector.fs");
    wave_test = load_wave("./resources/models/untitled2.obj");
    corner = load_wave("./resources/models/untitled3.obj");
    right_corner = load_wave("./resources/models/right_corner.obj");

    return 1;
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