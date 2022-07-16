#include "runner/game.hpp"
#include "physics/raycast.hpp"
#include "graphics/objLoader.hpp"
#include "graphics/model.hpp"

unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

Camera camera = Camera{glm::vec3(3.0f, 4.0f, 3.0f)};

float mouse_x = 0;
float mouse_y = 0;

Model fox;

Game::Game(){
    setupWindow();
    
    shader = Shader{"resources/shaders/texQuadShader.vs", "resources/shaders/texQuadShader.fs"};
    modelLoaderShader = Shader{"resources/shaders/shader.vs", "resources/shaders/shader.fs"};
    debugDepthQuad = Shader{"resources/shaders/debugDepthQuad.vs", "resources/shaders/debugDepthQuad.fs"};

    crateTexture = Texture2D{"resources/container.jpg", false};
    awesomeFaceTexture = Texture2D{"resources/awesomeface.png", true};

    BatchRenderer2D::init();
    BatchRenderer2D::setupShaderSampler(shader);

    BatchRendererCube::init();
    BatchRendererCube::setupShaderSampler(shader);

    fox.load("resources/models/cube.obj", "resources/fox.png", false);
    fox.setupShader(modelLoaderShader);
}

void Game::setupWindow(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0); // vsync
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    /*
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);

    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);*/
}

void Game::cleanup(){
    BatchRenderer2D::shutdown();
    BatchRendererCube::shutdown();
    glfwTerminate();
}

void Game::runMainGameLoop(){
    while (!glfwWindowShouldClose(window))
    {
        float current = glfwGetTime();
        deltaTime = current - lastTime;
        lastTime = current;
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.Update(deltaTime);

        // draw stuff
        renderScene();
        
        glfwSwapBuffers(window);
        glfwPollEvents();

        fpsTimer += deltaTime;
        fps++;
        if (fpsTimer >= 1.0f)
        {
            std::cout << "FPS: " << fps << std::endl;
            fpsTimer = 0.0f;
            fps = 0;
        }
    }
}

void Game::renderScene() {
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(20.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

    glm::mat4 model1 = glm::translate(model, glm::vec3(0.125 * 3, 0.0, 0.125 * 3));
    model1 = glm::scale(model1, glm::vec3(0.007));
    model1 = glm::rotate(model1, (float)glfwGetTime(), glm::vec3(0,1,0));
    modelLoaderShader.use();
    modelLoaderShader.setMat4("model", model1);
    modelLoaderShader.setMat4("view", view);
    modelLoaderShader.setMat4("projection", projection);
    float a = 1.25*glm::pi<float>();//glfwGetTime();
    modelLoaderShader.setVec3("lightDir", -glm::vec3(-cos(a), -sin(a), -sin(a)));
    modelLoaderShader.setMat3("normalMatrix", glm::mat3(transpose(inverse(model1))));
    modelLoaderShader.setFloat("ambientStrength", 0.3f);

    fox.draw();

    shader.use();
    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    //float a = 1.25*glm::pi<float>();//glfwGetTime();
    shader.setVec3("lightDir", -glm::vec3(-cos(a), -sin(a), -sin(a)));
    shader.setFloat("ambientStrength", 0.3f);

    Raycast raycast(glm::vec2(mouse_x, mouse_y), glm::vec2(SCR_WIDTH, SCR_HEIGHT), projection, view);
    glm::vec3 intersection = raycast.checkPlaneIntersection(camera.Position, glm::vec3(0, 1, 0), 0);

    BatchRenderer2D::resetStats();
    BatchRenderer2D::startBatch();

    for(int i = 0; i < 100; i++){
        for (int j = 0; j < 100; j++){
            if(j == (int)(intersection.x * 4) && i == (int)(intersection.z * 4)){
                BatchRenderer2D::drawTile(glm::vec2(j * 0.25f, i * 0.25f), glm::vec2(0.25f, 0.25f), glm::vec4(1,1,1,1));
            }else{
                glm::vec4 color = ((i + j) %  2 == 0 ? glm::vec4(0.7, 0.7, 0.7, 1) : glm::vec4(0.4, 0.4, 0.4, 1));
                BatchRenderer2D::drawTile(glm::vec2(j * 0.25f, i * 0.25f), glm::vec2(0.25f, 0.25f), color);
            }
        }
    }
    //BatchRenderer2D::drawQuad(glm::vec2(std::sin(glfwGetTime() + 1), -0.5f), glm::vec2(0.5f, 0.5f), crateTexture.getID());
    //BatchRenderer2D::drawQuad(glm::vec2(std::sin(glfwGetTime()), 0.0f), glm::vec2(0.5f, 0.5f), awesomeFaceTexture.getID());
    BatchRenderer2D::endBatch();
    BatchRenderer2D::flush();

    //std::cout << BatchRenderer2D::getStats().drawCalls << " " << BatchRenderer2D::getStats().quadCount << std::endl;

    BatchRendererCube::resetStats();
    BatchRendererCube::startBatch();
    BatchRendererCube::drawCube(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.25f), crateTexture.getID());
    BatchRendererCube::drawCube(glm::vec3(1.0f, 0.0f, 2.0f), glm::vec3(0.25f), awesomeFaceTexture.getID());
    BatchRendererCube::endBatch();
    BatchRendererCube::flush();
}

void Game::processInput(GLFWwindow* window){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        camera.ProcessKeyboard(UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        camera.ProcessKeyboard(DOWN, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS) {
        camera.ProcessKeyboard(ROTATE_CW, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS) {
        camera.ProcessKeyboard(ROTATE_CCW, deltaTime);
    }
}

void Game::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    //glfwGetWindowSize(window, &SCR_WIDTH, &SCR_HEIGHT);
}

void Game::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    mouse_x = xpos;
    mouse_y = ypos;
    /*
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;
    
    camera.ProcessMouseMovement(xoffset, yoffset);
    */
}

void Game::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}