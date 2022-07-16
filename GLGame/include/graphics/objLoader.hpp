#ifndef GLGAME_OBJLOADER_HPP
#define GLGAME_OBJLOADER_HPP

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>

class OBJLoader{
public:
    static bool loadFromFile(const std::string& path, std::vector<float>& vertexArray, std::vector<int>& indexArray){
        std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
        std::vector<glm::vec3> temp_vertices;
        std::vector<glm::vec2> temp_uvs;
        std::vector<glm::vec3> temp_normals;
        std::unordered_map<std::string, int> vertexMap;
        int currentIndex = 0;

        std::ifstream file;
        file.open(path);

        if(file.is_open()){
            std::string line;
            while(getline(file, line)) {
                std::stringstream ss(line);
                std::string header;

                ss >> header;

                if(header == "v"){
                    glm::vec3 vertex;
                    ss >> vertex.x >> vertex.y >> vertex.z;
                    temp_vertices.push_back(vertex);
                } else if (header == "vt") {
                    glm::vec2 uv;
                    ss >> uv.x >> uv.y;
                    temp_uvs.push_back(uv);
                } else if (header == "vn") {
                    glm::vec3 normal;
                    ss >> normal.x >> normal.y >> normal.z;
                    temp_normals.push_back(normal);
                } else if (header == "f") {
                    std::string vertex;
                    unsigned int v[3];
                    std::string token;

                    for (int i = 0; i < 3; i++) {
                        ss >> vertex;
                        auto it = vertexMap.find(vertex);
                        if (it != vertexMap.end()) {
                            indexArray.push_back(it->second);
                        } else {
                            std::stringstream ss1(vertex);
                            vertexMap.insert({vertex, currentIndex});
                            int i = 0;
                            while (getline(ss1, token, '/')) {
                                v[i] = std::stoi(token);
                                i++;
                            }
                            vertexIndices.push_back(v[0] - 1);
                            uvIndices.push_back(v[1] - 1);
                            normalIndices.push_back(v[2] - 1);
                            vertexArray.push_back(temp_vertices[vertexIndices[currentIndex]].x);
                            vertexArray.push_back(temp_vertices[vertexIndices[currentIndex]].y);
                            vertexArray.push_back(temp_vertices[vertexIndices[currentIndex]].z);
                            vertexArray.push_back(temp_uvs[uvIndices[currentIndex]].x);
                            vertexArray.push_back(temp_uvs[uvIndices[currentIndex]].y);
                            vertexArray.push_back(temp_normals[normalIndices[currentIndex]].x);
                            vertexArray.push_back(temp_normals[normalIndices[currentIndex]].y);
                            vertexArray.push_back(temp_normals[normalIndices[currentIndex]].z);

                            indexArray.push_back(currentIndex);

                            currentIndex++;
                        }
                    }
                }
            }

        } else {
            std::cout << "unable to open file: " << path << std::endl;
            return false;
        }
        int asdf = 0;
        file.close();
        return true;
    }
};

#endif
