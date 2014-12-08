#version 130

in vec2 vtexcoord;

uniform sampler2D texture;

void main() {
    vec4 sample = texture2D(texture, vtexcoord);
    if (sample.a < 0.5) {
        discard;
    }
    gl_FragColor = sample;
}
