#version 130

in vec3 vert;
in vec2 texcoord;

out vec2 vtexcoord;

uniform mat4 modelview;
uniform mat4 projection;
uniform vec4 vertxform;
uniform vec2 texscale;

void main() {
    vtexcoord = texcoord * texscale;
    gl_Position = projection * (modelview * vec4(vert, 1.0));
}
