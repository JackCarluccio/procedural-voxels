#version 460 core

in vec3 FragPos;

out vec4 FragColor;

void main() {
    FragColor = vec4(FragPos.x, FragPos.y, FragPos.z, 1.0);
} 
