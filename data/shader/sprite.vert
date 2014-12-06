#version 120

attribute vec2 vert;
attribute vec2 texcoord;
uniform vec4 vertxform;
uniform vec2 texscale;
varying vec2 vtexcoord;

void main() {
    vtexcoord = texcoord * texscale;
    gl_Position = vec4(vert * vertxform.xy + vertxform.zw, 0.0, 1.0);
}
