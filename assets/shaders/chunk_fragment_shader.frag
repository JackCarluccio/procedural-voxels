#version 460 core

in vec3 TexCoord;
in float AmbientOcclusion;

out vec4 FragColor;

uniform sampler2DArray texture_atlas;

void main() {
    FragColor = texture(texture_atlas, TexCoord);
    FragColor.rgb *= AmbientOcclusion;
}
