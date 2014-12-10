#version 140

in vec3 ex_color;
flat in vec3 ex_light;
out vec4 out_color;

void main() {
    out_color = vec4(ex_color * ex_light, 1.0);
}
