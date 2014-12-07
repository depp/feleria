#version 130

#define LIGHT_COUNT 4

in vec3 vert;
in vec3 color;
in vec3 normal;
flat out vec3 vcolor;

uniform mat4 modelview;
uniform mat4 projection;
uniform mat3 normalmat;

uniform vec3 light_dir[LIGHT_COUNT];
uniform vec3 light_color[LIGHT_COUNT];

void main() {
    vec3 norm = normalize(normalmat * normal);
    vec3 light = vec3(0.0, 0.0, 0.0);
    for (int i = 0; i < LIGHT_COUNT; i++) {
        light += max(dot(light_dir[i], norm), 0.0) * light_color[i];
    }
    vcolor = light * color;
    gl_Position = projection * (modelview * vec4(vert, 1.0));
}
