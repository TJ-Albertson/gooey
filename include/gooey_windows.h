#ifndef GOOEY_WINDOWS_H
#define GOOEY_WINDOWS_H

#include <my_math/vector.h>
#include <gooey_state.h>
#include <gooey_widgets.h>



int get_string_length(const char str[]) {
    int length = 0;

    while (str[length] != '\0') {
        length++;
    }

    return length;
}


void print_binary(int data, int size) {
    int i;
    for (i = size - 1; i >= 0; i--) {
        int bit = (data >> i) & 1;
        printf("%d", bit);
    }
    printf("\n");
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
            int str_length = 120 + get_string_length(title) * 20;

            Vector2D m = { min.x + str_length, min.y + str_length};

            window_state |= (1 << i);
            windows[i].min = min;
            windows[i].max = m;
            strcpy(windows[i].title, title);

            gooey_button_create(-30, 10, 20, i, RESIZE);
            /*gooey_button_create(0, 0, 30, i, CLOSE);*/
            gooey_button_create(5, -25, 20, i, HIDE);

            return i;
        }
    }

    return -1;
}

void gooey_window_draw(GooeyColor window_color)
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

        float handle_alpha = 0.4f;
        float window_alpha = 0.6f; 

        if(focused_window_index == i)
        {
            handle_alpha = 0.8f;
            window_alpha = 0.7f; 
        }

        glUseProgram(window_shader);
        glUniform4f(glGetUniformLocation(window_shader, "color"), color.x, color.y, color.z, window_alpha);
        glBindVertexArray(VAO);

        Vector2D min = window.min;
        Vector2D max = window.max;
        
        window.max.y = SCREEN_HEIGHT - window.max.y + 20;
        window.min.y = SCREEN_HEIGHT - window.min.y;

        float vertices[6][4] = {
            { window.max.x, window.min.y - 20, 0.0f, 1.0f },
            { window.min.x, window.min.y - 20, 0.0f, 0.0f },  
            { window.max.x, window.max.y, 1.0f, 1.0f },

            { window.min.x, window.min.y - 20, 1.0f, 1.0f },
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
        /*glDrawArrays(GL_TRIANGLES, 0, 6);*/

        Vector3D white = {1.0, 1.0, 1.0};

        glBindVertexArray(0);

        glUseProgram(vector_shader);
        Mat4 model; 
        clear_matrix(&model);
        
       
        /* glDisable(GL_BLEND);
            translateMat4(&model, window.min.x, window.min.y + 30, 0);
            scaleMat4(&model, 0.02 * (window.max.x - window.min.x), 5, 1.0f);
        */

        glUniform4f(glGetUniformLocation(window_shader, "color"), color.x, color.y, color.z, handle_alpha);

        int scale = 20;
        int width, height, x, y;


        glDisable(GL_BLEND);

        /* bottom right top left corner */
        width = 1;
        height = 1;
        x = 2;
        y = -2;
        clear_matrix(&model);
        setShaderBool(vector_shader, "convex", 1);
         /* Special Case: Bottom Right */
        translateMat4(&model, window.max.x - (scale * x), SCREEN_HEIGHT - max.y - (scale * y), 0);

        scaleMat4(&model, scale * width, scale * height, 1.0f);
        /* setShaderVec4\(vector_shader, "color", 0\.0, 0\.0, 1\.0, 1\.0\); */
        setShaderMat4(vector_shader, "model", &model);
        wavefront_draw(vector_shader, top_left_corner);

        /* bottom right square */
        width = 1;
        height = 1;
        x = 1;
        y = -2;
        clear_matrix(&model);
        setShaderBool(vector_shader, "convex", 0);
         /* Special Case: Bottom Right */
        translateMat4(&model, window.max.x - (scale * x), SCREEN_HEIGHT - max.y - (scale * y), 0);

        scaleMat4(&model, scale * width, scale * height, 1.0f);
        /*setShaderVec4\(vector_shader, "color", 1\.0, 0\.0, 1\.0, 1\.0\);*/
        setShaderMat4(vector_shader, "model", &model);
        wavefront_draw(vector_shader, square);


        /* Text Box */
        int text_width = get_string_length(window.title);
        height = 2;
        x = 1;
        y = 0;
        clear_matrix(&model);
        setShaderBool(vector_shader, "convex", 0);
        translateMat4(&model, window.min.x  + (scale * x), window.min.y - (scale * y), 0);
        scaleMat4(&model, scale * text_width, scale * height, 1.0f);
        setShaderMat4(vector_shader, "model", &model);
        wavefront_draw(vector_shader, square);

        /* top inner curver*/
        width = 1;
        height = 1;
        x = text_width + 1;
        y = 1;
        clear_matrix(&model);
        setShaderBool(vector_shader, "convex", 0);
        translateMat4(&model, window.min.x  + (scale * x), window.min.y - (scale * y), 0);
        scaleMat4(&model, scale * width, scale * height, 1.0f);
        /* setShaderVec4\(vector_shader, "color", 0\.0, 1\.0, 0\.0, 1\.0\);*/
        setShaderMat4(vector_shader, "model", &model);
        wavefront_draw(vector_shader, top_left_corner);


        /* top left square */
        width = 1;
        height = 1;
        x = 0;
        y = 1;
        clear_matrix(&model);
        translateMat4(&model, window.min.x + (scale * x), window.min.y - (scale * y), 0);
        scaleMat4(&model, scale * width, scale * height, 1.0f);
        setShaderMat4(vector_shader, "model", &model);
        /* setShaderVec4\(vector_shader, "color", 0\.0, 1\.0, 0\.0, 1\.0\);*/
        setShaderBool(vector_shader, "convex", 0);
        wavefront_draw(vector_shader, square);

        glUniform4f(glGetUniformLocation(window_shader, "color"), color.x, color.y, color.z, 0.8);

         /* top left corner */
        width = 1;
        height = 1;
        x = 4;
        y = 1;
        clear_matrix(&model);
        setShaderBool(vector_shader, "convex", 1);
        translateMat4(&model, window.min.x, window.min.y, 0);
        scaleMat4(&model, scale * width, scale * height, 1.0f);
        setShaderMat4(vector_shader, "model", &model); 
        wavefront_draw(vector_shader, top_left_corner);

        /* top stretch*/
        width = 1;
        height = 1;
        x = text_width + 1;
        y = 0;
        clear_matrix(&model);
        setShaderBool(vector_shader, "convex", 0);
        translateMat4(&model, window.min.x  + (scale * x), window.min.y - (scale * y), 0);
        scaleMat4(&model, scale * width + (window.max.x - window.min.x) - text_width * 20 - 60, scale * height, 1.0f);
        setShaderMat4(vector_shader, "model", &model);
        wavefront_draw(vector_shader, square);

        /* top right */
        width = 1;
        height = 1;
        x = 1;
        y = 0;
        clear_matrix(&model);
        setShaderBool(vector_shader, "convex", 1);
        /* Special Case: Top Right */
        translateMat4(&model, window.max.x - (scale * x), window.min.y - (scale * y), 0);

        scaleMat4(&model, scale * width, scale * height, 1.0f);
        /* setShaderVec4\(vector_shader, "color", 0\.0, 0\.0, 1\.0, 1\.0\); */
        setShaderMat4(vector_shader, "model", &model);
        wavefront_draw(vector_shader, top_right_corner);

        /*bottom left */
        width = 1;
        height = 1;
        x = 0;
        y = -1;
        clear_matrix(&model);
        setShaderBool(vector_shader, "convex", 1);
        /* Special Case: Bottom Left */
        translateMat4(&model, window.min.x + (scale * x), SCREEN_HEIGHT - max.y - (scale * y), 0);
        scaleMat4(&model, scale * width, scale * height, 1.0f);
        /*setShaderVec4\(vector_shader, "color", 0\.0, 1\.0, 1\.0, 1\.0\);;*/
        setShaderMat4(vector_shader, "model", &model);
        wavefront_draw(vector_shader, bottom_left_corner);

        /* Bottom bar*/
        width = 1;
        height = 1;
        x = 1;
        y = -1;
        clear_matrix(&model);
        setShaderBool(vector_shader, "convex", 0);
        /* Special Case: Bottom Left */
        translateMat4(&model, window.min.x + (scale * x), SCREEN_HEIGHT - max.y - (scale * y), 0);

        scaleMat4(&model, scale * width + (window.max.x - window.min.x) - 60, scale * height, 1.0f);
        /*setShaderVec4\(vector_shader, "color", 1\.0, 0\.0, 1\.0, 1\.0\);*/
        setShaderMat4(vector_shader, "model", &model);
        wavefront_draw(vector_shader, square);


        /* bottom righ */
        width = 1;
        height = 1;
        x = 1;
        y = -1;
        clear_matrix(&model);
        setShaderBool(vector_shader, "convex", 1);
         /* Special Case: Bottom Right */
        translateMat4(&model, window.max.x - (scale * x), SCREEN_HEIGHT - max.y - (scale * y), 0);

        scaleMat4(&model, scale * width, scale * height, 1.0f);
        /* setShaderVec4\(vector_shader, "color", 0\.0, 0\.0, 1\.0, 1\.0\); */
        setShaderMat4(vector_shader, "model", &model);
        wavefront_draw(vector_shader, bottom_right_corner);

        glEnable(GL_BLEND);

        gooey_text(window.title, window.min.x + 30, SCREEN_HEIGHT - window.min.y + 25, 0.45, white);
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