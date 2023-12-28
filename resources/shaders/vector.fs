#version 330 core
out vec4 fragColor;

in vec2 TexCoords;
in vec2 FragCoords;

uniform vec2 iResolution;

void main()
{
    // Gradients
   // vec2 p = (2.0 * TexCoords - iResolution.xy) / min(iResolution.y, iResolution.x);
    vec2 p = TexCoords;
    vec2 px = dFdx(p);
    vec2 py = dFdy(p);

    // Chain rule
    float fx = (2.0 * p.x) * px.x - px.y;
    float fy = (2.0 * p.x) * py.x - py.y;

    // Signed distance
    float sd = (p.x * p.x - p.y) / sqrt(fx * fx + fy * fy);

    // Linear alpha
    float alpha = 0.5 - sd;

    if (alpha > 1.0) {
        // Inside
         discard;
    } else if (alpha < 0.0) {
        // Outside
        fragColor = vec4(1.0, 0.0, 0.0, 1.0);
       
    } else {
        // Near boundary
        fragColor = vec4(0.0, 1.0, 0.0, alpha);
    }
}
