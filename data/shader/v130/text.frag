#version 130

in vec2 ex_texcoord;

uniform sampler2D u_texture;
uniform vec4 u_color;

void main() {
    gl_FragColor = u_color * texture(u_texture, ex_texcoord).r;
}
