#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include "settings.h"

extern Settings gSettings;

#define GL_CALL(CALL) CALL;checkGLError(__LINE__)

class Mset
{
public:
    int gRes = 0;
    int gProgress = 100;
    float frameDuration = 400;
    bool init();
    bool restart(int r = gSettings.minRes);
    bool iterate();
    bool iterating();
    void paint();
    void close();
    void saveFrame(const char* f);
    GLuint LoadShaders(bool feedback, const char* vertex_file_path, const char* fragment_file_path);
    int gPrecision = 0;
    int gDrawnPoints = 0;
    int gNoPoints = 0;

private:
    void checkGLError(int l);
    bool initGL();
    void tuneBatch();
    const GLfloat gGenericBD[12] =
    {
        -1.0f, -1.0f,
            1.0f, -1.0f,
            -1.0f,  1.0f,
            -1.0f,  1.0f,
            1.0f, -1.0f,
            1.0f,  1.0f
    };
    //Graphics program
    GLuint gGenericVertexArray = 0;
    GLuint gTexturePID[2] = { 0,0 };
    GLuint gScreenPID = 0;
    GLuint gTexPassPID = 0;
    GLint gCentreLocation[2] = { -1,-1 };
    GLint gScaleLocation[2] = { -1,-1 };
    GLint gColMapLocation = -1;
    GLint gParamsLocation[2] = {-1,-1};
    GLint gScrParamLocation = -1;
    GLint gPaintTexLocation = -1;
    GLint gCalcTexLocation[2] = {-1,-1};
//    GLint gCalcRectLocation[2] = { -1,-1 };
    GLuint gVertexBufferObject = 0;
    GLuint gTextureFrameBuffer = 0;
    int gTargetTexture = 0;
    GLuint gTexture[2] = { 0,0 };
    int gPrevRes = 0;

    bool gInitialised = false;
    int gXBatchSize = 0;
    int gYBatchSize = 0;
    int gDrawnTop = -1;
    int gDrawnBottom = -1;
    int gDrawnLeft = -1;
    int gDrawnRight = -1;
//    int gIndStep = 0;
    GLenum gTextureDrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    GLint gTexEmpty[4] = { -1,-1,-1,-1 };
};