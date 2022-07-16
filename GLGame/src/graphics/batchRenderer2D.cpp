#include "graphics/batchRenderer2D.hpp"

#include <array>
#include <glad/glad.h>
#include <iostream>

static const unsigned int MAX_QUADS = 10000;
static const unsigned int MAX_VERTICES = MAX_QUADS * 4;
static const unsigned int MAX_INDICES = MAX_QUADS * 6;
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

    BatchRenderer2D::Stats renderStats;
};

static RendererData sData;

void BatchRenderer2D::init(){
    if(sData.quadBuffer != nullptr)
        return;
    sData.quadBuffer = new Vertex[MAX_VERTICES];

    glGenVertexArrays(1, &sData.vao);
    glGenBuffers(1, &sData.vbo);
    glGenBuffers(1, &sData.ibo);

    glBindVertexArray(sData.vao);

    glBindBuffer(GL_ARRAY_BUFFER, sData.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * MAX_QUADS * 4, nullptr, GL_DYNAMIC_DRAW);

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
    for (int i = 0; i < MAX_INDICES; i += 6){
        indices[i + 0] = offset + 0;
        indices[i + 1] = offset + 1;
        indices[i + 2] = offset + 2;

        indices[i + 3] = offset + 2;
        indices[i + 4] = offset + 3;
        indices[i + 5] = offset + 0;

        offset += 4;
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

void BatchRenderer2D::shutdown(){
    glDeleteVertexArrays(1, &sData.vao);
    glDeleteBuffers(1, &sData.vbo);
    glDeleteBuffers(1, &sData.ibo);

    glDeleteTextures(1, &sData.whiteTexture);

    delete[] sData.quadBuffer;
}

void BatchRenderer2D::startBatch(){
    sData.quadBufferPtr = sData.quadBuffer;
}

void BatchRenderer2D::endBatch(){
    GLsizeiptr size = (uint8_t*)sData.quadBufferPtr - (uint8_t*)sData.quadBuffer;
    glBindBuffer(GL_ARRAY_BUFFER, sData.vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, sData.quadBuffer);
}

void BatchRenderer2D::flush(){
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

void BatchRenderer2D::setupShaderSampler(Shader& shader){
    shader.use();
    int samplers[MAX_TEXTURES];
    for(int i = 0; i < MAX_TEXTURES; i++)
        samplers[i] = i;
    shader.setTextures("u_Textures", samplers, MAX_TEXTURES);
}

void BatchRenderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color){
    if (sData.indexCount >= MAX_INDICES){
        endBatch();
        flush();
        startBatch();
    }

    float textureIndex = 0.0f;

    sData.quadBufferPtr->position = {position.x, position.y, 0.0f};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {0.0f, 0.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0.0f, 0.0f, 1.0f};
    sData.quadBufferPtr++;

    sData.quadBufferPtr->position = {position.x + size.x, position.y, 0.0f};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {1.0f, 0.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0.0f, 0.0f, 1.0f};
    sData.quadBufferPtr++;

    sData.quadBufferPtr->position = {position.x + size.x, position.y + size.y, 0.0f};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {1.0f, 1.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0.0f, 0.0f, 1.0f};
    sData.quadBufferPtr++;

    sData.quadBufferPtr->position = {position.x, position.y + size.y, 0.0f};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {0.0f, 1.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0.0f, 0.0f, 1.0f};
    sData.quadBufferPtr++;

    sData.indexCount += 6;
    sData.renderStats.quadCount++;
}

void BatchRenderer2D::drawTile(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color){
    if (sData.indexCount >= MAX_INDICES){
        endBatch();
        flush();
        startBatch();
    }

    float textureIndex = 0.0f;

    sData.quadBufferPtr->position = {position.x, 0.0f, position.y + size.y};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {0.0f, 0.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0.0f, 1.0f, 0.0f};
    sData.quadBufferPtr++;

    sData.quadBufferPtr->position = {position.x + size.x, 0.0f, position.y + size.y};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {1.0f, 0.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0.0f, 1.0f, 0.0f};
    sData.quadBufferPtr++;

    sData.quadBufferPtr->position = {position.x + size.x, 0.0f, position.y};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {1.0f, 1.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0.0f, 1.0f, 0.0f};
    sData.quadBufferPtr++;

    sData.quadBufferPtr->position = {position.x, 0.0f, position.y};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {0.0f, 1.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0.0f, 1.0f, 0.0f};
    sData.quadBufferPtr++;

    sData.indexCount += 6;
    sData.renderStats.quadCount++;
}

void BatchRenderer2D::drawTile(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, float rotation){
    if (sData.indexCount >= MAX_INDICES){
        endBatch();
        flush();
        startBatch();
    }
    
    glm::mat2 r = glm::mat2(glm::cos(rotation), glm::sin(rotation), -glm::sin(rotation), glm::cos(rotation));
    float halfWidth = size.x * 0.5f;
    float halfHeight = size.y * 0.5f;
    glm::vec2 translation = glm::vec2(position.x + size.x / 2, position.y + size.y / 2);
    glm::vec2 topLeft =  r * glm::vec2(-halfWidth, -halfHeight) + translation;
    glm::vec2 topRight = r * glm::vec2(halfWidth, -halfHeight) + translation;
    glm::vec2 bottomRight = r * glm::vec2(halfWidth, halfHeight) + translation;
    glm::vec2 bottomLeft = r * glm::vec2(-halfWidth, halfHeight) + translation;

    float textureIndex = 0.0f;

    sData.quadBufferPtr->position = {bottomLeft.x, 0.0f, bottomLeft.y};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {0.0f, 0.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0.0f, 1.0f, 0.0f};
    sData.quadBufferPtr++;

    sData.quadBufferPtr->position = {bottomRight.x, 0.0f, bottomRight.y};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {1.0f, 0.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0.0f, 1.0f, 0.0f};
    sData.quadBufferPtr++;

    sData.quadBufferPtr->position = {topRight.x, 0.0f, topRight.y};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {1.0f, 1.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0.0f, 1.0f, 0.0f};
    sData.quadBufferPtr++;

    sData.quadBufferPtr->position = {topLeft.x, 0.0f, topLeft.y,};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {0.0f, 1.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0.0f, 1.0f, 0.0f};
    sData.quadBufferPtr++;

    sData.indexCount += 6;
    sData.renderStats.quadCount++;
}

void BatchRenderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, unsigned int textureID){
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

    sData.quadBufferPtr->position = {position.x, position.y, 0.0f};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {0.0f, 0.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0.0f, 0.0f, 1.0f};
    sData.quadBufferPtr++;

    sData.quadBufferPtr->position = {position.x + size.x, position.y, 0.0f};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {1.0f, 0.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0.0f, 0.0f, 1.0f};
    sData.quadBufferPtr++;

    sData.quadBufferPtr->position = {position.x + size.x, position.y + size.y, 0.0f};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {1.0f, 1.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0.0f, 0.0f, 1.0f};
    sData.quadBufferPtr++;

    sData.quadBufferPtr->position = {position.x, position.y + size.y, 0.0f};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {0.0f, 1.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0.0f, 0.0f, 1.0f};
    sData.quadBufferPtr++;

    sData.indexCount += 6;
    sData.renderStats.quadCount++;
}

void BatchRenderer2D::drawTile(const glm::vec2& position, const glm::vec2& size, unsigned int textureID){
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

    sData.quadBufferPtr->position = {position.x, 0.0f, position.y + size.y};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {0.0f, 0.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0.0f, 1.0f, 0.0f};
    sData.quadBufferPtr++;

    sData.quadBufferPtr->position = {position.x + size.x, 0.0f, position.y + size.y};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {1.0f, 0.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0.0f, 1.0f, 0.0f};
    sData.quadBufferPtr++;

    sData.quadBufferPtr->position = {position.x + size.x, 0.0f, position.y};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {1.0f, 1.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0.0f, 1.0f, 0.0f};
    sData.quadBufferPtr++;

    sData.quadBufferPtr->position = {position.x, 0.0f, position.y};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {0.0f, 1.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0.0f, 1.0f, 0.0f};
    sData.quadBufferPtr++;

    sData.indexCount += 6;
    sData.renderStats.quadCount++;
}

void BatchRenderer2D::drawTile(const glm::vec2& position, const glm::vec2& size, unsigned int textureID, float rotation){
    if (sData.indexCount >= MAX_INDICES || sData.textureSlotIndex >= MAX_TEXTURES - 1){
        endBatch();
        flush();
        startBatch();
    }

    constexpr glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};

    glm::mat2 r = glm::mat2(glm::cos(rotation), glm::sin(rotation), -glm::sin(rotation), glm::cos(rotation));
    float halfWidth = size.x * 0.5f;
    float halfHeight = size.y * 0.5f;
    glm::vec2 translation = glm::vec2(position.x + size.x / 2, position.y + size.y / 2);
    glm::vec2 topLeft =  r * glm::vec2(-halfWidth, -halfHeight) + translation;
    glm::vec2 topRight = r * glm::vec2(halfWidth, -halfHeight) + translation;
    glm::vec2 bottomRight = r * glm::vec2(halfWidth, halfHeight) + translation;
    glm::vec2 bottomLeft = r * glm::vec2(-halfWidth, halfHeight) + translation;

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

    sData.quadBufferPtr->position = {bottomLeft.x, 0.0f, bottomLeft.y};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {0.0f, 0.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0.0f, 1.0f, 0.0f};
    sData.quadBufferPtr++;

    sData.quadBufferPtr->position = {bottomRight.x, 0.0f, bottomRight.y};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {1.0f, 0.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0.0f, 1.0f, 0.0f};
    sData.quadBufferPtr++;

    sData.quadBufferPtr->position = {topRight.x, 0.0f, topRight.y};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {1.0f, 1.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0.0f, 1.0f, 0.0f};
    sData.quadBufferPtr++;

    sData.quadBufferPtr->position = {topLeft.x, 0.0f, topLeft.y,};
    sData.quadBufferPtr->color = color;
    sData.quadBufferPtr->texCoord = {0.0f, 1.0f};
    sData.quadBufferPtr->texIndex = textureIndex;
    sData.quadBufferPtr->normal = {0.0f, 1.0f, 0.0f};
    sData.quadBufferPtr++;

    sData.indexCount += 6;
    sData.renderStats.quadCount++;
}

void BatchRenderer2D::resetStats(){
    memset(&sData.renderStats, 0, sizeof(Stats));
}

const BatchRenderer2D::Stats& BatchRenderer2D::getStats(){
    return sData.renderStats;
}