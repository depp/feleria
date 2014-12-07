#version 130

in vec2 vtexcoord;
uniform sampler2D texture;
uniform vec4 color;

void main() {
    gl_FragColor = texture2D(texture, vtexcoord) * color;
}
