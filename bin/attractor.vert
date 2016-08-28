#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color_input;

out vec3 color;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 mtx;

void main(){
   //gl_Position = vec4(position.xyz, 1.0f);
   //gl_Position = mtx * vec4(position.xyz, 1.0f);
   gl_Position = vec4(position.xyz, 1.0f);
   color = color_input;
}