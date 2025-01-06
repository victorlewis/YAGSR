#version 450

// Input from vertex shader
in gData{
    vec3 position;
    vec3 color;
} fragment;

// Output
out vec4 fragColor;

void main() {
    // Create circular points instead of squares
    vec2 circCoord = 2.0 * gl_PointCoord - 1.0;
    if (dot(circCoord, circCoord) > 1.0) {
        discard;
    }

    // Output color
    //fragColor = vec4(1.0);
    fragColor = vec4(fragment.color, 1.0);

    // Optional: Add simple shading based on distance from camera
    // float dist = length(fragment.position - object.cameraPosition);
    // float attenuation = 1.0 / (1.0 + 0.1 * dist);
    // fragColor.rgb *= attenuation;
}