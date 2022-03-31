#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoors;

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 offset;
uniform vec4 colour;

void main()
{
    float scale = 0.08f;
    TexCoords = texCoors;
    ParticleColor = colour;
    gl_Position = projection * view * vec4(position * scale + offset, 1.0);
}
