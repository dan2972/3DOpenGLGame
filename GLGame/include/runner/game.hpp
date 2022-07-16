#ifndef GAME_HPP
#define GAME_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "graphics/stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "graphics/shader.h"
#include "graphics/camera.h"
#include "graphics/texture2D.hpp"
#include "graphics/texQuadBatch.hpp"
#include "graphics/batchRenderer2D.hpp"
#include "graphics/batchRendererCube.hpp"

#include <iostream>
#include <entt/entity/registry.hpp>

class Game{
public:
    Game();
    void setupWindow();

    void runMainGameLoop();

    void cleanup();

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
private:
    GLFWwindow* window = nullptr;

    Shader shader;
    Shader modelLoaderShader;
    Shader debugDepthQuad;

    Texture2D crateTexture;
    Texture2D awesomeFaceTexture;
    Texture2D foxTexture;

    double lastTime, deltaTime, fpsTimer = 0;
    unsigned int fps = 0;

    void renderScene();

    void processInput(GLFWwindow* window);
};

#endif