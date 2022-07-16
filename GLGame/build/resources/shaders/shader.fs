#version 330 core
out vec4 FragColor;

in vec2 vTexCoord;
in vec3 vNormal;

uniform sampler2D u_texture;
uniform vec3 lightDir;
uniform float ambientStrength;

void main()
{
    vec3 norm = normalize(vNormal);
    float diff = max(dot(norm, normalize(-lightDir)), 0.0);
    vec3 diffuse = diff * vec3(1,1,1);
    FragColor = vec4(vec3(ambientStrength) + diffuse.xyz, 1) * (texture(u_texture, vTexCoord));
}