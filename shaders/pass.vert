#version 330
layout (location = 0) in vec2 position;
layout (location = 1) in vec3 color;

out vec2 _position; // = position?
out vec3 _color;

uniform vec2 resolution ;

void main() {
     vec2 screen_position = vec2(
          position.x / resolution.x,
          position.y / resolution.y );

     gl_Position = vec4(screen_position, .0, 1.0);
     _position = position;
     _color = color;
}