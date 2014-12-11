#version 130

in vec2 ex_texcoord;

uniform sampler2D u_texture;

void main() {
    vec4 color = texture(u_texture, ex_texcoord);
    if (color.a < 0.5) {
        discard;
    }
    gl_FragColor = color;
}
