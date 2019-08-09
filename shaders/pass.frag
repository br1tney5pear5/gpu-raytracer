#version 330

layout (location = 0) out vec4 out_frag_color;

in vec2 _position;
in vec3 _color;

void main() {
     //out_frag_color = vec4(_color, 1.0);
     out_frag_color = vec4(1.0,0.0,0.0, 1.0);
}