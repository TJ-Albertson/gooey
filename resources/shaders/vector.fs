#version 330 core
out vec4 fragColor;

in vec2 TexCoords;

uniform bool convex;
uniform vec4 color;

void main()
{
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
        if (convex) {
            fragColor = color;
       
        } else {
            discard;
        }
        
    } else if (alpha < 0.0) {
        // Outside
        if (convex) {
            discard;
       
        } else {
            fragColor = color;
        }
        
       
    } else {
        // Near boundary
        fragColor = vec4(0.0, 1.0, 0.0, alpha);
    }
}
