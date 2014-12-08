#version 130

in vec2 vtexcoord;

uniform sampler2D texture;

void main() {
    vec4 color = texture2D(texture, vtexcoord);
    if (color.a < 0.5) {
        discard;
    }
    gl_FragColor = color;
}
