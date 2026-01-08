#version 330 core
in vec2 vUV;
in vec4 vColor;
uniform sampler2D uTex;

out vec4 FragColor;

void main(){
    FragColor = texture(uTex, vUV) * vColor; // 혹은 그냥 texture(uTex, vUV)
}
