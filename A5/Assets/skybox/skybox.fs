#version 330

out vec4 fragColour;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
    fragColour = texture(skybox, TexCoords);
}
