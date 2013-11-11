#ifndef SCENE_H
#define SCENE_H


#include <glm/glm.hpp>

#include "shader.h"
#include <SDL2/SDL.h>
#include <GL/glew.h>

class Scene
{
public:
    Scene();
    ~Scene();
    int start();
private:
    bool mRunning = true;
    GLuint mVertexArrayId;
    GLuint mVertexBuffer;
    SDL_GLContext mContext;
    SDL_Window *mWindow;
    Shader *mShader;
    glm::mat4 mProjection;
    glm::mat4 mView;
    glm::mat4 mModel;
    glm::mat4 mMvp;

    GLuint mShaderMvp;

    void flushEvents();
    void draw();

};

#endif // SCENE_H
