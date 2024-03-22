#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "settings.h"
#include "quad.h"

extern Settings gSettings;

#define GL_CALL(CALL) CALL;checkGLError(__LINE__)

enum State {
    STATE_NONE,
    STATE_MANDEL,
    STATE_JULIA,
    NUM_STATES
};

class Mset
{
public:
    int gRes = 0;
    float frameDuration = 400;
    int progress()
    {
        return gDrawnPoints * 100 / gNoPoints;
    }
    bool init();
    bool restart(int r = gSettings.minRes);
    bool iterate();
    bool offsetJulia(glm::vec2);
    bool changeState(State);
    const State getState() const { return gState; }
    bool iterating();
    bool zoomIn(float ze = gSettings.zoomExp + gSettings.zoomFraction);
    bool zoomOut(float ze = gSettings.zoomExp - gSettings.zoomFraction);
    bool shift(int x, int y);
    void paint();
    void close();
    void saveFrame(const char* f);
    GLuint LoadShaders(bool feedback, const char* vertex_file_path, const char* fragment_file_path);
    int gPrecision = 0;
    glm::vec2 gJuliaOffset = { -0.618, 0.0 };
    

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
    GLuint gTextureJuliaPID[2] = { 0,0 };
    GLuint gScreenPID = 0;
    GLuint gTexPassPID = 0;
    GLint gBLLocation[2] = { -1,-1 };
    GLint gStepLocation[2] = { -1,-1 };
    GLint gColMapLocation = -1;
    GLint gParamsLocation[2] = {-1,-1};
    GLint gScrParamLocation = -1;
    GLint gPaintTexLocation = -1;
    GLint gCalcTexLocation[2] = {-1,-1};
    GLint gPassParamsLocation = -1;
    GLint gPaintZoomLocation = -1;

    GLint gJuliaLocation[2] = { -1,-1 };
    GLint gBLLocationJulia[2] = { -1,-1 };
    GLint gStepLocationJulia[2] = { -1,-1 };
    GLint gParamsLocationJulia[2] = { -1,-1 };
    GLint gCalcTexLocationJulia[2] = { -1,-1 };

    GLuint gVertexBufferObject = 0;
    GLuint gTextureFrameBuffer = 0;
    int gTargetTexture = 1;
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

    Quad bottom = Quad(0.0);
    Quad left = Quad(0.0);
    Quad step = Quad(0, 0);
    
    float gTextZoomExp = 0.0;
    float gPaintZoom = 0.0;
    double gScaler = 0.0;
    

    int gDrawnPoints = 0;
    int gNoPoints = 1;
    int gProgress = 100;

    State gState = STATE_MANDEL;
};