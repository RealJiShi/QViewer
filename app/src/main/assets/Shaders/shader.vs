#version 300 es
layout(location = 0) in vec4 inPos;
out vec4 fColor;
uniform mat4 mvp;
void main() {
    gl_Position = mvp * inPos;
}