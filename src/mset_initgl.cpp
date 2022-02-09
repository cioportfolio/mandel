#include "globals.h"
#include "mset.h"

bool Mset::initGL()
{
    //Success flag
    bool success = true;


    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &gVBO);
    checkGLError(__LINE__);

    glGenBuffers(1, &gVAB);
    checkGLError(__LINE__);

    glGenBuffers(1, &gTFB);
    checkGLError(__LINE__);

    glGenBuffers(1, &gIBO);
    checkGLError(__LINE__);

    glGenTextures(2, gTexture);
    checkGLError(__LINE__);

    glGenVertexArrays(1, &gTextureVA);
    checkGLError(__LINE__);

    glBindVertexArray(gTextureVA);
    checkGLError(__LINE__);

    glEnableVertexAttribArray(0);
    checkGLError(__LINE__);

    glBindBuffer(GL_ARRAY_BUFFER, gVBO);
    checkGLError(__LINE__);

    glVertexAttribIPointer(
        0,		  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        2,		  // size
        GL_INT, // type
//				GL_FALSE, // normalized?
0,		  // stride
(void*)0 // array buffer offset
);
    checkGLError(__LINE__);

    glBindTexture(GL_TEXTURE_2D, gTexture[0]);
    checkGLError(__LINE__);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, gSettings.winWidth, gSettings.winHeight, 0,GL_RED_INTEGER, GL_UNSIGNED_BYTE, 0);
    checkGLError(__LINE__);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    checkGLError(__LINE__);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    checkGLError(__LINE__);

    glBindTexture(GL_TEXTURE_2D, gTexture[1]);
    checkGLError(__LINE__);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, gSettings.winWidth, gSettings.winHeight, 0,GL_RED_INTEGER, GL_UNSIGNED_BYTE, 0);
    checkGLError(__LINE__);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    checkGLError(__LINE__);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    checkGLError(__LINE__);

    glGenFramebuffers(1, &gFramebuffer);
    checkGLError(__LINE__);

    glBindFramebuffer(GL_FRAMEBUFFER, gFramebuffer);
    checkGLError(__LINE__);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gTexture[0], 0);
   checkGLError(__LINE__);

//    glDrawBuffer(GL_COLOR_ATTACHMENT0);
//    glClearBufferiv(GL_COLOR, 0, gTexEmpty);
//    checkGLError(__LINE__);

    glDrawBuffers(2, gDrawBuffers);
    checkGLError(__LINE__);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) return false;

//    glBindFramebuffer(GL_FRAMEBUFFER, gFramebuffer);
//    glViewport(0,0,gSettings.winWidth/2, gSettings.winHeight/2);

    glGenBuffers(1, &gScreenBO);
    checkGLError(__LINE__);

    glBindBuffer(GL_ARRAY_BUFFER, gScreenBO);
    checkGLError(__LINE__);

    glBufferData(GL_ARRAY_BUFFER, sizeof(gScreenBD), gScreenBD, GL_STATIC_DRAW);
    checkGLError(__LINE__);

//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    glViewport(0,0,gSettings.winWidth, gSettings.winHeight);

    glGenVertexArrays(1, &gScreenVA);
    checkGLError(__LINE__);

    glBindVertexArray(gScreenVA);
    checkGLError(__LINE__);

    glEnableVertexAttribArray(0);
    checkGLError(__LINE__);

    glBindBuffer(GL_ARRAY_BUFFER, gScreenBO);
    checkGLError(__LINE__);

    glVertexAttribPointer(
        0,		  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        2,		  // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,		  // stride
        (void *)0 // array buffer offset
    );
    checkGLError(__LINE__);

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    gScreenPID = LoadShaders(false, "res/shaders/passthroughshader.txt", "res/shaders/textureshader.txt");

    glUseProgram(gScreenPID);
    checkGLError(__LINE__);

    gTextureLocation = glGetUniformLocation(gScreenPID, "tex");
    checkGLError(__LINE__);
    if (gTextureLocation == -1)
    {
        gLog("Failed to get texture location\n");
        success = false;
        return success;
    }
    checkGLError(__LINE__);

    glUniform1i(gTextureLocation, 0);
    checkGLError(__LINE__);

    gScrParamLocation = glGetUniformLocation(gScreenPID, "params");
    checkGLError(__LINE__);
    if (gScrParamLocation == -1)
    {
        gLog("Failed to get params locations\n");
        success = false;
        return success;
    }

    gColMapLocation = glGetUniformLocation(gScreenPID, "colmap");
    checkGLError(__LINE__);
    if (gColMapLocation == -1)
    {
        gLog("Failed to get colour map locations\n");
        success = false;
        return success;
    }

    gProgramID[0] = LoadShaders(true, gSettings.lowShader.c_str(), "res/shaders/tfshader.txt");
    checkGLError(__LINE__);

    gProgramID[1] = LoadShaders(true, gSettings.highShader.c_str(), "res/shaders/tfshader.txt");
    checkGLError(__LINE__);

    gCentreLocation = glGetUniformLocation(gProgramID[0], "centre");
    checkGLError(__LINE__);
    if (gCentreLocation == -1)
    {
        gLog("Failed to get centre location\n");
        success = false;
        return success;
    }

    gScaleLocation = glGetUniformLocation(gProgramID[0], "scale");
    checkGLError(__LINE__);
    if (gScaleLocation == -1)
    {
        gLog("Failed to get scale locations\n");
        success = false;
        return success;
    }

    gParamsLocation = glGetUniformLocation(gProgramID[0], "params");
    checkGLError(__LINE__);
    if (gParamsLocation == -1)
    {
        gLog("Failed to get params locations\n");
        success = false;
        return success;
    }

    gInTexLocation = glGetUniformLocation(gProgramID[0], "inTex");
    checkGLError(__LINE__);
    if (gInTexLocation == -1)
    {
        gLog("Failed to get inTex location\n");
        success = false;
        return success;
    }

    glUseProgram(gProgramID[0]);
    checkGLError(__LINE__);

    glUniform1i(gInTexLocation, 0);
    checkGLError(__LINE__);

    glUseProgram(gProgramID[1]);
    checkGLError(__LINE__);

    glUniform1i(gInTexLocation, 0);
    checkGLError(__LINE__);

    return buildWinData();
}
