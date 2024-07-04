#ifndef CLASSES_H_INCLUDED
#define CLASSES_H_INCLUDED

#include <vector>
#include <map>
#include <fstream>
#include <array>
#include <GL/glut.h>

#include "utils.h"

class Material {
private:
    std::array<float,3> ambient;
    std::array<float,3> diffuse;
    std::array<float,3> specular;
    std::array<float,3> emissive;
    float shininess;
public:
    Material(std::array<float,3> _ambient, std::array<float,3> _diffuse, std::array<float,3> _specular, std::array<float,3> _emissive, float _shininess) {
        ambient = _ambient;
        diffuse = _diffuse;
        specular = _specular;
        emissive = _emissive;
        shininess = _shininess;
    }

    float* getAmbient() {
        return ambient.data();
    }

    float* getDiffuse() {
        return diffuse.data();
    }

    float* getSpecular() {
        return specular.data();
    }

    float* getEmissive() {
        return emissive.data();
    }

    float getShininess() {
        return shininess;
    }

    Material() {}
};

class Snow {
private:
    float x;
    float y;
    float z;
    float radius;
    float velocity;

public:
    Snow() {
        y = 100;
        x = getRandom(-50, 50);
        z = getRandom(-50, 50);
        velocity = getRandomFromZeroToOne();
        radius = getRandomFromZeroToOne();
    }

    void updatePos() {
        y -= velocity;
        if (y < -12) y = 100;
    }

    float getX() {
        return x;
    }

    float getY() {
        return y;
    }

    float getZ() {
        return z;
    }

    float getRadius() {
        return radius;
    }

};

class Vertex {
private:
    int positionIndex;
    int textureIndex;
    int normalIndex;

public:
    Vertex(int pIndex, int tIndex, int nIndex ) {
        positionIndex = pIndex;
        textureIndex = tIndex;
        normalIndex = nIndex;
    }

    int getPositionIndex() {
        return positionIndex;
    }

    int getTextureIndex() {
        return textureIndex;
    }

    int getNormalIndex() {
        return normalIndex;
    }
};

class Face {
private:
    std::vector<Vertex> vertices;
public:
    Face(std::vector<Vertex> _vertices) {
        vertices = _vertices;
    }

    Face() {};

    std::vector<Vertex> getVertices() {
        return vertices;
    }

};

class Model {

private:
    std::vector<std::vector<float>> vertexPositions;
    std::vector<std::vector<float>> vertexNormals;
    std::vector<std::vector<float>> vertexTexCoords;

    std::map<std::string, Material> materials;

    std::map<std::string, std::vector<Face>> faces;

    void storeVertex(std::string line) {
//        std::cout << "Storing vertex " << line << std::endl;
        std::vector<std::string> tokens = split(line);
        std::vector<float> vertexPosition;
        for (int i=1; i<tokens.size(); i++) {
            vertexPosition.push_back(std::stof(tokens[i]));
        }
        vertexPositions.push_back(vertexPosition);
    }

    void storeNormal(std::string line) {
//        std::cout << "Storing normal " << line <<  std::endl;
        std::vector<std::string> tokens = split(line);
        std::vector<float> vertexNormal;
        for (int i=1; i<tokens.size(); i++) {
            vertexNormal.push_back(std::stof(tokens[i]));
        }
        vertexNormals.push_back(vertexNormal);
    }

    void storeTexture(std::string line) {
//        std::cout << "Storing texture " << line << std::endl;
        std::vector<std::string> tokens = split(line);
        std::vector<float> vertexTexCoord;
        for (int i=1; i<tokens.size(); i++) {
            vertexTexCoord.push_back(std::stof(tokens[i]));
        }
        vertexTexCoords.push_back(vertexTexCoord);
    }

    void storeFace(std::string materialName, std::string line) {
//        std::cout << "Storing face " << line << std::endl;
        std::vector<std::string> tokens = split(line); /* First element is 'f' */

        std::vector<Vertex> vertices;

        for (int i=1; i<tokens.size(); i++) {
            std::vector<std::string> vertex = split(tokens[i], '/');

            int pIndex = -1;
            int tIndex = -1;
            int nIndex = -1;
            if (vertex[1] == "") {
                pIndex = std::stoi(vertex[0]) - 1;
                nIndex = std::stoi(vertex[2]) - 1;
            }
            else {
                pIndex = std::stoi(vertex[0]) - 1;
                tIndex = std::stoi(vertex[1]) - 1;
                nIndex = std::stoi(vertex[2]) - 1;
            }
            vertices.push_back(Vertex(pIndex, tIndex, nIndex));
        }

        faces[materialName].push_back(Face(vertices));
    }

