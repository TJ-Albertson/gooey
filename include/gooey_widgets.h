#ifndef GOOEY_WIDGETS_H
#define GOOEY_WIDGETS_H

#include <my_math/vector.h>
#include <gooey_state.h>
#include <gooey_windows.h>

typedef enum {
    TOGGLE,
    RESIZE,
    SLIDER,
} ButtonType;

typedef struct {
    float x, y, scale;
    int window_index;
    ButtonType type;
} Button;

typedef struct {
    Vector2D min, max;

    union Value {
        int integer;
        float floating;
    };

} Slider;

int focused_slider_index = -1;
short slider_state = 0;
const unsigned int MAX_SLIDERS = 16;
Slider sliders[16];

int held_button_index = -1;
short buttons_state = 0;
const unsigned int MAX_BUTTONS = 16;
Button buttons[16];


int gooey_button_create(float x, float y, float scale, int window_index)
{
    /* find first empty index */
    int i;
    for (i = 0; i < MAX_BUTTONS; ++i)
    {
        int exist = (buttons_state >> i) & 1;

        if(!exist)
        {
            buttons_state |= (1 << i);
            buttons[i].x = x;
            buttons[i].y = y;
            buttons[i].scale = scale;
            buttons[i].window_index = window_index;
            buttons[i].type = RESIZE;
            return i;
        }
    }

    return -1;
}

void gooey_button_draw()
{
    int i;
    for(i = 0; i < MAX_BUTTONS; ++i)
    {
        int exist = (buttons_state >> i) & 1;

        if(!exist)
        {
            continue;
        }

        Vector4D color = gooey_color_to_vector3(BLUE);

        float button_alpha = 0.3f;
        if(held_button_index == i)
        {
            button_alpha = 1.0f;
            color.x = 1.0f;
            color.y = 0.0f;
        }

        glUseProgram(window_shader);
        glUniform4f(glGetUniformLocation(window_shader, "color"), color.x, color.y, color.z, button_alpha);
        glBindVertexArray(VAO);

        Button button = buttons[i];
        float scale = button.scale;

        Vector2D min;
        min.x = button.x + windows[button.window_index].min.x;
        min.y = button.y + SCREEN_HEIGHT - windows[button.window_index].min.y;

        Vector2D max;
        max.x = min.x + scale;
        max.y = min.y + scale;

        float vertices[6][4] = {
            { min.x, min.y, 0.0f, 1.0f },
            { max.x, min.y, 0.0f, 0.0f },  
            { max.x, max.y, 1.0f, 1.0f },

            { min.x, min.y, 1.0f, 1.0f },
            { max.x, max.y, 0.0f, 0.0f },      
            { min.x, max.y, 1.0f, 0.0f }
        };

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); /* be sure to use glBufferSubData and not glBufferData */

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

void gooey_button_collision(Vector2D point)
{
    int i;
    for (i = 0; i < MAX_BUTTONS; ++i)
    {
        int exist = (buttons_state >> i) & 1;

        if(!exist)
        {
            continue;
        }

        Button button = buttons[i];
        float scale = button.scale;

        Vector2D min;
         Vector2D max;

        min.x = button.x + windows[button.window_index].min.x;
        max.y = windows[button.window_index].min.y - button.y;

        max.x = min.x + scale;
        min.y = max.y - scale;

        printf("mousepos: %f %f\n", point.x, point.y);
        printf("min: %f %f\n", min.x, min.y);
        printf("max: %f %F\n", max.x, max.y);

        if ( point.x >= min.x && point.x <= max.x && point.y >= min.y && point.y <= max.y)
        {
            held_button_index = i;
            return;
        }
    }

    held_button_index = -1;

}

void gooey_button_resize(Vector2D mouse_offset)
{
    if(held_button_index >= 0 && buttons[held_button_index].type == RESIZE)
    {   
        windows[buttons[held_button_index].window_index].max.x += mouse_offset.x;
        windows[buttons[held_button_index].window_index].max.y += mouse_offset.y;
    }
}





void gooey_slider() 
{

}

void gooey_slider_move(Vector2D mouse_offset)
{
    if(focused_slider_index >= 0)
    {   
        sliders[focused_slider_index].min.x += mouse_offset.x;
        sliders[focused_slider_index].max.x += mouse_offset.x;

        sliders[focused_slider_index].min.y += mouse_offset.y;
        sliders[focused_slider_index].max.y += mouse_offset.y;
    }
}

#endif