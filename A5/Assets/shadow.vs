#version 330 core
#define PI 3.1415926535897932384626433832795

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec2 TexCoords;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
    vec4 ViewCoor;
} vs_out;

uniform mat4 view;
uniform mat4 model;
uniform mat4 Perspective;
uniform mat4 lightSpaceMatrix;
uniform float revertNormal;

void main()
{
    vs_out.FragPos = vec3(model * vec4(position, 1.0));
    vs_out.Normal = revertNormal * transpose(inverse(mat3(model))) * normal;
    vs_out.TexCoords = vec2(
      (atan(position.z , position.x) + PI) / (2.0 * PI),
      asin(-position.y) / PI + 0.5
    );
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    gl_Position = Perspective * view * model * vec4(position, 1.0);
    vs_out.ViewCoor = view * model * vec4(position, 1.0);
}