    void loadMaterial(const char* matPath) {

        std::cout << "Reading material file: " << matPath << std::endl;

        std::ifstream matFile(matPath);

        if (!matFile.is_open()) {
            std::cout << "Failed to read material file: " << matPath << std::endl;
            exit(-1);
        }

        std::string line;
        while (std::getline(matFile, line)) {
//            std::cout << "Parsing " << line << std::endl;
            if (line.find("newmtl ") == 0) {
                std::string matName = split(line)[1];
                float shininess = 0;
                std::array<float,3> ambient = {0, 0, 0};
                std::array<float,3> diffuse = {0, 0, 0};
                std::array<float,3> specular = {0, 0, 0};
                std::array<float,3> emissive = {0, 0, 0};

                std::getline(matFile, line);
                if (line.find("Ns ") == 0) {
                    shininess = stof(split(line)[1]);
                    std::getline(matFile, line);
                }

                if (line.find("Ka ") == 0) {
                    std::vector<std::string> values = split(line);
                    ambient = { stof(values[1]), stof(values[2]), stof(values[3]) };
                    std::getline(matFile, line);
                }

                std::vector<std::string> values;
                if (line.find("Kd ") == 0) {
                    values = split(line);
                    diffuse = { stof(values[1]), stof(values[2]), stof(values[3]) };
                    std::getline(matFile, line);
                }

                if (line.find("Ks ") == 0) {
                    values = split(line);
                    std::array<float,3> specular = { stof(values[1]), stof(values[2]), stof(values[3]) };
                    std::getline(matFile, line);
                }

                if (line.find("Ke ") == 0) {
                    values = split(line);
                    std::array<float,3> emissive = { stof(values[1]), stof(values[2]), stof(values[3]) };
                }

                Material material = Material(ambient, diffuse, specular, emissive, shininess);
                materials[matName] = material;
            }
        }
        std::cout << "Loaded material successfully!" << std::endl;
        matFile.close();
    }

    void loadModel(const char* modelPath) {

        std::cout << "Reading model file: " << modelPath << std::endl;

        std::ifstream modelFile(modelPath);

        if (!modelFile.is_open()) {
            std::cout << "Failed to read model file: " << modelPath << std::endl;
            exit(-1);
        }

        std::string line;

        std::string currentMaterial;
        while (std::getline(modelFile, line)) {
//            std::cout << "Parsing " << line << std::endl;

            if (line.find("mtllib ") == 0) {
                std::string matPath = "./assets/" + split(line)[1];
                loadMaterial(matPath.c_str());
            }
            else if (line.find("v ") == 0) {
                storeVertex(line);
            }
            else if (line.find("vt ") == 0) {
                storeTexture(line);
            }
            else if (line.find("vn ") == 0) {
                storeNormal(line);
            }
            else if (line.find("usemtl ") == 0) {
                currentMaterial = split(line)[1];
            }
            else if (line.find("f ") == 0) {
                storeFace(currentMaterial, line);
            }

        }

        std::cout << "Loaded model file successfully: " << modelPath << std::endl;
        modelFile.close();
    }

    void useMaterial(Material material) {
        glMaterialfv(GL_FRONT, GL_AMBIENT, material.getAmbient());
        glMaterialfv(GL_FRONT, GL_DIFFUSE, material.getDiffuse());
        glMaterialfv(GL_FRONT, GL_SPECULAR, material.getSpecular());
        glMaterialfv(GL_FRONT, GL_EMISSION, material.getEmissive());
        glMaterialf(GL_FRONT, GL_SHININESS, material.getShininess());
    }

public:
    Model(const char* modelPath) {
        loadModel(modelPath);
    }

    void drawModel() {
        for (auto const& keyValue : faces) {
            std::string materialName = keyValue.first;
            std::vector<Face> currentFaces = keyValue.second;

            useMaterial(materials[materialName]);
            glDisable(GL_COLOR_MATERIAL);


            for (Face face : currentFaces) {
                glBegin(GL_POLYGON);
                    for(Vertex vertex : face.getVertices()) {
                        int posIndex = vertex.getPositionIndex();
                        int texIndex = vertex.getTextureIndex();
                        int normIndex = vertex.getNormalIndex();
                        if (normIndex != -1) {
                            glNormal3f(vertexNormals[normIndex][0], vertexNormals[normIndex][1], vertexNormals[normIndex][2]);
                        }
                        if (texIndex != -1) {
                            glTexCoord2f(vertexTexCoords[texIndex][0], vertexTexCoords[texIndex][1]);
                        }
                        glVertex3f(vertexPositions[posIndex][0], vertexPositions[posIndex][1], vertexPositions[posIndex][2]);
                    }
                glEnd();
            }
        }
    }
};

#endif // CLASSES_H_INCLUDED
