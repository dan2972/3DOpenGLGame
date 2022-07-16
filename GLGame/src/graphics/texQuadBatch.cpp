#include "graphics/texQuadBatch.hpp"

TexQuadBatch::TexQuadBatch(){
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0,
        4, 5, 6,
        6, 7, 4,
    };	
    shader = Shader("resources/shaders/texQuadShader.vs", "resources/shaders/texQuadShader.fs");
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TexQuadVertex) * maxQuads * 4, nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TexQuadVertex), (const void*)(offsetof(TexQuadVertex, Position)));
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(TexQuadVertex), (const void*)(offsetof(TexQuadVertex, Color)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TexQuadVertex), (const void*)(offsetof(TexQuadVertex, TexCoords)));
    glEnableVertexAttribArray(2);
    // texture coord attribute
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(TexQuadVertex), (const void*)(offsetof(TexQuadVertex, TexID)));
    glEnableVertexAttribArray(3);

    shader.use();
    int sampler[2] = {0, 1};
    shader.setTextures("u_Textures", sampler, 2);
}

TexQuadBatch::~TexQuadBatch(){
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void TexQuadBatch::render(Camera& camera, float deltaTime){

    auto q0 = createQuad(-1.5f, 0, 1, 1, 0);
    auto q1 = createQuad(0.5f, 0, 1, 1, 1);

    TexQuadVertex vertices[8];
    memcpy(vertices, q0.data(), q0.size() * sizeof(TexQuadVertex));
    memcpy(vertices + q0.size(), q1.data(), q1.size()  * sizeof(TexQuadVertex));
    
    shader.use();
     // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    texture1.bind();
    glActiveTexture(GL_TEXTURE0 + 1);
    texture2.bind();
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    glm::mat4 view = camera.GetViewMatrix();
    
    projection = glm::perspective(glm::radians(45.0f), (float)800 / 600, 0.1f, 100.0f);

    shader.use();
    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    shader.setFloat("tick", deltaTime);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

std::array<TexQuadBatch::TexQuadVertex, 4> TexQuadBatch::createQuad(float x, float y, float sizeX, float sizeY, float textureID){
    TexQuadVertex v0;
    v0.Position = glm::vec3(x, y, 0);
    v0.Color = glm::vec4(1, 1, 1, 1);
    v0.TexCoords = glm::vec2(0, 0);
    v0.TexID = textureID;

    TexQuadVertex v1;
    v1.Position = glm::vec3(x + sizeX, y, 0);
    v1.Color = glm::vec4(1, 1, 1, 1);
    v1.TexCoords = glm::vec2(1, 0);
    v1.TexID = textureID;

    TexQuadVertex v2;
    v2.Position = glm::vec3(x + sizeX, y + sizeY, 0);
    v2.Color = glm::vec4(1, 1, 1, 1);
    v2.TexCoords = glm::vec2(1, 1);
    v2.TexID = textureID;

    TexQuadVertex v3;
    v3.Position = glm::vec3(x, y + sizeY, 0);
    v3.Color = glm::vec4(1, 1, 1, 1);
    v3.TexCoords = glm::vec2(0, 1);
    v3.TexID = textureID;

    return {v0, v1, v2, v3};
}