#version 450

#extension GL_EXT_scalar_block_layout : enable

struct Light {
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
    float range;
    float innerCutoff;
    float outerCutoff;
    float constant;
    float linear;
    float quadratic;
    int type;  // 0 = Directional, 1 = Point, 2 = Spot
};

struct Material {
    vec4 color;
    float metallic;
    float roughness;
    float ambientOcclusion;
    float padding;
};

layout(binding = 0) uniform UBO {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 cameraPos;
    Material material;
} ubo;

layout(binding = 1) uniform sampler2D albedoMap;
layout(binding = 2) uniform sampler2D normalMap;
layout(binding = 3) uniform sampler2D metallicRoughnessMap;
layout(binding = 4) uniform sampler2D aoMap;
layout(binding = 5) uniform sampler2D emissiveMap;

layout(std430, binding = 6) uniform LightUBO {
    Light lights[4];  // Suporte para até 4 luzes
    int numLights;
} lightUbo;

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

const float PI = 3.14159265359;

// Funções existentes mantidas
vec3 getNormalFromMap() {
    vec3 tangentNormal = texture(normalMap, fragTexCoord).xyz * 2.0 - 1.0;
    vec3 Q1 = dFdx(fragPos);
    vec3 Q2 = dFdy(fragPos);
    vec2 st1 = dFdx(fragTexCoord);
    vec2 st2 = dFdy(fragTexCoord);
    vec3 N = normalize(fragNormal);
    vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
    vec3 B = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);
    return normalize(TBN * tangentNormal);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Nova função para calcular atenuação
float calculateAttenuation(vec3 lightPos, vec3 fragPos, Light light) {
    float distance = length(lightPos - fragPos);
    if(distance > light.range) return 0.0;
    return 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
}

// Nova função para calcular contribuição de cada luz
vec3 calculateLightContribution(Light light, vec3 N, vec3 V, vec3 albedo, float metallic, float roughness, vec3 F0) {
    vec3 L;
    float attenuation = 1.0;
    
    if(light.type == 0) { // Directional
        L = normalize(-light.direction);
    }
    else if(light.type == 1) { // Point
        L = normalize(light.position - fragPos);
        attenuation = calculateAttenuation(light.position, fragPos, light);
    }
    else { // Spot
        L = normalize(light.position - fragPos);
        float theta = dot(L, normalize(-light.direction));
        float epsilon = light.innerCutoff - light.outerCutoff;
        float spotIntensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);
        attenuation = calculateAttenuation(light.position, fragPos, light) * spotIntensity;
    }
    
    vec3 H = normalize(V + L);
    
    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
    
    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;
    
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;
    
    float NdotL = max(dot(N, L), 0.0);
    return (kD * albedo / PI + specular) * light.color * light.intensity * NdotL * attenuation;
}

void main() {
    vec3 albedo = texture(albedoMap, fragTexCoord).rgb;
    float metallic = texture(metallicRoughnessMap, fragTexCoord).b;
    float roughness = texture(metallicRoughnessMap, fragTexCoord).g;
    float ao = texture(aoMap, fragTexCoord).r;
    vec3 emission = texture(emissiveMap, fragTexCoord).rgb;
    
    vec3 N = getNormalFromMap();
    if (!gl_FrontFacing) {
        N = -N;
    }
    
    vec3 V = normalize(ubo.cameraPos - fragPos);
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);
    
    vec3 Lo = vec3(0.0);
    
    // Calcular contribuição de todas as luzes
    for(int i = 0; i < lightUbo.numLights; i++) {
        Lo += calculateLightContribution(lightUbo.lights[i], N, V, albedo, metallic, roughness, F0);
    }
    
    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo + emission;
    
    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // Gamma correction
    color = pow(color, vec3(1.0/2.2));
    
    outColor = vec4(color, 1.0);
}