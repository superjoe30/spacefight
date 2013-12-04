#ifndef SCENE_H
#define SCENE_H


#include <glm/glm.hpp>

#include "shader.h"
#include "model.h"
#include "rendercontext.h"

#include "resourcebundle.h"

#include <SDL2/SDL.h>
#include <GL/glew.h>

#include "label.h"
#include "labelfactory.h"

class Scene
{
public:
    Scene();
    ~Scene();
    int start();
    void setMinFps(float fps);
private:
    bool mRunning = true;
    SDL_GLContext mContext;
    SDL_Window *mWindow;

    RenderContext m3DRenderContext;
    RenderContext m2DRenderContext;

    int mScreenWidth;
    int mScreenHeight;

    Model *mMonkeyModel;

    LabelFactory *mLabelFactory;

    float mMaxSpf;
    float mFps;
    Label *mFpsLabel;
    Label *mEngineLabel;

    std::vector<SDL_Joystick*> mJoysticks;

    glm::vec3 mCameraPosition;
    glm::vec3 mCameraUp; // direction vector
    glm::vec3 mCameraForward; // direction vector
    glm::vec3 mCameraVelocity;

    ResourceBundle mBundle;

    void flushEvents();
    void update(float dt, float dx);
    void draw();
    void initJoystick();
};

#endif // SCENE_H
