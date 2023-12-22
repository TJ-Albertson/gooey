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
unsigned int focused_window_index;

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

        if(!exist){
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

void gooey_window_collision(Vector2D point)
{
    int index = 0;
    int count = 0;
    for (count = 0; count < MAX_WINDOWS; ++count)
    {
        int exist = (window_state >> count) & 1;

        if(!exist)
        {
            continue;
        }

        if (windows[i].min.x <= point.x && point.x <= windows[i].max.x && windows[i].min.y <= point.y && point.y <= windows[i].max.y)
        {
            focused_window_index = i;
            return;
        }
    }
}

#endif