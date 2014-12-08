#version 130

in vec4 vert;

out vec2 vtexcoord;

void main() {
    vtexcoord = vert.zw;
    gl_Position = vec4(vert.xy, 0.0, 1.0);
}
