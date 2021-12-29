#version 430 core

layout (local_size_x = 8, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding=0) writeonly buffer Data{
    int data[];
};

void main(){
    data[gl_LocalInvocationID.x] = int(gl_LocalInvocationID.x);
}