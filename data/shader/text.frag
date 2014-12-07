#version 130

in vec2 texcoord;
uniform sampler2D texture;
uniform vec4 color;

void main() {
    gl_FragColor = color * texture2D(texture, texcoord).r;
}
