#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec4 color;        // This receives materialComponent.color
    float metallic;
    float roughness;
    float ao;
    float padding;
} ubo;

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec4 fragColor;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 texColor = texture(texSampler, fragTexCoord);
    vec3 texColorRGB = texColor.rgb == vec3(0.0) ? ubo.color.rgb : texColor.rgb;  // Use material color as fallback
    vec3 finalColor = (texColorRGB * ubo.color.rgb) * ubo.ao;
    outColor = vec4(finalColor, texColor.a * ubo.color.a);
}
