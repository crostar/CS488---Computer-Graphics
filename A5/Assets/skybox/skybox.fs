#version 330

const float PI = 3.1415926535897932384626433832795;
out vec4 fragColour;

in vec3 TexCoords;

uniform samplerCube skybox;

float transparency(vec3 TexCoords) {
  return length(TexCoords - vec3(0.0)) - 0.3;
}

void main()
{
    vec4 texColor = texture(skybox, TexCoords);
    fragColour = vec4(texColor.rgb * 0.8, transparency(TexCoords));
}
