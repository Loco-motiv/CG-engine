#version 460 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
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
uniform float alpha;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

// void main() {
//     // ambient
//     vec3 ambient = light.ambient * texture(material.diffuse, texturePosition).rgb;  

//     // diffuse 
//     vec3 norm = normalize(normal);
//     vec3 lightDirection = normalize(light.position - fragPosition);
//     float diff = max(dot(norm, lightDirection), 0.0);
//     vec3 diffuse = light.diffuse * diff * texture(material.diffuse, texturePosition).rgb;

//     // specular
//     vec3 viewDirection = normalize(viewPosition - fragPosition);
//     vec3 reflectDirection = reflect(-lightDirection, norm);  
//     float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
//     vec3 specular = light.specular * spec * texture(material.specular, texturePosition).rgb;  
 
//     vec3 result = ambient + diffuse + specular;
//     fragColour = vec4(result, alpha);
// }

void main()
{    
    // properties
    vec3 norm = normalize(normal);
    vec3 viewDirection = normalize(viewPosition - fragPosition);
    
    // == =====================================================
    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // == =====================================================
    // phase 1: directional lighting
    vec3 result = CalcDirLight(dirLight, norm, viewDirection);
    // phase 2: point lights
    for (int i = 0; i < pointLightCount; i++)
        result += CalcPointLight(pointLights[i], norm, fragPosition, viewDirection);    
    // phase 3: spot light
    for (int i = 0; i < spotLightCount; i++)
        result += CalcSpotLight(spotLight[i], norm, fragPosition, viewDirection);    
    
    fragColour = vec4(result, alpha);
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texturePosition));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texturePosition));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, texturePosition));
    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texturePosition));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texturePosition));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, texturePosition));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texturePosition));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texturePosition));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, texturePosition));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}