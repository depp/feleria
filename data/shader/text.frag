#version 140

in vec2 ex_texcoord;
out vec4 out_color;

uniform sampler2D u_texture;
uniform vec4 u_color;

void main() {
    out_color = u_color * texture(u_texture, ex_texcoord).r;
}
