#ifndef GLGAME_MODEL_HPP
#define GLGAME_MODEL_HPP
#include <glad/glad.h>
#include <vector>
#include "objLoader.hpp"
#include "texture2D.hpp"
#include "shader.h"

class Model {
public:
    Model() = default;
    ~Model(){
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ibo);
    }
    void setupShader(Shader& shader){
        shader.setInt("u_texture", 0);
    }

    void load(std::string path, std::string texturePath, bool alphaOn){
        texture = Texture2D{texturePath.c_str(), alphaOn};
        OBJLoader::loadFromFile(path, vertexArray, indexArray);

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ibo);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexArray.size(), vertexArray.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * indexArray.size(), indexArray.data(), GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void*)0);
        glEnableVertexAttribArray(0);
        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // normal vector attribute
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void*)(5 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }

    void draw() {
        glActiveTexture(GL_TEXTURE0);
        texture.bind();

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indexArray.size(), GL_UNSIGNED_INT, nullptr);
    }
private:
    unsigned int vao, vbo, ibo;

    std::vector<float> vertexArray;
    std::vector<int> indexArray;

    Texture2D texture;
};

#endif
