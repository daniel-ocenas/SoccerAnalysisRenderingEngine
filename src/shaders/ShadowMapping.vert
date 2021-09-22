#version 460 core

layout(location = 0) in vec3 viPosition;
layout(location = 1) in vec2 viUvs;
layout(location = 2) in vec3 viNormals;

out vec3 Normal;
out vec3 FragPos;  
out vec2 UV;
out vec3 vertexPos;
out vec4 FragPosLightSpace;
//out vec3 lightEyePos;
//out vec3 normalEyeSpace; 

// Values that stay constant for the whole mesh.
uniform mat4 projMat;
uniform mat4 viewMat;
uniform mat4 modelMat;
uniform mat4 lightSpaceMat;


void main()
{

    FragPos = vec3( modelMat * vec4(viPosition, 1.0) );
    //lightEyePos = vec3(projMat  * vec4(viPosition, 1.0) );
    FragPosLightSpace = lightSpaceMat * vec4(FragPos, 1.0);
    //normalEyeSpace = vec3( modelMat * vec4(viNormals, 0.0));
//    Normal = viNormals;

    Normal = transpose(inverse(mat3(modelMat))) * viNormals;
    UV = viUvs;
    gl_Position =  projMat * viewMat * modelMat * vec4(viPosition, 1.0) ;
    vertexPos = viPosition;
}