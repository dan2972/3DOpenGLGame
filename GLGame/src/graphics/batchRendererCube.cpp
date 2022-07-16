#include "graphics/batchRendererCube.hpp"

#include <array>
#include <glad/glad.h>
#include <iostream>

static const unsigned int MAX_CUBES = 1000;
static const unsigned int MAX_VERTICES = MAX_CUBES * 24;
static const unsigned int MAX_INDICES = MAX_CUBES * 36;
static const unsigned int MAX_TEXTURES = 16;

struct Vertex{
    glm::vec3 position;
    glm::vec4 color;
    glm::vec2 texCoord;
    glm::vec3 normal;
    float texIndex;
};

struct RendererData{
    unsigned int vao = 0;
    unsigned int vbo = 0;
    unsigned int ibo = 0;

    unsigned int whiteTexture = 0;

    unsigned int indexCount = 0;

    Vertex* quadBuffer = nullptr;
    Vertex* quadBufferPtr = nullptr;

    std::array<unsigned int, MAX_TEXTURES> textureSlots;
    unsigned int textureSlotIndex = 1;

    BatchRendererCube::Stats renderStats;
};

static RendererData sData;

void BatchRendererCube::init(){
    if(sData.quadBuffer != nullptr)
        return;
    sData.quadBuffer = new Vertex[MAX_VERTICES];

    glGenVertexArrays(1, &sData.vao);
    glGenBuffers(1, &sData.vbo);
    glGenBuffers(1, &sData.ibo);

    glBindVertexArray(sData.vao);

    glBindBuffer(GL_ARRAY_BUFFER, sData.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * MAX_CUBES * 4, nullptr, GL_DYNAMIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(offsetof(Vertex, position)));
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(offsetof(Vertex, color)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(offsetof(Vertex, texCoord)));
    glEnableVertexAttribArray(2);
    // texture index attribute
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(offsetof(Vertex, texIndex)));
    glEnableVertexAttribArray(3);
    // normal vector attribute
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(offsetof(Vertex, normal)));
    glEnableVertexAttribArray(4);

    unsigned int indices[MAX_INDICES];
    unsigned int offset = 0;
    for (int i = 0; i < MAX_INDICES; i += 36){
        //front
        indices[i + 0] = offset + 0;
        indices[i + 1] = offset + 1;
        indices[i + 2] = offset + 2;
        indices[i + 3] = offset + 2;
        indices[i + 4] = offset + 3;
        indices[i + 5] = offset + 0;
        ///right
        indices[i + 6] = offset + 4;
        indices[i + 7] = offset + 5;
        indices[i + 8] = offset + 6;
        indices[i + 9] = offset + 6;
        indices[i + 10] = offset + 7;
        indices[i + 11] = offset + 4;
        //left
        indices[i + 12] = offset + 8;
        indices[i + 13] = offset + 9;
        indices[i + 14] = offset + 10;
        indices[i + 15] = offset + 10;
        indices[i + 16] = offset + 11;
        indices[i + 17] = offset + 8;
        //top
        indices[i + 18] = offset + 12;
        indices[i + 19] = offset + 13;
        indices[i + 20] = offset + 14;
        indices[i + 21] = offset + 14;
        indices[i + 22] = offset + 15;
        indices[i + 23] = offset + 12;
        //bottom
        indices[i + 24] = offset + 16;
        indices[i + 25] = offset + 17;
        indices[i + 26] = offset + 18;
        indices[i + 27] = offset + 18;
        indices[i + 28] = offset + 19;
        indices[i + 29] = offset + 16;
        //back
        indices[i + 30] = offset + 20;
        indices[i + 31] = offset + 21;
        indices[i + 32] = offset + 22;
        indices[i + 33] = offset + 22;
        indices[i + 34] = offset + 23;
        indices[i + 35] = offset + 20;

        offset += 24;
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sData.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // create a default white texture
    glGenTextures(1, &sData.whiteTexture);
    glBindTexture(GL_TEXTURE_2D, sData.whiteTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    unsigned int color = 0xffffffff;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &color);

    sData.textureSlots[0] = sData.whiteTexture;
    for(int i = 1; i < MAX_TEXTURES; i++)
        sData.textureSlots[i] = 0;
}

void BatchRendererCube::shutdown(){
    glDeleteVertexArrays(1, &sData.vao);
    glDeleteBuffers(1, &sData.vbo);
    glDeleteBuffers(1, &sData.ibo);

    glDeleteTextures(1, &sData.whiteTexture);

    delete[] sData.quadBuffer;
}

void BatchRendererCube::startBatch(){
    sData.quadBufferPtr = sData.quadBuffer;
}

void BatchRendererCube::endBatch(){
    GLsizeiptr size = (uint8_t*)sData.quadBufferPtr - (uint8_t*)sData.quadBuffer;
    glBindBuffer(GL_ARRAY_BUFFER, sData.vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, sData.quadBuffer);
}

