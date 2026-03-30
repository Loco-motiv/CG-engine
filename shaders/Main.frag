#version 460 core

layout(binding = 1) uniform sampler2D diffuse;
layout(binding = 2) uniform sampler2D specular;

struct Material {
    float shininess;
    float alpha;
    vec3 colour;
    
    bool hasDiffuseTexture;
    bool hasSpecularTexture;
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

#define POINT_LIGHTS 100
#define SPOT_LIGHTS  50

in vec3 normal;
in vec3 fragPosition;
in vec2 texturePosition;

out vec4 fragColour;

uniform int pointLightCount;
uniform int spotLightCount;

uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLights[POINT_LIGHTS];
uniform SpotLight spotLight[SPOT_LIGHTS];

uniform vec3 viewPosition;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 baseDiffuse, vec3 baseSpecular);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 baseDiffuse, vec3 baseSpecular);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 baseDiffuse, vec3 baseSpecular);

void main()
{    
    vec3 baseDiffuse = material.hasDiffuseTexture ? texture(diffuse, texturePosition).rgb : vec3(1.0);
    baseDiffuse *=  material.colour;

    vec3 baseSpecular = material.hasSpecularTexture ? texture(specular, texturePosition).rgb : vec3(1.0);
    baseSpecular *= material.colour; //TODO do i need u?

    // properties
    vec3 norm = normalize(normal);
    vec3 viewDirection = normalize(viewPosition - fragPosition);
    
    // phase 1: directional lighting
    vec3 result;
    if (material.isLightSource) //TODO do i need u?
    {
        result = baseDiffuse;
    }
    else{
        result = CalcDirLight(dirLight, norm, viewDirection, baseDiffuse, baseSpecular);
    }
    // phase 2: point lights
    for (int i = 0; i < pointLightCount; i++)
        result += CalcPointLight(pointLights[i], norm, fragPosition, viewDirection, baseDiffuse, baseSpecular);    
    // phase 3: spot light
    for (int i = 0; i < spotLightCount; i++)
        result += CalcSpotLight(spotLight[i], norm, fragPosition, viewDirection, baseDiffuse, baseSpecular);    
    
    fragColour = vec4(result, material.alpha);
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 baseDiffuse, vec3 baseSpecular)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading Blinn-Phong
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient * baseDiffuse;
    vec3 diffuse = light.diffuse * diff * baseDiffuse;
    vec3 specular = light.specular * spec * baseSpecular;
    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 baseDiffuse, vec3 baseSpecular)
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
    vec3 ambient = light.ambient * baseDiffuse;
    vec3 diffuse = light.diffuse * diff * baseDiffuse;
    vec3 specular = light.specular * spec * baseSpecular;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 baseDiffuse, vec3 baseSpecular)
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
    vec3 ambient = light.ambient * baseDiffuse;
    vec3 diffuse = light.diffuse * diff * baseDiffuse;
    vec3 specular = light.specular * spec * baseSpecular;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}