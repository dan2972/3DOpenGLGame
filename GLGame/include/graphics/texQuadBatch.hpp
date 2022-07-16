#ifndef TEX_QUAD_BATCH_HPP
#define TEX_QUAD_BATCH_HPP
#include <glad/glad.h>
#include <array>
#include "shader.h"
#include "graphics/camera.h"
#include "texture2D.hpp"

class TexQuadBatch{
public:
    TexQuadBatch();
    ~TexQuadBatch();
    void render(Camera& camera, float deltaTime);

    struct TexQuadVertex{
        glm::vec3 Position;
        glm::vec4 Color;
        glm::vec2 TexCoords;
        float TexID;
    };
private:
    std::array<TexQuadBatch::TexQuadVertex, 4> createQuad(float x, float y, float sizeX, float sizeY, float textureID);
    Shader shader;
    unsigned int VAO, VBO, EBO;
    unsigned int maxQuads = 250;
    Texture2D texture1{"resources/container.jpg", false};
    Texture2D texture2{"resources/awesomeface.png", true};
};

#endif