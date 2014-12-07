#version 130

flat in vec3 vcolor;
flat in vec3 vnormal;

void main() {
    gl_FragColor = vec4(vcolor, 1.0);
}
