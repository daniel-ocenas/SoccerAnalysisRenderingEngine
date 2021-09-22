#version 450 core

uniform sampler2D Texture0;

in vec2 TexCoords;

out vec4 fragColor;

void main()
{  
    fragColor = texture(Texture0, TexCoords);
}