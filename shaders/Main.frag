#version 460 core

layout(binding = 1) uniform sampler2D diffuseMap;
layout(binding = 2) uniform sampler2D specularMap;
layout(binding = 3) uniform sampler2D alphaMap;
layout(binding = 4) uniform sampler2D normalMap;
layout(binding = 5) uniform samplerCubeArray pointShadowMaps;

struct Material {
    float shininess;
    float alpha;
    vec3 colour;
    vec3 ambientColour;
    vec3 diffuseColour;
    vec3 specularColour;
    
    bool hasDiffuseTexture;
    bool hasSpecularTexture;
    bool hasAlphaTexture;
    bool hasNormalTexture;
    bool isLightSource;
};

struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
    float farPlane; 
    int shadowIndex;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

layout(std430, binding = 0) readonly buffer PointLightBuffer {
    PointLight pointLights[];
};

#define SPOT_LIGHTS  16

in vec3 normal;
in vec3 fragPosition;
in vec2 texturePosition;
in mat3 TBN;

out vec4 fragColour;

uniform int pointLightCount;
uniform int spotLightCount;

uniform Material material;
uniform DirLight dirLight;
uniform SpotLight spotLight[SPOT_LIGHTS];

uniform vec3 viewPosition;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 baseAmbient, vec3 baseDiffuse, vec3 baseSpecular);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 baseAmbient, vec3 baseDiffuse, vec3 baseSpecular);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 baseAmbient, vec3 baseDiffuse, vec3 baseSpecular);

float CalcPointShadow(vec3 fragPos, PointLight light) {
    if (light.shadowIndex < 0) return 0.0;

    vec3 fragToLight = fragPos - light.position;
    vec3 lightDir = normalize(light.position - fragPos);
    float currentDepth = length(fragToLight);
    
    float closestDepth = texture(pointShadowMaps, vec4(fragToLight, light.shadowIndex)).r;
    closestDepth *= light.farPlane;
    
    float bias = max(100 * (1.0 - dot(normal, lightDir)), 0.5); 
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    
    return shadow;
}

void main()
{
    float baseAlpha = material.alpha;
    vec3 baseDiffuse = vec3(1.0);
    if (material.hasDiffuseTexture){
        vec4 texColour = texture(diffuseMap, texturePosition);
        baseDiffuse = texColour.rgb;
        baseAlpha = texColour.a;
    }
    baseDiffuse *=  material.diffuseColour;

    vec3 baseSpecular = material.hasSpecularTexture ? texture(specularMap, texturePosition).rgb : vec3(1.0);
    baseSpecular *= material.specularColour;

    // properties
    vec3 norm = normalize(normal);
    vec3 viewDirection = normalize(viewPosition - fragPosition);
    if (material.hasAlphaTexture){
        baseAlpha = texture(alphaMap, texturePosition).r;
    }
    if (baseAlpha < 0.1) discard;

    if (material.hasNormalTexture && material.hasAlphaTexture){
        norm = texture(normalMap, texturePosition).rgb;
        norm = norm * 2.0 - 1.0;   
        norm = normalize(TBN * norm); 
    }
    vec3 result;
    if (material.isLightSource) //TODO do i need u?
    {
        result = baseDiffuse;
    }
    else{
        // phase 1: directional lighting
        result = CalcDirLight(dirLight, norm, viewDirection, material.ambientColour, baseDiffuse, baseSpecular);
        // phase 2: point lights
        for (int i = 0; i < pointLightCount; i++)
            result += CalcPointLight(pointLights[i], norm, fragPosition, viewDirection, material.ambientColour, baseDiffuse, baseSpecular);    
        // phase 3: spot light
        for (int i = 0; i < spotLightCount; i++)
            result += CalcSpotLight(spotLight[i], norm, fragPosition, viewDirection, material.ambientColour, baseDiffuse, baseSpecular);    
    }
    
    
    fragColour = vec4(result, baseAlpha);
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 baseAmbient, vec3 baseDiffuse, vec3 baseSpecular)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading Blinn-Phong
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient * baseAmbient;
    vec3 diffuse = light.diffuse * diff * baseDiffuse;
    vec3 specular = light.specular * spec * baseSpecular;
    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 baseAmbient, vec3 baseDiffuse, vec3 baseSpecular)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading Blinn-Phong
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * baseAmbient;
    vec3 diffuse = light.diffuse * diff * baseDiffuse;
    vec3 specular = light.specular * spec * baseSpecular;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    float shadow = CalcPointShadow(fragPos, light);
    
    return (ambient + (1.0 - shadow) * (diffuse + specular));
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 baseAmbient, vec3 baseDiffuse, vec3 baseSpecular)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading Blinn-Phong
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * baseAmbient;
    vec3 diffuse = light.diffuse * diff * baseDiffuse;
    vec3 specular = light.specular * spec * baseSpecular;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}