#ifndef GOOEY_WIDGETS_H
#define GOOEY_WIDGETS_H

#include <my_math/vector.h>
#include <gooey_state.h>
#include <gooey_windows.h>

typedef enum {
    TOGGLE,
    RESIZE,
    SLIDER,
    CLOSE,
    HIDE
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


int gooey_button_create(float x, float y, float scale, int window_index, ButtonType type)
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
            buttons[i].type = type;
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

        if (button.type == RESIZE)
        {
            min.x = button.x + windows[button.window_index].max.x;
            min.y = button.y + SCREEN_HEIGHT - windows[button.window_index].max.y;
        } 
        else if (button.type == CLOSE) 
        {
            min.x = button.x + windows[button.window_index].max.x;
            min.y = button.y + SCREEN_HEIGHT - windows[button.window_index].min.y;
        } 
        else 
        {
            min.x = button.x + windows[button.window_index].min.x;
            min.y = button.y + SCREEN_HEIGHT - windows[button.window_index].min.y;
        }
        
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

        if (button.type == RESIZE)
        {
            min.x = button.x + windows[button.window_index].max.x;
            max.y = windows[button.window_index].max.y - button.y;
        } 
        else if (button.type == CLOSE) 
        {
            min.x = button.x + windows[button.window_index].max.x;
            max.y = windows[button.window_index].min.y - button.y;
        } 
        else 
        {
            min.x = button.x + windows[button.window_index].min.x;
            max.y = windows[button.window_index].min.y - button.y;  
        }

        max.x = min.x + scale;
        min.y = max.y - scale;

        if ( point.x >= min.x && point.x <= max.x && point.y >= min.y && point.y <= max.y)
        {
            if(button.type == HIDE)
            {
                windows[button.window_index].max.y = windows[button.window_index].min.y + 60;
            }
            else if(button.type == CLOSE)
            {
                window_state |= (0 << button.window_index);
            }
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
        int index = buttons[held_button_index].window_index;

        Vector2D max = windows[index].max;
        Vector2D min = windows[index].min;

        int str_length = 60 + get_string_length(windows[index].title) * 20;

        max.x += mouse_offset.x;
        if(max.x - min.x >= str_length)
        {
            windows[index].max.x += mouse_offset.x;
        }

        
        max.y += mouse_offset.y;      
        if(max.y - min.y >= 60)
        {
            windows[index].max.y += mouse_offset.y;
        }
    }
    else if(held_button_index >= 0 && buttons[held_button_index].type == HIDE)
    {
        
    }
}


void gooey_widget_destroy()
{

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