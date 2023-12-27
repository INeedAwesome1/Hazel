#type vertex
#version 330 core

layout (location = 0) in vec3 a_Position;

out vec3 v_Position;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

void main() {
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1);
}

#type pixel
#version 330 core

layout (location = 0) out vec4 o_Color;

uniform vec3 u_Color;

void main() {
	o_Color = vec4(u_Color, 1);
}