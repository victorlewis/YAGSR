#version 450

// Attributes
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 dc;
layout(location = 2) in vec3 scale;

// Interpolated outputs
out gData{
    vec3 position;
    vec3 color;    // Added color output
    vec3 scale;
} vertex;

// Uniforms
layout(shared, row_major) uniform Uniform{
    mat4x4 objectToWorldMatrix;
    mat4x4 modelViewProjectionMatrix;
    vec3 cameraPosition;
} object;

void main() {
    // Transform position to world space
    vertex.position = (object.objectToWorldMatrix * vec4(position, 1.0)).xyz;
    // mult by -1.0,-1.0,0.0
    vertex.position = vertex.position * vec3(-1.0, -1.0, 1.0);

    vertex.color = dc * 0.28 + vec3(0.5, 0.5, 0.5);
    vertex.scale = scale;

    // Set point size (you can make this uniform if you want to control it from CPU)
    //gl_PointSize = 5.0;

    // Calculate final position
    gl_Position = object.modelViewProjectionMatrix * vec4(position, 1.0);
}



//float dist = length(vertex.position - object.cameraPosition);
//float attenuation = 1.0 / (1.0 + 0.1 * dist);
// force it to be between 0 and 1
//attenuation = clamp(attenuation, 0.0, 1.0);
//vertex.color = vec4(attenuation, attenuation, attenuation, 1.0);
