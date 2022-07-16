#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in float aTexIndex;
layout (location = 4) in vec3 aNormal;

out vec4 vColor;
out vec2 vTexCoord;
out float vTexIndex;
out vec3 vNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vColor = aColor;
    vTexCoord = aTexCoord;
    vTexIndex = aTexIndex;
    vNormal = aNormal;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}