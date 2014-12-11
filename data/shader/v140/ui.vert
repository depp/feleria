#version 140

in vec4 in_vert;
out vec2 ex_texcoord;

void main() {
    ex_texcoord = in_vert.zw;
    gl_Position = vec4(in_vert.xy, 0.0, 1.0);
}
