#define _CRT_SECURE_NO_WARNINGS

#include "globals.h"
#include "mset.h"
#include "quad.h"
#include <sys/types.h>
#include <time.h>
#include <sys/timeb.h>

bool Mset::restart()
{
    if (!gInitialised) return false;
    gRes = gSettings.minRes;
    gPrevRes = gRes*2;
    gBatch=0;
    gNoIndices=0;
    glBindFramebuffer(GL_FRAMEBUFFER, gFramebuffer);
    glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, gTexture[gOutTexture], 0);
    glDrawBuffers(2, gDrawBuffers);
    glClearBufferiv(GL_COLOR, 1, gTexEmpty);
    checkGLError(__LINE__);
    bool highPrec = (gSettings.scaler.h < gSettings.quadZoom);
    if (highPrec != (gM.gProgram == 1))
    {
        gM.precision();
    }
    return true;
}

bool Mset::resize()
{
    if (!gInitialised) return false;
//    gSettings.scalei = gSettings.scaler.mul(Quad((double)gSettings.winHeight/ gSettings.winWidth));

    glBindTexture(GL_TEXTURE_2D, gTexture[0]);
    checkGLError(__LINE__);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, gSettings.winWidth, gSettings.winHeight, 0,GL_RED_INTEGER, GL_UNSIGNED_BYTE, 0);
    checkGLError(__LINE__);

    glBindTexture(GL_TEXTURE_2D, gTexture[1]);
    checkGLError(__LINE__);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, gSettings.winWidth, gSettings.winHeight, 0,GL_RED_INTEGER, GL_UNSIGNED_BYTE, 0);
    checkGLError(__LINE__);

    glBindFramebuffer(GL_FRAMEBUFFER, gFramebuffer);
    glViewport(0,0,gSettings.winWidth,gSettings.winHeight);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, gSettings.winWidth, gSettings.winHeight);

    buildWinData();

    //printf("Finished sizing window event (%i,%i)\n", gSettings.winWidth, gSettings.winHeight);
    return true;
}

bool Mset::iterating()
{
    return (gPrevRes > 1 || gBatch < gNoIndices);
}

void Mset::tuneBatch()
{
        gIndStep = gNoIndices;
        long long total_iter = (long long) gNoIndices * gSettings.thresh * (1 + (gSettings.highCapFactor - 1)*gProgram);
        long long target_iter = gSettings.frameCap * (gSettings.durationTarget / frameDuration);
        while (total_iter > target_iter && gIndStep > 1)
        {
            total_iter /= 2;
            gIndStep /= 2;
        }
}

