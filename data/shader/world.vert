#version 130

const int light_count = 4;

in vec4 vert;
in vec3 normal;
out vec3 vcolor;
flat out vec3 vlight;

uniform mat4 modelview;
uniform mat4 projection;
uniform mat3 normalmat;

uniform vec4 terrain_color[8];
uniform vec3 light_dir[light_count];
uniform vec3 light_color[light_count];

void main() {
    int index = int(vert.w) + 2;
    vec4 c1 = terrain_color[index * 2], c2 = terrain_color[index * 2 + 1];
    vcolor = mix(c1.rgb, c2.rgb, smoothstep(c1.a, c2.a, vert.z));

    vec3 norm = normalize(normalmat * normal);
    vec3 light = vec3(0.0, 0.0, 0.0);
    for (int i = 0; i < light_count; i++) {
        light += max(dot(light_dir[i], norm), 0.0) * light_color[i];
    }
    vlight = light;

    gl_Position = projection * (modelview * vec4(vert.xyz, 1.0));
}
