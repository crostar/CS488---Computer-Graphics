#version 330

// Model-Space coordinates
layout (location = 0) in vec3 position;

uniform mat4 ModelView;
uniform mat4 Perspective;

out vec3 TexCoords;


void main() {
	TexCoords = position;
  vec4 pos = Perspective * ModelView * vec4(position, 1.0);
	gl_Position = pos.xyww;
}
