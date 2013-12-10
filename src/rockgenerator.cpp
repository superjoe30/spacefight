#include "rockgenerator.h"
#include "lightblock.h"

#include <iostream>

RockGenerator::RockGenerator(ResourceBundle *bundle)
{
    mShader = bundle->getShader("rock");

    mLightBlock = mShader->getUniformBlock("Light", LIGHT_BLOCK_FIELDS, 0);
    mMaterialBlock = mShader->getUniformBlock("Material", MATERIAL_BLOCK_FIELDS, 1);


    mShaderModelViewMatrix = mShader->uniformId("ModelViewMatrix");
    mShaderNormalMatrix = mShader->uniformId("NormalMatrix");
    mShaderProjectionMatrix = mShader->uniformId("ProjectionMatrix");
    mShaderMvp = mShader->uniformId("MVP");
    texUniformId = mShader->uniformId("Tex");

    attribPositionIndex = mShader->attribLocation("VertexPosition");
    attribNormalIndex = mShader->attribLocation("VertexNormal");
    attribTexCoordIndex = mShader->attribLocation("TexCoord");

    ResourceBundle::Image image;
    bundle->getImage("rockTexture", image);

    glGenTextures(1, &mTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    image.doPixelStoreAlignment();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, image.format(), image.type(), image.pixels());
    image.resetPixelStoreAlignment();


    GLenum err = glGetError();
    assert(err == GL_NO_ERROR);
}

RockGenerator::~RockGenerator()
{
    glDeleteTextures(1, &mTexture);
    mTexture = 0;
}


static const float invSqrt2 = 0.7071067811865475; // 1/sqrt(2)


static const float PI = 3.1415926535;

static float randFloat() {
    return rand() / (float) RAND_MAX;
}


