#version 130

in vec3 vert;
in vec3 color;
in vec3 normal;
uniform mat4 modelview;
uniform mat4 projection;
flat out vec3 vcolor;
flat out vec3 vnormal;

void main() {
    vcolor = color;
    vnormal = normal;
    gl_Position = projection * (modelview * vec4(vert, 1.0));
}
