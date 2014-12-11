#version 130

in vec3 in_vert;
in vec2 in_texcoord;
out vec2 ex_texcoord;

uniform mat4 u_modelview;
uniform mat4 u_projection;
uniform vec2 u_texscale;

void main() {
    ex_texcoord = in_texcoord * u_texscale;
    gl_Position = u_projection * (u_modelview * vec4(in_vert, 1.0));
}
