#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "stdio.h"

#include <shader.h>
#include <gooey.h>

typedef struct {
    unsigned int TextureID;
    Vector2D Size;
    Vector2D Bearing;
    unsigned int Advance;
} Character;

unsigned int SCREEN_WIDTH = 640;
unsigned int SCREEN_HEIGHT = 480;

float mouse_x, mouse_y;

unsigned int VAO, VBO;
Character Characters[128];

void RenderText(unsigned int shader_id, char *text, float x, float y, float scale, Vector3D color);
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
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        printf("left button click\n");

        Vector2D mouse_pos = { mouse_x, mouse_y };

        gooey_window_collision(mouse_pos);
    }
        
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

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        fprintf(stderr, "ERROR::GLAD: Failed to initialize GLAD\n");
        return -1;
    }

    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); */

    ShaderID window_shader = createShader("resources/shaders/window.vs", "resources/shaders/window.fs");
    ShaderID text_shader = createShader("resources/shaders/text.vs", "resources/shaders/text.fs");
    Mat4* projection = ortho(0.0f, SCREEN_WIDTH, 0.0f, SCREEN_HEIGHT, 0.0f, 100.0f);
    glUseProgram(text_shader);
    setShaderMat4(text_shader, "projection", projection);

    /* FreeType */
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        fprintf(stderr, "ERROR::FREETYPE: Could not init FreeType Library");
        return -1;
    }


    FT_Face face;
    if (FT_New_Face(ft, "resources/fonts/arial.ttf", 0, &face))
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

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

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

    int window_index = gooey_window_create(min, max);
    printf("window_index: %d\n", window_index);

    printf("window[%d].min.x: %.2f\n", window_index, windows[window_index].max.x);


    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        
        Mat4* projection = ortho(0.0f, SCREEN_WIDTH, 0.0f, SCREEN_HEIGHT, 0.0f, 100.0f);
        glUseProgram(text_shader);
        setShaderMat4(text_shader, "projection", projection);
        
        char mouse_pos_str[50];

        sprintf(mouse_pos_str, "X=%.2f, Y=%.2f", mouse_x, mouse_y);


        
        RenderText(text_shader, mouse_pos_str, 50.0f, 40.0f, 1.0f, color1);


        RenderText(text_shader, "(C) LearnOpenGL.com", 420.0f, 400.0f, 0.5f, color2);

        glUseProgram(window_shader);
        setShaderMat4(window_shader, "projection", projection);

        RenderBox(window_shader, window1, 1.0f, window_color);
        gooey_window_draw(window_shader, SCREEN_HEIGHT, VAO, VBO);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void RenderBox(unsigned int shader_id, Window window, float scale, Vector4D color)
{
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

void RenderText(unsigned int shader_id, char *text, float x, float y, float scale, Vector3D color)
{
    /* activate corresponding render state */
    glUseProgram(shader_id);
    glUniform3f(glGetUniformLocation(shader_id, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    y = SCREEN_HEIGHT - y;

    /* iterate through all characters */
    char *c;
    for (c = text; *c != '\0'; c++)
    {
      
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        /* update VBO for each character */
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        /* render glyph texture over quad */
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        /* update content of VBO memory */
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); /* be sure to use glBufferSubData and not glBufferData */

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        /* render quad */
        glDrawArrays(GL_TRIANGLES, 0, 6);

        /* now advance cursors for next glyph (note that advance is number of 1/64 pixels) */
        x += (ch.Advance >> 6) * scale; /* bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels)) */
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

