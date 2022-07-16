#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec2 vTexCoord;
out vec3 vNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

void main()
{
    vTexCoord = aTexCoord;
    vNormal = normalMatrix * aNormal;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}