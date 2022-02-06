#ifndef MSET_H_INCLUDED
#define MSET_H_INCLUDED
#define GLEW_STATIC
#include <GL/glew.h>
#include "settings.h"

extern Settings gSettings;

class Mset
{
public:
    int gProgram = 0;
    int gRes = 0;
    int gProgress = 100;
    float frameDuration = 400;
    bool init();
    bool precision();
    bool restart(int r = gSettings.minRes);
    bool resize();
    bool iterate();
    bool iterating();
    void paint();
    void close();
    void saveFrame(const char *f);
    GLuint LoadShaders(bool feedback, const char * vertex_file_path,const char * fragment_file_path);
    /*    void GLAPIENTRY MessageCallback(GLenum source,
                                        GLenum type,
                                        GLuint id,
                                        GLenum severity,
                                        GLsizei length,
                                        const GLchar* message);*/
private:
    void checkGLError(int l);
    bool initGL();
    bool buildWinData();
    void tuneBatch();
    const GLfloat gScreenBD[12] =
    {
        -1.0f, -1.0f,
            1.0f, -1.0f,
            -1.0f,  1.0f,
            -1.0f,  1.0f,
            1.0f, -1.0f,
            1.0f,  1.0f
        };
//Graphics program
    GLuint gTextureVA = 0;
    GLuint gScreenVA = 0;
    GLuint gProgramID[2] = {0,0};
    GLuint gScreenPID = 0;
    GLint gCentreLocation = -1;
    GLint gScaleLocation = -1;
    GLint gColMapLocation = -1;
    GLint gParamsLocation = -1;
    GLint gScrParamLocation = -1;
    GLint gTextureLocation = -1;
    GLint gInTexLocation = -1;

    GLuint gVBO = 0;
    GLuint gScreenBO = 0;
    GLuint gIBO = 0;
    GLuint gVAB = 0;
    GLuint gTFB = 0;
    GLuint gFramebuffer = 0;
    int gOutTexture = 0;
    GLuint gTexture[2] = {0,0};
    GLenum gDrawBuffers[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    GLint gTexEmpty[4] = {-1,-1,-1,-1};
    int gNoVertices = 0;
    int gNoCoords = 0;
    GLint *gVBD = nullptr;
    GLuint *gIBD = nullptr;
    int gPrevRes=0;
    int gNoIndices=0;
    bool gInitialised=false;
    int gBatch=0;
    int gIndStep=0;
};

#endif

