#ifndef GOOEY_STATE_H
#define GOOEY_STATE_H

#include <my_math/vector.h>

typedef struct {
    Vector2D min, max;
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

short buttons_state = 0;
const unsigned int MAX_BUTTONS = 16;
Button buttons[16];


void gooey_button_create(float x, float y, float scale, Vector3D color)
{

}

void gooey_button_collision()
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