#define _CRT_SECURE_NO_WARNINGS

#include "globals.h"
#include "mset.h"
#include <sys/types.h>
#include <time.h>
#include <sys/timeb.h>

bool Mset::zoomIn()
{
    float lastZoom = gZoomExp;
    gZoomExp += gSettings.zoomFraction;
    if (floor(lastZoom) != floor(gZoomExp))
    {
        restart();
        iterate();
    }
    return true;
}

bool Mset::zoomOut()
{
    float lastZoom = gZoomExp;
    gZoomExp -= gSettings.zoomFraction;
    if (floor(lastZoom) != floor(gZoomExp))
    {
        restart();
        iterate();
    }
    return true;
}

bool Mset::restart(int r)
{
    if (!gInitialised) return false;
    gRes = r;
    gPrevRes = gRes*2;
    gDrawnPoints=0;
    gDrawnTop = -1;
    gDrawnBottom = -1;
    gDrawnLeft = -1;
    gDrawnRight = -1;
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

    gSettings.scaler = Quad(pow(2.0, -floor(gZoomExp)));
    gSettings.scalei = gSettings.scaler.mul(Quad((double)gSettings.winHeight / gSettings.winWidth));
    bottom = gSettings.centrei.add(gSettings.scalei.mul(Quad(-1.0)));
    left = gSettings.centrer.add(gSettings.scaler.mul(Quad(-1.0)));
    step = gSettings.scaler.mul(Quad(2.0 / gSettings.winWidth));

    return true;
}

bool Mset::iterating()
{
    return (gPrevRes > 1 || gDrawnPoints < gNoPoints);
}

void Mset::tuneBatch()
{
    gNoPoints = gSettings.winWidth / gRes * gSettings.winHeight / gRes;
    long long workEst = (long long)gNoPoints * gSettings.thresh * (1 + (gSettings.highCapFactor - 1) * gPrecision);
    long long capacityEst = gSettings.frameCap * gSettings.durationTarget / frameDuration;
    int batches = workEst < capacityEst * 2 ? 1 : workEst / capacityEst;
//    gXBatchSize = gSettings.winWidth / gRes < batches * 2 ? 1 : gSettings.winWidth / gRes / batches;
    gYBatchSize = gSettings.winHeight / gRes < batches * 2 ? 1 : gSettings.winHeight / gRes / batches;
}

bool Mset::iterate()
{
    if (!gInitialised) return false;


    if (gDrawnPoints >= gNoPoints && gRes != gPrevRes)
    {
//        printf("Next pass height %d \n", gSettings.winHeight / gRes);
        gTargetTexture = 1 - gTargetTexture;
        tuneBatch();
        gDrawnPoints=0;
        gDrawnTop = -1;
        gDrawnBottom = -1;
        gDrawnLeft = -1;
        gDrawnRight = -1;
        GL_CALL(glBindTexture(GL_TEXTURE_2D, gTexture[gTargetTexture]));
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, gSettings.winWidth / gRes, gSettings.winHeight / gRes, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, 0));
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, gTextureFrameBuffer));
        GL_CALL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gTexture[gTargetTexture], 0));
        GL_CALL(glDrawBuffers(1, gTextureDrawBuffers));
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            printf("Framebuffer not complete at line %i\n", __LINE__);

        GL_CALL(glBindVertexArray(gGenericVertexArray));

        GL_CALL(glUseProgram(gTexPassPID));
        GL_CALL(glUniform4i(gPassParamsLocation, 0, 0, 1, 2));


        GL_CALL(glActiveTexture(GL_TEXTURE0));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, gTexture[1 - gTargetTexture]));

        GL_CALL(glViewport(0, 0, gSettings.winWidth / gRes, gSettings.winHeight / gRes));

        GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 6));
    }
    if (gDrawnPoints < gNoPoints)
    {
        struct _timeb startTime, endTime;
        _ftime(&startTime);
        gProgress = (int)((100.0 * gDrawnPoints / gNoPoints));
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, gTextureFrameBuffer));
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            printf("Framebuffer not complete at line %i\n", __LINE__);

        GL_CALL(glBindVertexArray(gGenericVertexArray));

        GL_CALL(glUseProgram(gTexturePID[gPrecision]));
        GL_CALL(glUniform4d(gBLLocation[gPrecision], left.h, left.l, bottom.h, bottom.l));
//        gSettings.scalei = gSettings.scaler.mul(Quad((double)gSettings.winHeight / gSettings.winWidth));
        GL_CALL(glUniform2d(gStepLocation[gPrecision], step.h, step.l));
        GL_CALL(glUniform4i(gParamsLocation[gPrecision], gSettings.thresh, gSettings.winWidth, gSettings.winHeight, gRes));

        GL_CALL(glActiveTexture(GL_TEXTURE0));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, gTexture[1-gTargetTexture]));

        GL_CALL(glViewport(0,0,gSettings.winWidth/gRes,gSettings.winHeight/gRes));

        GL_CALL(glEnable(GL_SCISSOR_TEST));
        int yBatchStart = -1;
        int yBatchEnd = -1;
        if (gDrawnBottom < gSettings.winHeight / gRes - gDrawnTop)
        {
            if (gDrawnTop == -1)
            {
                yBatchStart = gSettings.winHeight / gRes / 2;
            }
            else
            {
                yBatchStart = gDrawnTop;
            }
            yBatchEnd = yBatchStart + gYBatchSize;
            if (yBatchEnd > gSettings.winHeight / gRes) {
                yBatchEnd = gSettings.winHeight / gRes;
            }
//            printf("Top batch %d - %d\n", yBatchStart, yBatchEnd);
        }
        else
        {
            if (gDrawnBottom == -1)
            {
                yBatchEnd = gSettings.winHeight / gRes / 2;
            }
            else
            {
                yBatchEnd = gDrawnBottom;
            }
            yBatchStart = yBatchEnd - gYBatchSize;
            if (yBatchStart < 0) {
                yBatchStart = 0;
            }
//            printf("Bottom batch %d - %d\n", yBatchStart, yBatchEnd);
        }

        GL_CALL(glScissor(0, yBatchStart, gSettings.winWidth / gRes, yBatchEnd - yBatchStart));
        GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 6));
        GL_CALL(glFinish());
        GL_CALL(glDisable(GL_SCISSOR_TEST));

        if (gDrawnBottom == -1 || yBatchStart < gDrawnBottom)
            gDrawnBottom = yBatchStart;
        if (gDrawnTop == -1 || yBatchEnd > gDrawnTop)
            gDrawnTop = yBatchEnd;
        gDrawnPoints += (yBatchEnd - yBatchStart) * gSettings.winWidth / gRes;

        _ftime(&endTime);
        endTime.time -= startTime.time;
        float newDuration = 1000.0 * endTime.time + endTime.millitm - startTime.millitm;
        frameDuration = (1.0 - gSettings.durationFilter)*frameDuration + gSettings.durationFilter * newDuration;
        tuneBatch();
        if (gDrawnPoints >= gNoPoints)
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

    int paintRes = gDrawnPoints < gNoPoints ? gRes : gPrevRes;
    float zoom = pow(2.0, gZoomExp - floor(gZoomExp));
    GL_CALL(glUniform4i(gScrParamLocation, gSettings.thresh, paintRes, gSettings.winWidth, gSettings.winHeight));
    GL_CALL(glUniform2f(gColMapLocation, gSettings.baseHue, gSettings.hueScale));
    GL_CALL(glUniform1f(gPaintZoomLocation, zoom));
    
    GL_CALL(glViewport(0, 0, gSettings.winWidth, gSettings.winHeight));

    GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 2*3));
}
