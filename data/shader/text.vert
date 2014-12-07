#version 130

in vec4 vert;
uniform vec4 vertxform;
uniform vec2 texscale;
out vec2 texcoord;

void main() {
    texcoord = vert.zw * texscale;
    gl_Position = vec4(vert.xy * vertxform.xy + vertxform.zw, 0.0, 1.0);
}
