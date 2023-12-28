#ifndef GOOEY_WINDOWS_H
#define GOOEY_WINDOWS_H

#include <my_math/vector.h>
#include <gooey_state.h>

typedef struct {
    Vector2D min;
    Vector2D max;
    char title[64];
} Window;

typedef enum {
    BLUE,
    RED
} WindowColor;

short window_state = 0;
const unsigned int MAX_WINDOWS = 16;
Window windows[16];

unsigned int num_windows;
unsigned int focused_window_index = -1;

void print_binary(int data, int size) {
    int i;
    for (i = size - 1; i >= 0; i--) {
        int bit = (data >> i) & 1;
        printf("%d", bit);
    }
    printf("\n");
}

Vector4D gooey_color_to_vector3(WindowColor color)
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

int gooey_window_create(char* title, Vector2D min, Vector2D max)
{   
    /* find first empty index */
    int i;
    for (i = 0; i < MAX_WINDOWS; ++i)
    {
        int exist = (window_state >> i) & 1;

        if(!exist)
        {
            window_state |= (1 << i);
            windows[i].min = min;
            windows[i].max = max;
            strcpy(windows[i].title, title);

            return i;
        }
    }

    return -1;
}

void gooey_window_destroy(int index)
{

}

void gooey_window_draw(WindowColor window_color)
{

    int i;
    for(i = 0; i < MAX_WINDOWS; ++i)
    {
        int exist = (window_state >> i) & 1;

        if(!exist)
        {
            continue;
        }

        Window window = windows[i];
        /* Vector4D color = {0.2, 0.2, 0.7, 0.5}; */
        Vector4D color = gooey_color_to_vector3(window_color);

        float handle_alpha = 0.6f;
        float window_alpha = 0.6f; 

        if(focused_window_index == i)
        {
            handle_alpha = 0.8f;
            window_alpha = 0.65f; 
        }

        glUseProgram(window_shader);
        glUniform4f(glGetUniformLocation(window_shader, "color"), color.x, color.y, color.z, window_alpha);
        glBindVertexArray(VAO);

        Vector2D min = window.min;
        Vector2D max = window.max;
        
        window.max.y = SCREEN_HEIGHT - window.max.y;
        window.min.y = SCREEN_HEIGHT - window.min.y;

        float vertices[6][4] = {
            { window.max.x, window.min.y, 0.0f, 1.0f },
            { window.min.x, window.min.y, 0.0f, 0.0f },  
            { window.max.x, window.max.y, 1.0f, 1.0f },

            { window.min.x, window.min.y, 1.0f, 1.0f },
            { window.min.x, window.max.y, 0.0f, 0.0f },      
            { window.max.x, window.max.y, 1.0f, 0.0f }
        };

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); /* be sure to use glBufferSubData and not glBufferData */

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glUniform4f(glGetUniformLocation(window_shader, "color"), color.x, color.y, color.z, handle_alpha);

        window.max.y = window.min.y - 20;

        float grab_tab[6][4] = {
            { window.max.x, window.min.y, 0.0f, 1.0f },
            { window.min.x, window.min.y, 0.0f, 0.0f },  
            { window.max.x, window.max.y, 1.0f, 1.0f },

            { window.min.x, window.min.y, 1.0f, 1.0f },
            { window.min.x, window.max.y, 0.0f, 0.0f },      
            { window.max.x, window.max.y, 1.0f, 0.0f }
        };

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(grab_tab), grab_tab); /* be sure to use glBufferSubData and not glBufferData */

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        Vector3D white = {1.0, 1.0, 1.0};

        glBindVertexArray(0);

        gooey_text(window.title, window.min.x + 5, SCREEN_HEIGHT - window.min.y + 18, 0.45, white);
    }
}

void gooey_window_collision(Vector2D point)
{
    int i;
    for (i = 0; i < MAX_WINDOWS; ++i)
    {
        int exist = (window_state >> i) & 1;

        if(!exist)
        {
            continue;
        }

        Vector2D min = windows[i].min;
        Vector2D max = windows[i].max;

        max.y = min.y + 20;

        if ( point.x >= min.x && point.x <= max.x && point.y >= min.y && point.y <= max.y)
        {
            focused_window_index = i;
            return;
        }
    }

    focused_window_index = -1;
}

void gooey_window_move(Vector2D mouse_offset)
{
    if(focused_window_index >= 0)
    {   
        windows[focused_window_index].min.x += mouse_offset.x;
        windows[focused_window_index].max.x += mouse_offset.x;

        windows[focused_window_index].min.y += mouse_offset.y;
        windows[focused_window_index].max.y += mouse_offset.y;
    }
}

#endif