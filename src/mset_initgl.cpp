#include "globals.h"
#include "mset.h"

#define GL_GET_LOCATION(PID, NAME, VAR)                 \
    GL_CALL(glUseProgram(PID));                         \
    GL_CALL(VAR = glGetUniformLocation(PID, #NAME));    \
    if (VAR == -1)                                      \
    {                                                   \
        printf("Failed to get %s location\n", #NAME);   \
    }
//        success = false;                                
//        return success;                                 

bool Mset::initGL()
{
    //Success flag
    bool success = true;

    GL_CALL(glClearColor(0.0f, 0.0f, 0.4f, 0.0f));

    GL_CALL(glGenTextures(2, gTexture));
    for (int i = 0; i < 2; i++)
    {
        GL_CALL(glBindTexture(GL_TEXTURE_2D, gTexture[i]));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    }

    GL_CALL(glGenBuffers(1, &gVertexBufferObject));
    GL_CALL(glGenVertexArrays(1, &gGenericVertexArray));
    GL_CALL(glBindVertexArray(gGenericVertexArray));
    GL_CALL(glEnableVertexAttribArray(0));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(gGenericBD), gGenericBD, GL_STATIC_DRAW));
    GL_CALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0));

    GL_CALL(glGenFramebuffers(1, &gTextureFrameBuffer));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, gTextureFrameBuffer));
    GL_CALL(glDrawBuffers(1, gTextureDrawBuffers));

    GL_CALL(gScreenPID = LoadShaders(false, "res/shaders/vertexpassthrough.txt", "res/shaders/paintfragment.txt"));
    
    GL_GET_LOCATION(gScreenPID, tex, gPaintTexLocation);
    glUniform1i(gPaintTexLocation, 0);
    GL_GET_LOCATION(gScreenPID, zoom, gPaintZoomLocation);

    GL_GET_LOCATION(gScreenPID, params, gScrParamLocation);
    GL_GET_LOCATION(gScreenPID, colmap, gColMapLocation);

    GL_CALL(gTexturePID[0] = LoadShaders(true, "res/shaders/vertexpassthrough.txt", gSettings.lowShader.c_str()));
    GL_CALL(gTexturePID[1] = LoadShaders(true, "res/shaders/vertexpassthrough.txt", gSettings.highShader.c_str()));
    GL_CALL(gTextureJuliaPID[0] = LoadShaders(true, "res/shaders/vertexpassthrough.txt", gSettings.lowShaderJulia.c_str()));
    GL_CALL(gTextureJuliaPID[1] = LoadShaders(true, "res/shaders/vertexpassthrough.txt", gSettings.highShaderJulia.c_str()));
    GL_CALL(gTexPassPID = LoadShaders(true, "res/shaders/vertexpassthrough.txt", "res/shaders/calcfragmentpass.txt"));

    for (int i = 0; i < 2; i++)
    {
       
        GL_GET_LOCATION(gTexturePID[i], bottomLeft, gBLLocation[i]);                       
        GL_GET_LOCATION(gTexturePID[i], step, gStepLocation[i]);
        GL_GET_LOCATION(gTexturePID[i], params, gParamsLocation[i]);
        GL_GET_LOCATION(gTexturePID[i], inTex, gCalcTexLocation[i]);
//        GL_GET_LOCATION(gTexturePID[i], rect, gCalcRectLocation[i]);
        GL_CALL(glUseProgram(gTexturePID[i]));
        GL_CALL(glUniform1i(gCalcTexLocation[i], 0));


        GL_GET_LOCATION(gTextureJuliaPID[i], juliaOffset, gJuliaLocation[i]);
        GL_GET_LOCATION(gTextureJuliaPID[i], bottomLeft, gBLLocationJulia[i]);
        GL_GET_LOCATION(gTextureJuliaPID[i], step, gStepLocationJulia[i]);
        GL_GET_LOCATION(gTextureJuliaPID[i], params, gParamsLocationJulia[i]);
        GL_GET_LOCATION(gTextureJuliaPID[i], inTex, gCalcTexLocationJulia[i]);
        //        GL_GET_LOCATION(gTexturePID[i], rect, gCalcRectLocation[i]);
        GL_CALL(glUseProgram(gTextureJuliaPID[i]));
        GL_CALL(glUniform2d(gJuliaLocation[i], gJuliaOffset.x,gJuliaOffset.y));
        GL_CALL(glUniform1i(gCalcTexLocationJulia[i], 0));
    }

    int t = -1;
    GL_GET_LOCATION(gTexPassPID, params, gPassParamsLocation);
    GL_GET_LOCATION(gTexPassPID, inTex, t);
    GL_CALL(glUseProgram(gTexPassPID));
    GL_CALL(glUniform1i(t, 0));

    return success;
}
