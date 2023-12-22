#ifndef GOOEY_WINDOWS_H
#define GOOEY_WINDOWS_H

#include <my_math/vector.h>

typedef struct {
    Vector2D min;
    Vector2D max;
} Window;

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

int gooey_window_create(Vector2D min, Vector2D max)
{   
    print_binary(window_state, 16);

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

            return i;
        }
    }

    
    return -1;
}

void gooey_window_destroy(int index)
{

}

void gooey_window_draw(unsigned int shader_id, int SCREEN_HEIGHT, unsigned int VAO, unsigned int VBO)
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
        Vector4D color = {0.2, 0.2, 0.7, 0.5};

        if(focused_window_index == i)
        {
            color.w = 0.8; 
        }

        glUseProgram(shader_id);
        glUniform4f(glGetUniformLocation(shader_id, "color"), color.x, color.y, color.z, color.w);
        glBindVertexArray(VAO);
        
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

        glBindVertexArray(0);
    }
}

void gooey_window_collision(Vector2D point)
{
    print_binary(window_state, 16);

    int i;
    for (i = 0; i < MAX_WINDOWS; ++i)
    {
        int exist = (window_state >> i) & 1;

        if(!exist)
        {
            continue;
        }

        printf("point: %f %f\n", point.x, point.y);

        if ( point.x >= windows[i].min.x && point.x <= windows[i].max.x && point.y >= windows[i].min.y && point.y <= windows[i].max.y)
        {
            focused_window_index = i;
            return;
        }
    }

    focused_window_index = -1;
}

#endif