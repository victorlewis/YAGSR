#version 450

// Input from vertex shader
in gData{
    vec3 position;
    vec3 color;
} fragment;

// Output
out vec4 fragColor;

void main() {
    fragColor = vec4(fragment.color, 1.0);
}