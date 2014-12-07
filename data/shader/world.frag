#version 130

in vec3 vcolor;
flat in vec3 vlight;

void main() {
    gl_FragColor = vec4(vcolor * vlight, 1.0);
}
