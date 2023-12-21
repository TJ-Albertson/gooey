#ifndef GOOEY_WINDOWS_H
#define GOOEY_WINDOWS_H

#include <my_math/vector.h>

typedef struct {
    Vector2D min;
    Vector2D max;
} Window;

const unsigned int MAX_WINDOWS = 10;
Window windows[10];

unsigned int num_windows;
unsigned int focused_window_index;

void gooey_window_collision(Vector2D point)
{
    int i;
    for (i = 0; i < num_windows; ++i)
    {
        if (windows[i].min.x <= point.x && point.x <= windows[i].max.x && windows[i].min.y <= point.y && point.y <= windows[i].max.y)
        {
            focused_window_index = i;
            return;
        }
    }
}

#endif