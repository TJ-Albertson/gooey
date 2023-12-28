#version 330 core
layout (location = 0) in vec4 aPos; // <vec2 pos, vec2 tex>
layout (location = 1) in vec2 aTex; // <vec2 pos, vec2 tex>

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
}