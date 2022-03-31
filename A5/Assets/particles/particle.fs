#version 330 core
in vec2 TexCoords;
in vec4 ParticleColor;
out vec4 fragColour;

uniform sampler2D sprite;

void main()
{
    fragColour = vec4(vec3(texture(sprite, TexCoords)), ParticleColor.a);
}
