#version 130

const int LIGHT_COUNT = 4;

in vec4 in_vert;
in vec3 in_normal;
out vec3 ex_color;
flat out vec3 ex_light;

uniform mat4 u_modelview;
uniform mat4 u_projection;
uniform mat3 u_normalmat;

uniform vec4 u_terrain_color[8];
uniform vec3 u_light_dir[LIGHT_COUNT];
uniform vec3 u_light_color[LIGHT_COUNT];

void main() {
    int index = int(in_vert.w) + 2;
    vec4 c1 = u_terrain_color[index * 2];
    vec4 c2 = u_terrain_color[index * 2 + 1];
    ex_color = mix(c1.rgb, c2.rgb, smoothstep(c1.a, c2.a, in_vert.z));

    vec3 norm = normalize(u_normalmat * in_normal);
    vec3 light = vec3(0.0, 0.0, 0.0);
    for (int i = 0; i < LIGHT_COUNT; i++) {
        light += max(dot(u_light_dir[i], norm), 0.0) * u_light_color[i];
    }
    ex_light = light;

    gl_Position = u_projection * (u_modelview * vec4(in_vert.xyz, 1.0));
}
