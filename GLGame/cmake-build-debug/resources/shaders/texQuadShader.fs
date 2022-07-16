#version 330 core
out vec4 FragColor;
  
in vec4 vColor;
in vec2 vTexCoord;
in float vTexIndex;
in vec3 vNormal;

uniform sampler2D u_Textures[16];
uniform vec3 lightDir;
uniform float ambientStrength;

void main()
{
    int index = int(vTexIndex);

    vec3 norm = normalize(vNormal);
    float diff = max(dot(norm, normalize(-lightDir)), 0.0);
    vec3 diffuse = diff * vec3(1,1,1);
    FragColor = vec4(vec3(ambientStrength) + diffuse.xyz, 1) * (texture(u_Textures[index], vTexCoord) * vColor);
}