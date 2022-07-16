#ifndef BATCH_RENDERER_CUBE_HPP
#define BATCH_RENDERER_CUBE_HPP
// code greatly inspired by The Cherno
#include <glm/glm.hpp>
#include "graphics/shader.h"

class BatchRendererCube
{
public:
    static void init();
    static void shutdown();

    static void startBatch();
    static void endBatch();
    static void flush();

    static void drawCube(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color);
    static void drawCube(const glm::vec3& position, const glm::vec3& size, unsigned int textureID);

    static void setupShaderSampler(Shader& shader);

    struct Stats{
        unsigned int drawCalls = 0;
        unsigned int quadCount = 0;
    };
    
    static const Stats& getStats();
    static void resetStats();
};

#endif