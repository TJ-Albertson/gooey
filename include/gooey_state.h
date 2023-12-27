#ifndef GOOEY_STATE_H
#define GOOEY_STATE_H

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "my_math/matrix.h"
#include "shader.h"

unsigned int SCREEN_WIDTH = 640;
unsigned int SCREEN_HEIGHT = 480;

unsigned int window_shader, text_shader;
unsigned int VAO, VBO;

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

    return 1;
}


#endif