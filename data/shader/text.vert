#version 140

in vec4 in_vert;
out vec2 ex_texcoord;

uniform vec4 u_vertxform;
uniform vec2 u_texscale;

void main() {
    ex_texcoord = in_vert.zw * u_texscale;
    gl_Position = vec4(
        in_vert.xy * u_vertxform.xy + u_vertxform.zw, 0.0, 1.0);
}
