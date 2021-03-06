#ifndef LABEL_H
#define LABEL_H

#include "rendercontext.h"

#include <string>
#include <vector>

#include <GL/glew.h>

#include <glm/glm.hpp>

#include <pango/pangocairo.h>

class LabelFactory;

class Label
{
public:
    Label(LabelFactory *factory);
    ~Label();

    static void init();

    void setFontFace(const std::string &fontFace);
    void setText(const std::string &text);
    void setColor(glm::vec4 color);

    // call this to make your settings changes take effect
    void update();

    void draw(const RenderContext &renderContext);

    // actual text width
    int mWidth;
    int mHeight;

private:

    GLuint mVertexArray;
    GLuint mTextureId;
    GLuint mVertexBuffer;
    GLuint mTexCoordBuffer;

    glm::vec4 mColor;

    std::vector<unsigned char> mImgBuffer;
    cairo_surface_t *mSurface;
    cairo_t *mCairoContext;
    PangoLayout *mLayout;

    int mSurfaceWidth;
    int mSurfaceHeight;

    LabelFactory *mFactory;

    void maybeResize();
};

#endif // LABEL_H
