#version 130

in vec3 ex_color;
flat in vec3 ex_light;

void main() {
    gl_FragColor = vec4(ex_color * ex_light, 1.0);
}