void BatchRendererCube::flush(){
    for (unsigned int i = 0; i < sData.textureSlotIndex; i++){
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, sData.textureSlots[i]);
    }

    glBindVertexArray(sData.vao);
    glDrawElements(GL_TRIANGLES, sData.indexCount, GL_UNSIGNED_INT, nullptr);
    sData.renderStats.drawCalls++;

    sData.indexCount = 0;
    sData.textureSlotIndex = 1;
    //std::cout << sData.whiteTexture << std::endl;
}

void BatchRendererCube::setupShaderSampler(Shader& shader){
    shader.use();
    int samplers[MAX_TEXTURES];
    for(int i = 0; i < MAX_TEXTURES; i++)
        samplers[i] = i;
    shader.setTextures("u_Textures", samplers, MAX_TEXTURES);
}

void BatchRendererCube::drawCube(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color){
    if (sData.indexCount >= MAX_INDICES){
        endBatch();
        flush();
        startBatch();
    }

    float textureIndex = 0.0f;

    //front
    sData.quadBufferPtr->position = {position.x, position.y, position.z + size.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {0.0f, 0.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0, 0, 1};
    sData.quadBufferPtr++;
    sData.quadBufferPtr->position = {position.x + size.x, position.y, position.z + size.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {1.0f, 0.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0, 0, 1};
    sData.quadBufferPtr++;
    sData.quadBufferPtr->position = {position.x + size.x, position.y + size.y, position.z + size.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {1.0f, 1.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0, 0, 1};
    sData.quadBufferPtr++;
    sData.quadBufferPtr->position = {position.x, position.y + size.y, position.z + size.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {0.0f, 1.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0, 0, 1};
    sData.quadBufferPtr++;
    //right
    sData.quadBufferPtr->position = {position.x + size.x, position.y, position.z + size.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {0.0f, 0.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {1, 0, 0};
    sData.quadBufferPtr++;
    sData.quadBufferPtr->position = {position.x + size.x, position.y, position.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {1.0f, 0.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {1, 0, 0};
    sData.quadBufferPtr++;
    sData.quadBufferPtr->position = {position.x + size.x, position.y + size.y, position.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {1.0f, 1.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {1, 0, 0};
    sData.quadBufferPtr++;
    sData.quadBufferPtr->position = {position.x + size.x, position.y + size.y, position.z + size.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {0.0f, 1.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {1, 0, 0};
    sData.quadBufferPtr++;
    //left
    sData.quadBufferPtr->position = {position.x, position.y, position.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {0.0f, 0.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {-1, 0, 0};
    sData.quadBufferPtr++;
    sData.quadBufferPtr->position = {position.x, position.y, position.z + size.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {1.0f, 0.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {-1, 0, 0};
    sData.quadBufferPtr++;
    sData.quadBufferPtr->position = {position.x, position.y + size.y, position.z + size.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {1.0f, 1.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {-1, 0, 0};
    sData.quadBufferPtr++;
    sData.quadBufferPtr->position = {position.x, position.y + size.y, position.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {0.0f, 1.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {-1, 0, 0};
    sData.quadBufferPtr++;
    //top
    sData.quadBufferPtr->position = {position.x, position.y + size.y, position.z + size.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {0.0f, 0.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0, 1, 0};
    sData.quadBufferPtr++;
    sData.quadBufferPtr->position = {position.x + size.x, position.y + size.y, position.z + size.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {1.0f, 0.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0, 1, 0};
    sData.quadBufferPtr++;
    sData.quadBufferPtr->position = {position.x + size.x, position.y + size.y, position.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {1.0f, 1.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0, 1, 0};
    sData.quadBufferPtr++;
    sData.quadBufferPtr->position = {position.x, position.y + size.y, position.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {0.0f, 1.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0, 1, 0};
    sData.quadBufferPtr++;
    //bottom
    sData.quadBufferPtr->position = {position.x + size.x, position.y, position.z + size.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {0.0f, 0.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0, -1, 0};
    sData.quadBufferPtr++;
    sData.quadBufferPtr->position = {position.x, position.y, position.z + size.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {1.0f, 0.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0, -1, 0};
    sData.quadBufferPtr++;
    sData.quadBufferPtr->position = {position.x, position.y, position.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {1.0f, 1.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0, -1, 0};
    sData.quadBufferPtr++;
    sData.quadBufferPtr->position = {position.x + size.x, position.y, position.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {0.0f, 1.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0, -1, 0};
    sData.quadBufferPtr++;
    //back
    sData.quadBufferPtr->position = {position.x + size.x, position.y, position.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {0.0f, 0.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0, 0, -1};
    sData.quadBufferPtr++;
    sData.quadBufferPtr->position = {position.x, position.y, position.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {1.0f, 0.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0, 0, -1};
    sData.quadBufferPtr++;
    sData.quadBufferPtr->position = {position.x, position.y + size.y, position.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {1.0f, 1.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0, 0, -1};
    sData.quadBufferPtr++;
    sData.quadBufferPtr->position = {position.x + size.x, position.y + size.y, position.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {0.0f, 1.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0, 0, -1};
    sData.quadBufferPtr++;


    sData.indexCount += 36;
    sData.renderStats.quadCount++;
}

void BatchRendererCube::drawCube(const glm::vec3& position, const glm::vec3& size, unsigned int textureID){
    if (sData.indexCount >= MAX_INDICES || sData.textureSlotIndex >= MAX_TEXTURES - 1){
        endBatch();
        flush();
        startBatch();
    }

    constexpr glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};

    float textureIndex = 0.0f;
    for (unsigned int i = 1; i < sData.textureSlotIndex; i++) {
        if (sData.textureSlots[i] == textureID){
            textureIndex = (float)i;
            break;
        }
    }

    if (textureIndex == 0.0f){
        textureIndex = (float)sData.textureSlotIndex;
        sData.textureSlots[sData.textureSlotIndex] = textureID;
        sData.textureSlotIndex++;
    }

    //front
    sData.quadBufferPtr->position = {position.x, position.y, position.z + size.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {0.0f, 0.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0, 0, 1};
    sData.quadBufferPtr++;
    sData.quadBufferPtr->position = {position.x + size.x, position.y, position.z + size.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {1.0f, 0.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0, 0, 1};
    sData.quadBufferPtr++;
    sData.quadBufferPtr->position = {position.x + size.x, position.y + size.y, position.z + size.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {1.0f, 1.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0, 0, 1};
    sData.quadBufferPtr++;
    sData.quadBufferPtr->position = {position.x, position.y + size.y, position.z + size.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {0.0f, 1.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0, 0, 1};
    sData.quadBufferPtr++;
    //right
    sData.quadBufferPtr->position = {position.x + size.x, position.y, position.z + size.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {0.0f, 0.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {1, 0, 0};
    sData.quadBufferPtr++;
    sData.quadBufferPtr->position = {position.x + size.x, position.y, position.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {1.0f, 0.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {1, 0, 0};
    sData.quadBufferPtr++;
    sData.quadBufferPtr->position = {position.x + size.x, position.y + size.y, position.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {1.0f, 1.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {1, 0, 0};
    sData.quadBufferPtr++;
    sData.quadBufferPtr->position = {position.x + size.x, position.y + size.y, position.z + size.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {0.0f, 1.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {1, 0, 0};
    sData.quadBufferPtr++;
    //left
    sData.quadBufferPtr->position = {position.x, position.y, position.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {0.0f, 0.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {-1, 0, 0};
    sData.quadBufferPtr++;
    sData.quadBufferPtr->position = {position.x, position.y, position.z + size.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {1.0f, 0.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {-1, 0, 0};
    sData.quadBufferPtr++;
    sData.quadBufferPtr->position = {position.x, position.y + size.y, position.z + size.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {1.0f, 1.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {-1, 0, 0};
    sData.quadBufferPtr++;
    sData.quadBufferPtr->position = {position.x, position.y + size.y, position.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {0.0f, 1.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {-1, 0, 0};
    sData.quadBufferPtr++;
    //top
    sData.quadBufferPtr->position = {position.x, position.y + size.y, position.z + size.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {0.0f, 0.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0, 1, 0};
    sData.quadBufferPtr++;
    sData.quadBufferPtr->position = {position.x + size.x, position.y + size.y, position.z + size.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {1.0f, 0.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0, 1, 0};
    sData.quadBufferPtr++;
    sData.quadBufferPtr->position = {position.x + size.x, position.y + size.y, position.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {1.0f, 1.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0, 1, 0};
    sData.quadBufferPtr++;
    sData.quadBufferPtr->position = {position.x, position.y + size.y, position.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {0.0f, 1.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0, 1, 0};
    sData.quadBufferPtr++;
    //bottom
    sData.quadBufferPtr->position = {position.x + size.x, position.y, position.z + size.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {0.0f, 0.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0, -1, 0};
    sData.quadBufferPtr++;
    sData.quadBufferPtr->position = {position.x, position.y, position.z + size.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {1.0f, 0.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0, -1, 0};
    sData.quadBufferPtr++;
    sData.quadBufferPtr->position = {position.x, position.y, position.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {1.0f, 1.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0, -1, 0};
    sData.quadBufferPtr++;
    sData.quadBufferPtr->position = {position.x + size.x, position.y, position.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {0.0f, 1.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0, -1, 0};
    sData.quadBufferPtr++;
    //back
    sData.quadBufferPtr->position = {position.x + size.x, position.y, position.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {0.0f, 0.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0, 0, -1};
    sData.quadBufferPtr++;
    sData.quadBufferPtr->position = {position.x, position.y, position.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {1.0f, 0.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0, 0, -1};
    sData.quadBufferPtr++;
    sData.quadBufferPtr->position = {position.x, position.y + size.y, position.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {1.0f, 1.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0, 0, -1};
    sData.quadBufferPtr++;
    sData.quadBufferPtr->position = {position.x + size.x, position.y + size.y, position.z};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {0.0f, 1.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0, 0, -1};
    sData.quadBufferPtr++;

    sData.indexCount += 36;
    sData.renderStats.quadCount++;
}

void BatchRendererCube::resetStats(){
    memset(&sData.renderStats, 0, sizeof(Stats));
}