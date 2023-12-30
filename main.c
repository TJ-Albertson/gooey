#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "stdio.h"

#include <shader.h>
#include <gooey.h>


float mouse_x, mouse_y, mouse_x_last, mouse_y_last;

float svg_scale = 200.0f;

void RenderBox(unsigned int shader_id, Window window, float scale, Vector4D color);

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    SCREEN_WIDTH = width;
    SCREEN_HEIGHT = height;
    glViewport(0, 0, width, height);
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    mouse_x = xpos;
    mouse_y = ypos;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) 
    {
        mouse_x_last = mouse_x;
        mouse_y_last = mouse_y;
        return;
    }

    float x_offset = mouse_x - mouse_x_last;
    float y_offset = mouse_y - mouse_y_last;

    Vector2D mouse_offset = { x_offset, y_offset };
    gooey_window_move(mouse_offset);
    gooey_button_resize(mouse_offset);

    mouse_x_last = mouse_x;
    mouse_y_last = mouse_y;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        Vector2D mouse_pos = { mouse_x, mouse_y };

        gooey_window_collision(mouse_pos);
        gooey_button_collision(mouse_pos);
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        held_button_index = -1;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    svg_scale += yoffset * 20;
}

int main()
{
    /* Initialize GLFW */
    if (!glfwInit())
    {
        fprintf(stderr, "ERROR::GLFW: Failed to initialize GLFW\n");
        return -1;
    }

    GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello World", NULL, NULL);
    if (!window)
    {
        fprintf(stderr, "ERROR::GLFW: Failed to open GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    /* callbacks */
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        fprintf(stderr, "ERROR::GLAD: Failed to initialize GLAD\n");
        return -1;
    }

    /* FreeType */
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        fprintf(stderr, "ERROR::FREETYPE: Could not init FreeType Library");
        return -1;
    }


    FT_Face face;
    if (FT_New_Face(ft, "resources/fonts/Fusion.ttf", 0, &face))
    {
        fprintf(stderr, "ERROR::FREETYPE: Failed to load font");
        return -1;
    } else {

        FT_Set_Pixel_Sizes(face, 0, 48);

        /* disable byte-alignment restriction */
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        unsigned char c;
        for (c = 0; c < 128; c++)
        {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                fprintf(stderr, "ERROR::FREETYTPE: Failed to load Glyph");
                continue;
            }

            /* generate texture */
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );

            /* set texture options */
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            Character character;
            character.TextureID = texture;

            character.Size.x = face->glyph->bitmap.width;
            character.Size.y = face->glyph->bitmap.rows;

            character.Bearing.x = face->glyph->bitmap_left;
            character.Bearing.y = face->glyph->bitmap_top;

            character.Advance = face->glyph->advance.x;

            /*
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                face->glyph->advance.x
            };

            Characters.insert(std::pair<char, Character>(c, character));
            */

            Characters[c] = character;
        }

    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    if (!gooey_initialize())
    {
        fprintf(stderr, "ERROR::GOOEY: Failed to initialize GOOEY\n");
        return -1;
    }

    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    /*
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    */    
   
    

    Vector3D color1 = {0.5, 0.8, 0.2};
    Vector3D color2 = {0.3, 0.7, 0.9};
    
    
    Vector4D window_color = {0.2, 0.2, 0.7, 0.5};


    Window window1;
    window1.min.x = 25.0f;
    window1.min.y = 25.0f;

    window1.max.x = 125.0f;
    window1.max.y = 125.0f;

    Vector2D min = {125, 125};
    Vector2D max = {275, 275};

    int window_index = gooey_window_create("test", min, max);
    printf("window_index: %d\n", window_index);


    Vector2D min2 = {300, 300};
    Vector2D max2 = {500, 500};
    /* TODO: Remove max arg. change to base on text length*/
    gooey_window_create("window t2", min2, max2);

    printf("window[%d].min.x: %.2f\n", window_index, windows[window_index].max.x);

    double lastTime = glfwGetTime();
    int frameCount = 0;

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        

        
        Mat4* projection = ortho(0.0f, SCREEN_WIDTH, 0.0f, SCREEN_HEIGHT, 0.0f, 100.0f);
        glUseProgram(vector_shader);
        setShaderMat4(vector_shader, "projection", projection);
        glUseProgram(text_shader);
        setShaderMat4(text_shader, "projection", projection);
        
        
        char mouse_pos_str[50];

        sprintf(mouse_pos_str, "X=%.2f, Y=%.2f", mouse_x, mouse_y);


        
        gooey_text(mouse_pos_str, 50.0f, 50.0f, 1.0f, color1);


        gooey_text("(C) LearnOpenGL.com", 420.0f, 400.0f, 0.5f, color2);

        glUseProgram(window_shader);
        setShaderMat4(window_shader, "projection", projection);
        gooey_window_draw(BLUE);
        gooey_button_draw();

        char fps_str[50];
        double currentTime = glfwGetTime();
        frameCount++;
        if (currentTime - lastTime >= 1.0) 
        {
            sprintf(fps_str, "FPS: %d", frameCount);
            
            frameCount = 0;
            lastTime += 1.0;
        }
        gooey_text(fps_str, SCREEN_WIDTH - 300.0f, 50.0f, 1.0f, color1);



        /*
        glUseProgram(vector_shader);
        Mat4 model; 
        clear_matrix(&model);
        translateMat4(&model, mouse_x, SCREEN_HEIGHT - mouse_y, 0);
        scaleMat4(&model, svg_scale, svg_scale, 1.0f);
        setShaderMat4(vector_shader, "model", &model);
        setShaderBool(vector_shader, "convex", 1);
        setShaderVec4(vector_shader, "color", 1.0, 0.0, 0.0, 1.0);
        wavefront_draw(vector_shader, top_left_corner);


        clear_matrix(&model);
        setShaderBool(vector_shader, "convex", 0);
        translateMat4(&model, mouse_x, SCREEN_HEIGHT - mouse_y - svg_scale, 0);
        scaleMat4(&model, svg_scale, svg_scale, 1.0f);
        setShaderVec4(vector_shader, "color", 0.0, 1.0, 0.0, 1.0);
        setShaderMat4(vector_shader, "model", &model);
        wavefront_draw(vector_shader, square);

        int width = 3;
        int height = 2;
        clear_matrix(&model);
        setShaderBool(vector_shader, "convex", 0);
        translateMat4(&model, mouse_x + svg_scale, SCREEN_HEIGHT - mouse_y, 0);
        scaleMat4(&model, svg_scale * width, svg_scale * height, 1.0f);
        setShaderVec4(vector_shader, "color", 0.0, 0.0, 1.0, 1.0);
        setShaderMat4(vector_shader, "model", &model);
        wavefront_draw(vector_shader, square);

        int x = 4;
        int y = 1;
        clear_matrix(&model);
        setShaderBool(vector_shader, "convex", 0);
        translateMat4(&model, mouse_x + svg_scale * 4, SCREEN_HEIGHT - mouse_y - svg_scale, 0);
        scaleMat4(&model, svg_scale, svg_scale, 1.0f);
        setShaderVec4(vector_shader, "color", 0.0, 1.0, 0.0, 1.0);
        setShaderMat4(vector_shader, "model", &model);
        wavefront_draw(vector_shader, top_left_corner);

        clear_matrix(&model);
        setShaderBool(vector_shader, "convex", 0);
        translateMat4(&model, mouse_x + svg_scale * 4, SCREEN_HEIGHT - mouse_y, 0);
        scaleMat4(&model, svg_scale * 5, svg_scale, 1.0f);
        setShaderVec4(vector_shader, "color", 1.0, 0.0, 0.0, 1.0);
        setShaderMat4(vector_shader, "model", &model);
        wavefront_draw(vector_shader, square);

        clear_matrix(&model);
        setShaderBool(vector_shader, "convex", 1);
        translateMat4(&model, mouse_x + svg_scale * 9, SCREEN_HEIGHT - mouse_y, 0);
        scaleMat4(&model, svg_scale, svg_scale, 1.0f);
        setShaderVec4(vector_shader, "color", 0.0, 0.0, 1.0, 1.0);
        setShaderMat4(vector_shader, "model", &model);
        wavefront_draw(vector_shader, top_right_corner);


        clear_matrix(&model);
        setShaderBool(vector_shader, "convex", 1);
        translateMat4(&model, mouse_x, SCREEN_HEIGHT - mouse_y - svg_scale * 5, 0);
        scaleMat4(&model, svg_scale, svg_scale, 1.0f);
        setShaderVec4(vector_shader, "color", 0.0, 0.0, 1.0, 1.0);
        setShaderMat4(vector_shader, "model", &model);
        wavefront_draw(vector_shader, bottom_left_corner);

        width = 8;
        height = 1;
        x = 1;
        y = 5;
        clear_matrix(&model);
        setShaderBool(vector_shader, "convex", 0);
        translateMat4(&model, mouse_x + (svg_scale * x), SCREEN_HEIGHT - mouse_y - (svg_scale * y), 0);
        scaleMat4(&model, svg_scale * width, svg_scale * height, 1.0f);
        setShaderVec4(vector_shader, "color", 1.0, 0.0, 0.0, 1.0);
        setShaderMat4(vector_shader, "model", &model);
        wavefront_draw(vector_shader, square);

        width = 1;
        height = 1;
        x = 9;
        y = 5;
        clear_matrix(&model);
        setShaderBool(vector_shader, "convex", 1);
        translateMat4(&model, mouse_x + (svg_scale * x), SCREEN_HEIGHT - mouse_y - (svg_scale * y), 0);
        scaleMat4(&model, svg_scale * width, svg_scale * height, 1.0f);
        setShaderVec4(vector_shader, "color", 0.0, 0.0, 1.0, 1.0);
        setShaderMat4(vector_shader, "model", &model);
        wavefront_draw(vector_shader, bottom_right_corner);

        width = 1;
        height = 1;
        x = 9;
        y = 4;
        clear_matrix(&model);
        setShaderBool(vector_shader, "convex", 0);
        translateMat4(&model, mouse_x + (svg_scale * x), SCREEN_HEIGHT - mouse_y - (svg_scale * y), 0);
        scaleMat4(&model, svg_scale * width, svg_scale * height, 1.0f);
        setShaderVec4(vector_shader, "color", 0.0, 1.0, 0.0, 1.0);
        setShaderMat4(vector_shader, "model", &model);
        wavefront_draw(vector_shader, square);

        width = 1;
        height = 1;
        x = 8;
        y = 4;
        clear_matrix(&model);
        setShaderBool(vector_shader, "convex", 1);
        translateMat4(&model, mouse_x + (svg_scale * x), SCREEN_HEIGHT - mouse_y - (svg_scale * y), 0);
        scaleMat4(&model, svg_scale * width, svg_scale * height, 1.0f);
        setShaderVec4(vector_shader, "color", 0.0, 0.0, 1.0, 1.0);
        setShaderMat4(vector_shader, "model", &model);
        wavefront_draw(vector_shader, top_left_corner);
        */
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}