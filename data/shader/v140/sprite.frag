#version 140

in vec2 ex_texcoord;
out vec4 out_color;

uniform sampler2D u_texture;

void main() {
    vec4 sample = texture(u_texture, ex_texcoord);
    if (sample.a < 0.5) {
        discard;
    }
    out_color = sample;
}
