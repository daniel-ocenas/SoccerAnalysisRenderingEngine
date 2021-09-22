#version 460 core

layout(binding=0) uniform sampler2D Texture0;
layout(binding=1) uniform sampler2D Texture1;

in vec2 TexCoords;

out vec4 fragColor;

void main()
{  
    vec4 texel0, texel1;

    texel1 = texture2D(Texture1, TexCoords);
    texel0 = texture2D(Texture0, TexCoords);

    fragColor = texel0 * texel1.r;

    //fragColor = texture(Texture1, TexCoords);
}