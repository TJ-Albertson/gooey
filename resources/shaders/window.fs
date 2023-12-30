#version 330 core
out vec4 FragColor;

uniform vec4 color;
uniform float time;

void main()
{
    vec2 fragCoord = gl_FragCoord.xy;

    // You can adjust the frequency and speed of the movement by changing the constants
    float frequency = 7.0;
    float speed = 0.3;

    // Introduce time-dependent factor
    float timeFactor = time * speed;

    // Calculate alpha based on y-coordinate and time
    float alpha = (cos((fragCoord.y + timeFactor * 50.0) / frequency) + 10.0) / 15.0;

    FragColor = vec4(color.x, color.y, color.z, alpha);
}
