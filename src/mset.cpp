#define _CRT_SECURE_NO_WARNINGS

#include "globals.h"
#include "mset.h"
#include "quad.h"
#include <sys/types.h>
#include <time.h>
#include <sys/timeb.h>

bool Mset::restart(int r)
{
    if (!gInitialised) return false;
    gRes = r;
    gPrevRes = gRes*2;
    gBatch=0;
    tuneBatch();

    for (int i = 0; i < 2; i++)
    {
        GL_CALL(glBindTexture(GL_TEXTURE_2D, gTexture[i]));
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, gSettings.winWidth / gSettings.minRes, gSettings.winHeight / gSettings.minRes, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, 0));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, gTextureFrameBuffer));
        GL_CALL(glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gTexture[i], 0));
        GL_CALL(glClearBufferiv(GL_COLOR, 0, gTexEmpty));
    }

    gM.gPrecision = (gSettings.scaler.h < gSettings.quadZoom);
    return true;
}

bool Mset::iterating()
{
    return (gPrevRes > 1 || gBatch < gNoBatches);
}

void Mset::tuneBatch()
{
        long long workEst = (long long) gSettings.winWidth / gRes * gSettings.winHeight / gRes * gSettings.thresh * (1 + (gSettings.highCapFactor - 1)*gPrecision);
        long long capacityEst = gSettings.frameCap * gSettings.durationTarget / frameDuration;
        gNoBatches = workEst < capacityEst * 2 ? 1 : workEst / capacityEst;
}

bool Mset::iterate()
{
    if (!gInitialised) return false;


    if (gBatch >= gNoBatches && gRes != gPrevRes)
    {
        gTargetTexture = 1 - gTargetTexture;
        tuneBatch();
        gBatch=0;
    }
    if (gBatch < gNoBatches)
    {
        struct _timeb startTime, endTime;
        _ftime(&startTime);
        gProgress = (int)((100.0 * gBatch / gNoBatches));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, gTexture[gTargetTexture]));
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, gSettings.winWidth / gRes, gSettings.winHeight / gRes, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, 0));
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, gTextureFrameBuffer));
        GL_CALL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gTexture[gTargetTexture], 0));
        GL_CALL(glDrawBuffers(1, gTextureDrawBuffers));
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            printf("Framebuffer not complete at line %i\n", __LINE__);

        GL_CALL(glBindVertexArray(gGenericVertexArray));

        GL_CALL(glUseProgram(gTexturePID[gPrecision]));
        GL_CALL(glUniform4d(gCentreLocation[gPrecision], gSettings.centrer.h, gSettings.centrer.l, gSettings.centrei.h, gSettings.centrei.l));
        gSettings.scalei = gSettings.scaler.mul(Quad((double)gSettings.winHeight / gSettings.winWidth));
        GL_CALL(glUniform4d(gScaleLocation[gPrecision], gSettings.scaler.h, gSettings.scaler.l, gSettings.scalei.h, gSettings.scalei.l));
        GL_CALL(glUniform4i(gParamsLocation[gPrecision], gSettings.thresh, gSettings.winWidth, gSettings.winHeight, gRes));

        GL_CALL(glActiveTexture(GL_TEXTURE0));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, gTexture[1-gTargetTexture]));

        GL_CALL(glViewport(0,0,gSettings.winWidth/gRes,gSettings.winHeight/gRes));

        GL_CALL(glEnable(GL_SCISSOR_TEST));
        int xBatchSize = gSettings.winWidth/gRes < 2*gNoBatches ? 1: gSettings.winWidth / gRes / gNoBatches;
        int xBatches = gSettings.winWidth / gRes / xBatchSize;
        int xBatchStart = gBatch * xBatchSize;
        int xBatchEnd = gBatch * xBatchSize + xBatchSize;
        if (xBatchEnd <= gSettings.winWidth / gRes)
        {
            if (xBatchEnd > gSettings.winWidth / gRes)
            {
                xBatchEnd = gSettings.winWidth / gRes;
                gBatch = gNoBatches;
            }
            GL_CALL(glScissor(xBatchStart, 0, xBatchEnd, gSettings.winHeight / gRes));
            GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 6));
            GL_CALL(glFinish());
            GL_CALL(glDisable(GL_SCISSOR_TEST));
        }
        else
        {
            gBatch = gNoBatches;
        }
        gBatch++;
        _ftime(&endTime);
        endTime.time -= startTime.time;
        float newDuration = 1000.0 * endTime.time + endTime.millitm - startTime.millitm;
        frameDuration = (1.0 - gSettings.durationFilter)*frameDuration + gSettings.durationFilter * newDuration;
//        tuneBatch();
        if (gBatch >= gNoBatches)
        {
            gPrevRes = gRes;
            if (gRes > 1)
                gRes /= 2;
        }
    } 
    return true;
}

void Mset::paint()
{
    if (!gInitialised) return;
    GL_CALL(glBindVertexArray(gGenericVertexArray));

    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));

    GL_CALL(glActiveTexture(GL_TEXTURE0));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, gTexture[gTargetTexture]));

    GL_CALL(glUseProgram(gScreenPID));

    int paintRes = gBatch < gNoBatches ? gRes : gPrevRes;
    GL_CALL(glUniform4i(gScrParamLocation, gSettings.thresh, paintRes, gSettings.winWidth, gSettings.winHeight));
    GL_CALL(glUniform2f(gColMapLocation, gSettings.baseHue, gSettings.hueScale));
    
    GL_CALL(glViewport(0, 0, gSettings.winWidth, gSettings.winHeight));

    GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 2*3));
}
