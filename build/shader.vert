#version 450

//=============================== mainvert
attribute vec3 position;

out gl_PerVertex { vec4 gl_Position; };

varying vec4 v_position;

void main() {
    v_position = vec4(position, 1.0);
    gl_Position = vec4(position,1.0);
}