void RockGenerator::generate(Rock &rock)
{
    rock.cleanup();

    rock.generator = this;

    std::vector<glm::vec3> points;
    std::vector<glm::vec2> texCoords;
    std::vector<GLuint> indexes;

    int rowCount = 10;

    float sideLen = 2 * invSqrt2 / (float)rowCount;


    glm::vec3 top(0, 1, 0);
    glm::vec3 bottomLeft(invSqrt2, 0, -invSqrt2);
    glm::vec3 downLeftDir = glm::normalize(bottomLeft - top);
    glm::vec3 zPos(0, 0, 1);
    glm::vec3 zNeg(0, 0, -1);
    glm::vec3 xNeg(-1, 0, 0);
    glm::vec3 xPos(1, 0, 0);
    glm::vec2 uvPos(0, 0);
    float uvSideLen = 1.0f / (float)rowCount;
    glm::vec2 uvZPos(0, 1);
    glm::vec2 uvZNeg = -uvZPos;
    glm::vec2 uvXPos(-1, 0);
    glm::vec2 uvXNeg = -uvXPos;
    for (int row = 0; row <= rowCount; row += 1) {
        glm::vec3 pt = top + downLeftDir * (row * sideLen);
        points.push_back(pt);
        texCoords.push_back(uvPos);
        for (int i = 1; i <= row; i += 1) {
            pt += zPos * sideLen;
            uvPos += uvZPos * uvSideLen;
            points.push_back(pt);
            texCoords.push_back(uvPos);
        }
        for (int i = 1; i <= row; i += 1) {
            pt += xNeg * sideLen;
            uvPos += uvXNeg * uvSideLen;
            points.push_back(pt);
            texCoords.push_back(uvPos);
        }
        for (int i = 1; i <= row; i += 1) {
            pt += zNeg * sideLen;
            uvPos += uvZNeg * uvSideLen;
            points.push_back(pt);
            texCoords.push_back(uvPos);
        }
        for (int i = 1; i < row; i += 1) {
            pt += xPos * sideLen;
            uvPos += uvXPos * uvSideLen;
            points.push_back(pt);
            texCoords.push_back(uvPos);
        }
    }

    // triangle on the cap
    indexes.push_back(1);
    indexes.push_back(0);
    indexes.push_back(2);

    indexes.push_back(0);
    indexes.push_back(3);

    indexes.push_back(0);
    indexes.push_back(4);

    indexes.push_back(0);
    indexes.push_back(1);

    int topIndex = 1;
    int bottomIndex = 5;
    for (int row = 1; row < rowCount; row += 1) {
        int triangleCount = row + 1;
        int rowStartTop = topIndex;
        int rowStartBottom = bottomIndex;

        indexes.push_back(bottomIndex);

        indexes.push_back(bottomIndex++);
        for (int i = 0; i < triangleCount; i += 1) {
            indexes.push_back(topIndex++);
            indexes.push_back(bottomIndex++);
        }
        topIndex -= 1;
        for (int i = 0; i < triangleCount; i += 1) {
            indexes.push_back(topIndex++);
            indexes.push_back(bottomIndex++);
        }
        topIndex -= 1;
        for (int i = 0; i < triangleCount; i += 1) {
            indexes.push_back(topIndex++);
            indexes.push_back(bottomIndex++);
        }
        topIndex -= 1;
        for (int i = 0; i < triangleCount - 1; i += 1) {
            indexes.push_back(topIndex++);
            indexes.push_back(bottomIndex++);
        }
        indexes.push_back(rowStartTop);
        indexes.push_back(rowStartBottom);
    }
    // add all the points except the bottom row to points again, flipping the Y
    int pointsMidway = points.size();
    int pointsExceptLastRow = pointsMidway - rowCount * 4;
    for (int i = pointsExceptLastRow - 1; i >= 0; i -= 1) {
        glm::vec3 pt = points[i];
        pt[1] = -pt[1];
        points.push_back(pt);
        texCoords.push_back(texCoords[i]);
    }
    // add all the indexes again to indexes
    int indexEnd = indexes.size();
    for (int i = 0; i < indexEnd; i += 1) {
        int index = points.size() - 1 - indexes[i];
        // if the index is trying to be a bottom row index, adjust it accordingly
        if (index < pointsMidway) {
            index = pointsExceptLastRow + (pointsMidway - index);
        }
        indexes.push_back(index);
    }

    std::vector<glm::vec3> normals;
    for (unsigned int i = 0; i < points.size(); i += 1) {
        glm::vec3 normal = glm::normalize(points[i]);
        normals.push_back(normal);

        points[i] = (0.8f + randFloat() * 0.4f) * normal;
    }


    rock.elementCount = indexes.size();

    glGenVertexArrays(1, &rock.vertexArrayObject);
    glGenBuffers(1, &rock.vertexPositionBuffer);
    glGenBuffers(1, &rock.vertexNormalBuffer);
    glGenBuffers(1, &rock.vertexIndexBuffer);
    glGenBuffers(1, &rock.texCoordBuffer);


    glBindVertexArray(rock.vertexArrayObject);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rock.vertexIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(GLuint), &indexes[0], GL_STATIC_DRAW);


    if (attribPositionIndex != -1) {
        glBindBuffer(GL_ARRAY_BUFFER, rock.vertexPositionBuffer);
        glBufferData(GL_ARRAY_BUFFER, points.size() * 3 * sizeof(GLfloat), &points[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(attribPositionIndex);
        glVertexAttribPointer(attribPositionIndex, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    } else {
        std::cerr << "warning: shader ignoring vertex position data\n";
    }

    if (attribNormalIndex != -1) {
        glBindBuffer(GL_ARRAY_BUFFER, rock.vertexNormalBuffer);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * 3 * sizeof(GLfloat), &normals[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(attribNormalIndex);
        glVertexAttribPointer(attribNormalIndex, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    } else {
        std::cerr << "warning: shader ignoring vertex normal data\n";
    }

    if (attribTexCoordIndex != -1) {
        glBindBuffer(GL_ARRAY_BUFFER, rock.texCoordBuffer);
        glBufferData(GL_ARRAY_BUFFER, texCoords.size() * 2 * sizeof(GLfloat), &texCoords[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(attribTexCoordIndex);
        glVertexAttribPointer(attribTexCoordIndex, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    } else {
        std::cerr << "Warning: shader ignoring tex coord data\n";
    }

    GLenum err = glGetError();
    assert(err == GL_NO_ERROR);
}