 #version 460 core

in vec3 Normal;  
in vec3 FragPos;  
in vec3 vertexPos;
in vec4 FragPosLightSpace;
//in vec3 lightEyePos;
//in vec3 normalEyeSpace; 

out vec4 fragColor;

uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform mat4 viewMat;

uniform vec3 objectColor;
uniform vec3 ambient;
uniform vec3 specularColor;
uniform float shininess;

uniform sampler2D depthMap;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // transofrm to range 0-1
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // get depth values
    float closestDepth = texture(depthMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;

    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(depthMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}


void main()
{
    vec3 normal = normalize(Normal);
    // diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(lightDir, normal), 0.0);

    vec3 diffuse = diff * objectColor;
    // specular
    vec3 viewDir = normalize(cameraPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    vec3 specular = pow(max(dot(normal, halfwayDir), 0.0), shininess) * specularColor;

    // calculate shadow
    float shadow = ShadowCalculation(FragPosLightSpace);                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));    
    
    fragColor = vec4(lighting, 1.0);

}