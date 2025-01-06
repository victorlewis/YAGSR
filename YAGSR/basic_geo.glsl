#version 450

layout(points) in;
layout(triangle_strip, max_vertices = 24) out;

// Input from vertex shader
in gData{
    vec3 position;
    vec3 color;
    vec3 scale;
} vertices[];

// Output to fragment shader
out gData{
    vec3 position;
    vec3 color;
} geometry;

// Uniforms (make sure these match your vertex shader)
layout(shared, row_major) uniform Uniform{
    mat4x4 objectToWorldMatrix;
    mat4x4 modelViewProjectionMatrix;
    vec3 cameraPosition;
} object;

void EmitCubeFace(vec3 center, vec3 right, vec3 up, vec3 normal)
{
    vec3 halfSize = vec3(0.5); // Cube size of 1.0

    // Add the normal offset to properly position each face
    vec3 faceCenter = center + normal * halfSize.z;

    // Calculate the four corners of the face
    vec3 p1 = faceCenter - right * halfSize.x - up * halfSize.y;
    vec3 p2 = faceCenter + right * halfSize.x - up * halfSize.y;
    vec3 p3 = faceCenter - right * halfSize.x + up * halfSize.y;
    vec3 p4 = faceCenter + right * halfSize.x + up * halfSize.y;

    // Emit the face as two triangles
    geometry.color = vertices[0].color;

    geometry.position = p1;
    gl_Position = object.modelViewProjectionMatrix * vec4(p1, 1.0);
    EmitVertex();

    geometry.position = p2;
    gl_Position = object.modelViewProjectionMatrix * vec4(p2, 1.0);
    EmitVertex();

    geometry.position = p3;
    gl_Position = object.modelViewProjectionMatrix * vec4(p3, 1.0);
    EmitVertex();

    geometry.position = p4;
    gl_Position = object.modelViewProjectionMatrix * vec4(p4, 1.0);
    EmitVertex();

    EndPrimitive();
}

void main()
{
    vec3 center = vertices[0].position;

    //vec3 scale = vertices[0].scale; // Multiply by 0.5 since we're extending in both directions
    // test with scale 0.01, 0.01, 0.01
    //vec3 scale = 
    vec3 scale = vertices[0].scale * vec3(0.1, 0.1, 0.1);

    // Front face
    EmitCubeFace(center, vec3(scale.x, 0, 0), vec3(0, scale.y, 0), vec3(0, 0, scale.z));
    // Back face
    EmitCubeFace(center, vec3(-scale.x, 0, 0), vec3(0, scale.y, 0), vec3(0, 0, -scale.z));
    // Right face
    EmitCubeFace(center, vec3(0, 0, -scale.z), vec3(0, scale.y, 0), vec3(scale.x, 0, 0));
    // Left face
    EmitCubeFace(center, vec3(0, 0, scale.z), vec3(0, scale.y, 0), vec3(-scale.x, 0, 0));
    // Top face
    EmitCubeFace(center, vec3(scale.x, 0, 0), vec3(0, 0, -scale.z), vec3(0, scale.y, 0));
    // Bottom face
    EmitCubeFace(center, vec3(scale.x, 0, 0), vec3(0, 0, scale.z), vec3(0, -scale.y, 0));
}