bool Mset::iterate()
{
    if (!gInitialised) return false;
    if (gBatch >= gNoIndices && gRes != gPrevRes)
    {
        int pointWidth = gSettings.winWidth / gRes;
        int pointHeight = gSettings.winHeight / gRes;
        gNoIndices = pointWidth * pointHeight;
        if (gIBD != nullptr)
        {
            delete[] gIBD;
        }
        gIBD = new GLuint[gNoIndices];
        int i = 0;
        int ystep = gSettings.winWidth;
        for (int v = 0; v < pointHeight; v++)
        {
            int y = (v+1)/2*((v%2)*2-1)+(pointHeight-1)/2;
            int basey = y * gRes * ystep;
            for (int w = 0; w < pointWidth; w++)
            {
                int x = (w+1)/2*((w%2)*2-1)+(pointWidth-1)/2;
                int base = x * gRes + basey;
                if (i < gNoIndices)
                {
                    gIBD[i++] = base;
                }
                else
                {
                    printf("Indices %i, i %i\nOverflow gIBD. Window (%i,%i) loop (%i,%i) gRes %i\n", gNoIndices, i, gSettings.winWidth, gSettings.winHeight, x, y, gRes);
                }
            }
        }
        glBindVertexArray(gTextureVA);
        checkGLError(__LINE__);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIBO);
        checkGLError(__LINE__);

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * gNoIndices, gIBD, GL_STATIC_DRAW);
        checkGLError(__LINE__);
        tuneBatch();
        gBatch=0;
    }
    if (gBatch < gNoIndices)
    {
        struct _timeb startTime, endTime;
        _ftime(&startTime);
        glBindFramebuffer(GL_FRAMEBUFFER, gFramebuffer);
        glFramebufferTexture(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, gTexture[gOutTexture], 0);
        glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gTexture[1-gOutTexture], 0);
        glReadBuffer(GL_COLOR_ATTACHMENT1);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glBlitFramebuffer(0, 0, gSettings.winWidth, gSettings.winHeight, 0, 0, gSettings.winWidth, gSettings.winHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        gOutTexture = 1 - gOutTexture;

        gProgress = (int)((100.0*gBatch/gNoIndices));

        glBindVertexArray(gTextureVA);
        checkGLError(__LINE__);

        glUseProgram(gProgramID[gProgram]);
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
            (void *)0 // array buffer offset
        );
        checkGLError(__LINE__);

        glActiveTexture(GL_TEXTURE0);
        checkGLError(__LINE__);

        glBindTexture(GL_TEXTURE_2D, gTexture[1-gOutTexture]);
        checkGLError(__LINE__);

        glUniform1i(gInTexLocation, 0);
        checkGLError(__LINE__);

        glUniform4d(gCentreLocation, gSettings.centrer.h, gSettings.centrer.l, gSettings.centrei.h, gSettings.centrei.l);
        checkGLError(__LINE__);

        gSettings.scalei = gSettings.scaler.mul(Quad((double)gSettings.winHeight / gSettings.winWidth));
        glUniform4d(gScaleLocation, gSettings.scaler.h, gSettings.scaler.l, gSettings.scalei.h, gSettings.scalei.l);
        checkGLError(__LINE__);

        glUniform4i(gParamsLocation, gSettings.thresh,gSettings.winWidth,gSettings.winHeight,gRes);
        checkGLError(__LINE__);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIBO);
        checkGLError(__LINE__);

        int noElements = gIndStep;
        if (gBatch+noElements >= gNoIndices) noElements = gNoIndices - gBatch;

        glBindFramebuffer(GL_FRAMEBUFFER, gFramebuffer);
        checkGLError(__LINE__);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gTexture[gOutTexture], 0);
        checkGLError(__LINE__);

        glDrawBuffers(2, gDrawBuffers);
        checkGLError(__LINE__);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            printf("Framebuffer not complete at line %i\n",__LINE__);

        glViewport(0,0,gSettings.winWidth,gSettings.winHeight);

        glDrawElements(GL_POINTS, noElements, GL_UNSIGNED_INT, (void *)(gBatch * sizeof(GLuint)));
        checkGLError(__LINE__);

        glDisableVertexAttribArray(0);
        checkGLError(__LINE__);
        glFinish();

        gBatch += gIndStep;
        _ftime(&endTime);
        endTime.time -= startTime.time;
        float newDuration = 1000.0 * endTime.time + endTime.millitm - startTime.millitm;
        frameDuration = (1.0 - gSettings.durationFilter)*frameDuration + gSettings.durationFilter * newDuration;
        tuneBatch();
        if (gBatch >= gNoIndices)
        {
            gNoIndices=0;
            gPrevRes = gRes;
            if (gRes > 1)
                gRes /= 2;
            //else
                //gLog("Finished");
        }
    }
    return true;
}

void Mset::paint()
{
    if (!gInitialised) return;
    glBindVertexArray(gScreenVA);
    checkGLError(__LINE__);

    glUseProgram(gScreenPID);
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

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    checkGLError(__LINE__);

    glViewport(0,0,gSettings.winWidth, gSettings.winHeight);
    checkGLError(__LINE__);

    glActiveTexture(GL_TEXTURE0);
    checkGLError(__LINE__);

    glBindTexture(GL_TEXTURE_2D, gTexture[gOutTexture]);
    checkGLError(__LINE__);

    glUniform1i(gTextureLocation, 0);
    checkGLError(__LINE__);

    glUniform4i(gScrParamLocation, gSettings.thresh, gRes, gSettings.winWidth, gSettings.winHeight);
    checkGLError(__LINE__);

    glUniform2f(gColMapLocation, gSettings.baseHue, gSettings.hueScale);
    checkGLError(__LINE__);

    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, 2*3); // 12*3 indices starting at 0 -> 12 triangles -> 6 squagRes
    checkGLError(__LINE__);

    glDisableVertexAttribArray(0);
    checkGLError(__LINE__);
}

bool Mset::buildWinData()
{
    bool success = true;

    gNoVertices = gSettings.winWidth * gSettings.winHeight;
    gNoCoords = gNoVertices * 2;

    if (gVBD != nullptr)
    {
        //gLog("About to delete gVBD\n");
        delete[] gVBD;
    }
    //gLog("About to create gVBD\n");
    gVBD = new GLint[gNoCoords];
    int i = 0;
    for (int y = 0; y < gSettings.winHeight; y++)
    {
        for (int x = 0; x < gSettings.winWidth; x++)
        {
            gVBD[i++] = x;
            gVBD[i++] = y;
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, gVBO);
    checkGLError(__LINE__);

    glBufferData(GL_ARRAY_BUFFER, sizeof(GLint) * gNoCoords, gVBD, GL_STATIC_DRAW);
    checkGLError(__LINE__);

    glBindFramebuffer(GL_FRAMEBUFFER, gFramebuffer);
    glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, gTexture[gOutTexture], 0);
    glDrawBuffers(2, gDrawBuffers);
    glClearBufferiv(GL_COLOR, 1, gTexEmpty);
    checkGLError(__LINE__);

    return success;
}
