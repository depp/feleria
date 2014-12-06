#version 120

uniform sampler2D texture;
uniform vec4 color;
varying vec2 vtexcoord;

void main() {
    gl_FragColor = texture2D(texture, vtexcoord) * color;
}
