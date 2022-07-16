#ifndef BATCH_RENDERER_2D_HPP
#define BATCH_RENDERER_2D_HPP
// code greatly inspired by The Cherno
#include <glm/glm.hpp>
#include "graphics/shader.h"

class BatchRenderer2D
{
public:
    static void init();
    static void shutdown();

    static void startBatch();
    static void endBatch();
    static void flush();

    static void drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
    static void drawQuad(const glm::vec2& position, const glm::vec2& size, unsigned int textureID);
    static void drawTile(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, float rotation);
    static void drawTile(const glm::vec2& position, const glm::vec2& size, unsigned int textureID, float rotation);
    static void drawTile(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
    static void drawTile(const glm::vec2& position, const glm::vec2& size, unsigned int textureID);

    static void setupShaderSampler(Shader& shader);

    struct Stats{
        unsigned int drawCalls = 0;
        unsigned int quadCount = 0;
    };
    
    static const Stats& getStats();
    static void resetStats();
};

#endif