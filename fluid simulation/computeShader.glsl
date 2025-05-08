#version 460 core
layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D screen;

void main() {
    ivec2 uv = gl_GlobalInvocationID.xy;
}