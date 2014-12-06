#version 120

uniform sampler2D texture;
uniform vec4 color;
varying vec2 texcoord;

void main() {
    gl_FragColor = color * texture2D(texture, texcoord).r;
}
