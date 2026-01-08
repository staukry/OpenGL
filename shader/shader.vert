#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec4 aColor;
layout(location=2) in vec2 aUV;

out vec2 vUV;
out vec4 vColor;

uniform vec2 offsetv;

void main(){
    gl_Position = vec4(aPos.xy + offsetv.xy, aPos.z, 1.0);
    vUV = aUV;
    vColor = aColor;
}
