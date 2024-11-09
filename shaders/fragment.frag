#version 450

layout(location = 0) in vec3 fragColor;  // Cor recebida do vertex shader
layout(location = 0) out vec4 outColor;  // Cor final do fragmento

void main() {
    outColor = vec4(fragColor, 1.0);  // A cor final é a cor passada do vertex shader
}
