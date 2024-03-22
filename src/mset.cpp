#define _CRT_SECURE_NO_WARNINGS

#include "globals.h"
#include "mset.h"
#include <sys/types.h>
#include <time.h>
#include <sys/timeb.h>

bool Mset::zoomIn(float newZoom)
{
    float lastZoom = gTextZoomExp;
    gSettings.zoomExp = newZoom;
    gTextZoomExp = floor(gSettings.zoomExp);
    gPaintZoom = pow(2.0, gSettings.zoomExp - gTextZoomExp);
    if (lastZoom > gTextZoomExp)
        zoomOut(newZoom);
    else if (lastZoom != gTextZoomExp) //if we have zoomed
    {
        //swap textures, and set that texture to be rendered to
        gTargetTexture = 1 - gTargetTexture;
        GL_CALL(glBindTexture(GL_TEXTURE_2D, gTexture[gTargetTexture]));
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, gSettings.winWidth, gSettings.winHeight, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, 0));
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, gTextureFrameBuffer));
        GL_CALL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gTexture[gTargetTexture], 0));
        GL_CALL(glDrawBuffers(1, gTextureDrawBuffers));
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            printf("Framebuffer not complete at line %i\n", __LINE__);

        GL_CALL(glBindVertexArray(gGenericVertexArray));

        GL_CALL(glUseProgram(gTexPassPID));
        GL_CALL(glUniform4i(gPassParamsLocation, gSettings.winWidth/4, gSettings.winHeight/4, 2, 1));

        GL_CALL(glActiveTexture(GL_TEXTURE0));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, gTexture[1 - gTargetTexture]));

        GL_CALL(glViewport(0, 0, gSettings.winWidth, gSettings.winHeight));
        GL_CALL(glClearBufferiv(GL_COLOR, 0, gTexEmpty));
        GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 6));

        //double resolution, reset drawn points
        gRes = gRes * 2;
        gPrevRes = gRes * 2;
        gDrawnPoints = 1;
        gNoPoints = 1;
        gDrawnTop = -1;
        gDrawnBottom = -1;
        gDrawnLeft = -1;
        gDrawnRight = -1;
//        tuneBatch();

        //check the depth and change precision level accordingly
        gScaler = pow(2.0, -gTextZoomExp);
        gM.gPrecision = (gScaler < gSettings.quadZoom);

        bottom = gSettings.centrei.add(Quad(-gScaler * gSettings.winHeight / gSettings.winWidth));
        left = gSettings.centrer.add(Quad(-gScaler));
        step = Quad(gScaler).mul(Quad(2.0 / gSettings.winWidth));
    }
    return true;
}

bool Mset::zoomOut(float newZoom)
{
    float lastZoom = gTextZoomExp;
    gSettings.zoomExp = newZoom;
    gTextZoomExp = floor(gSettings.zoomExp);
    gPaintZoom = pow(2.0, gSettings.zoomExp - gTextZoomExp);
    if (lastZoom < gTextZoomExp)
        zoomIn(newZoom);
    else if (lastZoom != gTextZoomExp)
    {
        //flips target texture to other texture, 0-1
        gTargetTexture = 1 - gTargetTexture;

        //sets up texture to be rendered to
        GL_CALL(glBindTexture(GL_TEXTURE_2D, gTexture[gTargetTexture]));
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, gSettings.winWidth, gSettings.winHeight, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, 0));
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, gTextureFrameBuffer));
        GL_CALL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gTexture[gTargetTexture], 0));
        GL_CALL(glDrawBuffers(1, gTextureDrawBuffers));
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            printf("Framebuffer not complete at line %i\n", __LINE__);

        GL_CALL(glBindVertexArray(gGenericVertexArray));

        GL_CALL(glUseProgram(gTexPassPID));
        GL_CALL(glUniform4i(gPassParamsLocation, -gSettings.winWidth / 2, -gSettings.winHeight / 2, 1, 2));

        GL_CALL(glActiveTexture(GL_TEXTURE0));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, gTexture[1 - gTargetTexture]));

        GL_CALL(glViewport(0, 0, gSettings.winWidth, gSettings.winHeight));
        GL_CALL(glClearBufferiv(GL_COLOR, 0, gTexEmpty));
        GL_CALL(glEnable(GL_SCISSOR_TEST));
        GL_CALL(glScissor(gSettings.winWidth / 4, gSettings.winHeight / 4, gSettings.winWidth / 2, gSettings.winHeight/2));
        GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 6));
        GL_CALL(glDisable(GL_SCISSOR_TEST));

        gRes = gSettings.minRes;
        gPrevRes = gRes * 2;
        gDrawnPoints = 1;
        gNoPoints = 1;
        gDrawnTop = -1;
        gDrawnBottom = -1;
        gDrawnLeft = -1;
        gDrawnRight = -1;
//        tuneBatch();
        gScaler = pow(2.0, -gTextZoomExp);
        gM.gPrecision = (gScaler < gSettings.quadZoom);

        bottom = gSettings.centrei.add(Quad(-gScaler * gSettings.winHeight / gSettings.winWidth));
        left = gSettings.centrer.add(Quad(-gScaler));
        step = Quad(gScaler).mul(Quad(2.0 / gSettings.winWidth));
    }
    return true;
}

bool Mset::shift(int x, int y)
{
    int textMoveX = x / gPaintZoom;
    int textMoveY = y / gPaintZoom;
    if (textMoveX != 0 || textMoveY != 0)
    {

        gSettings.centrer = gSettings.centrer.add(step.mul(Quad(textMoveX)));
        gSettings.centrei = gSettings.centrei.add(step.mul(Quad(textMoveY)));
        gTargetTexture = 1 - gTargetTexture;
        GL_CALL(glBindTexture(GL_TEXTURE_2D, gTexture[gTargetTexture]));
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, gSettings.winWidth, gSettings.winHeight, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, 0));
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, gTextureFrameBuffer));
        GL_CALL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gTexture[gTargetTexture], 0));
        GL_CALL(glDrawBuffers(1, gTextureDrawBuffers));
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            printf("Framebuffer not complete at line %i\n", __LINE__);

        GL_CALL(glBindVertexArray(gGenericVertexArray));

        GL_CALL(glUseProgram(gTexPassPID));
        GL_CALL(glUniform4i(gPassParamsLocation, textMoveX, textMoveY, 1, 1));

        GL_CALL(glActiveTexture(GL_TEXTURE0));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, gTexture[1 - gTargetTexture]));

        GL_CALL(glViewport(0, 0, gSettings.winWidth, gSettings.winHeight));
        GL_CALL(glClearBufferiv(GL_COLOR, 0, gTexEmpty));
        GL_CALL(glEnable(GL_SCISSOR_TEST));
        int sx = textMoveX < 0? -textMoveX: 0;
        int sy = textMoveY < 0? -textMoveY: 0;
        int sw = gSettings.winWidth - (textMoveX < 0? -textMoveX: textMoveX);
        int sh = gSettings.winHeight - (textMoveY < 0? -textMoveY: textMoveY);
        GL_CALL(glScissor(sx,sy,sw,sh));

        GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 6));
        GL_CALL(glDisable(GL_SCISSOR_TEST));

        gRes = gSettings.minRes;
        gPrevRes = gRes * 2;
        gDrawnPoints = 1;
        gNoPoints = 1;
        gDrawnTop = -1;
        gDrawnBottom = -1;
        gDrawnLeft = -1;
        gDrawnRight = -1;
//        tuneBatch();
        gScaler = pow(2.0, -gTextZoomExp);
        gM.gPrecision = (gScaler < gSettings.quadZoom);

        bottom = gSettings.centrei.add(Quad(-gScaler * gSettings.winHeight / gSettings.winWidth));
        left = gSettings.centrer.add(Quad(-gScaler));
        step = Quad(gScaler).mul(Quad(2.0 / gSettings.winWidth));

    }
    return true;
}

bool Mset::restart(int r)
{
    if (!gInitialised) return false;
    gRes = r;
    gPrevRes = gRes*2;
    gDrawnPoints=1;
    gNoPoints = 1;
    gDrawnTop = -1;
    gDrawnBottom = -1;
    gDrawnLeft = -1;
    gDrawnRight = -1;
    tuneBatch();

    for (int i = 0; i < 2; i++)
    {
        GL_CALL(glBindTexture(GL_TEXTURE_2D, gTexture[i]));
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, gSettings.winWidth, gSettings.winHeight, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, 0));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, gTextureFrameBuffer));
        GL_CALL(glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gTexture[i], 0));
        GL_CALL(glClearBufferiv(GL_COLOR, 0, gTexEmpty));
    }

    


    gTextZoomExp = floor(gSettings.zoomExp);
    gPaintZoom = pow(2.0, gSettings.zoomExp - gTextZoomExp);
    gScaler = pow(2.0, -gTextZoomExp);
    gM.gPrecision = (gScaler < gSettings.quadZoom);

    if (gState == STATE_JULIA) {
        gSettings.centrer = 0.0;
        gScaler *= 1.5;
    }
    else if (gState == STATE_MANDEL) {
        gSettings.centrer = -0.5;
    }
   
    bottom = gSettings.centrei.add(Quad(-gScaler * gSettings.winHeight / gSettings.winWidth));
    left = gSettings.centrer.add(Quad(-gScaler));
    step = Quad(gScaler).mul(Quad(2.0 / gSettings.winWidth));

    return true;
}

bool Mset::iterating()
{
    return (gPrevRes > 1 || gDrawnPoints < gNoPoints);
}

bool Mset::offsetJulia(glm::vec2 offset) {
    gJuliaOffset += offset;
    return true;
}

bool Mset::changeState(State state) {
    gState = state;
    return true;
}


void Mset::tuneBatch()
{
    gNoPoints = gSettings.winWidth * gSettings.winHeight;
    long long workEst = (long long)gNoPoints / gRes / gRes * gSettings.thresh * (1 + (gSettings.highCapFactor - 1) * gPrecision);
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
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, gSettings.winWidth, gSettings.winHeight, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, 0));
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, gTextureFrameBuffer));
        GL_CALL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gTexture[gTargetTexture], 0));
        GL_CALL(glDrawBuffers(1, gTextureDrawBuffers));
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            printf("Framebuffer not complete at line %i\n", __LINE__);

        GL_CALL(glBindVertexArray(gGenericVertexArray));

        GL_CALL(glUseProgram(gTexPassPID));
        GL_CALL(glUniform4i(gPassParamsLocation, 0, 0, 1, 1));

       


        GL_CALL(glActiveTexture(GL_TEXTURE0));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, gTexture[1 - gTargetTexture]));

        GL_CALL(glViewport(0, 0, gSettings.winWidth, gSettings.winHeight));

        GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 6));
    }
    if (gDrawnPoints < gNoPoints)
    {

        //benchmark
        struct _timeb startTime, endTime;
        _ftime(&startTime);
        gProgress = (int)((100.0 * gDrawnPoints / gNoPoints));
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, gTextureFrameBuffer));       
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            printf("Framebuffer not complete at line %i\n", __LINE__);

        GL_CALL(glBindVertexArray(gGenericVertexArray));  

        if (gState == STATE_MANDEL) {
            GL_CALL(glUseProgram(gTexturePID[gPrecision])); 
            GL_CALL(glUniform4d(gBLLocation[gPrecision], left.h, left.l, bottom.h, bottom.l));
            GL_CALL(glUniform2d(gStepLocation[gPrecision], step.h, step.l));
            GL_CALL(glUniform4i(gParamsLocation[gPrecision], gSettings.thresh, gSettings.winWidth, gSettings.winHeight, gRes));
        }
        else if (gState == STATE_JULIA) {
            GL_CALL(glUseProgram(gTextureJuliaPID[gPrecision]));  
            GL_CALL(glUniform2d(gJuliaLocation[gPrecision], gJuliaOffset.x, gJuliaOffset.y));
            GL_CALL(glUniform4d(gBLLocationJulia[gPrecision], left.h, left.l, bottom.h, bottom.l));
            GL_CALL(glUniform2d(gStepLocationJulia[gPrecision], step.h, step.l));
            GL_CALL(glUniform4i(gParamsLocationJulia[gPrecision], gSettings.thresh, gSettings.winWidth, gSettings.winHeight, gRes));
        }
       
        

        GL_CALL(glActiveTexture(GL_TEXTURE0));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, gTexture[1-gTargetTexture]));  

        GL_CALL(glViewport(0,0,gSettings.winWidth,gSettings.winHeight));
        GL_CALL(glEnable(GL_SCISSOR_TEST));

        int yBatchStart = -1;
        int yBatchEnd = -1;
        if (gDrawnBottom < gSettings.winHeight - gDrawnTop)
        {
            if (gDrawnTop == -1)
            {
                yBatchStart = gSettings.winHeight / gRes / 2 * gRes;
            }
            else
            {
                yBatchStart = gDrawnTop;
            }
            yBatchEnd = yBatchStart + gYBatchSize * gRes;
            if (yBatchEnd > gSettings.winHeight) {
                yBatchEnd = gSettings.winHeight;
            }
//            printf("Top batch %d - %d\n", yBatchStart, yBatchEnd);
        }
        else
        {
            if (gDrawnBottom == -1)
            {
                yBatchEnd = gSettings.winHeight / gRes / 2 * gRes;
            }
            else
            {
                yBatchEnd = gDrawnBottom;
            }
            yBatchStart = yBatchEnd - gYBatchSize * gRes;
            if (yBatchStart < 0) {
                yBatchStart = 0;
            }
//            printf("Bottom batch %d - %d\n", yBatchStart, yBatchEnd);
        }

        //set up clip space so nothing is rendered past window, then draw to framebuffer
        GL_CALL(glScissor(0, yBatchStart, gSettings.winWidth, (yBatchEnd - yBatchStart)));
        GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 6));
        GL_CALL(glFinish());
        GL_CALL(glDisable(GL_SCISSOR_TEST));

        if (gDrawnBottom == -1 || yBatchStart < gDrawnBottom)
            gDrawnBottom = yBatchStart;
        if (gDrawnTop == -1 || yBatchEnd > gDrawnTop)
            gDrawnTop = yBatchEnd;
        gDrawnPoints += (yBatchEnd - yBatchStart) * gSettings.winWidth;

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
    GL_CALL(glUniform4i(gScrParamLocation, gSettings.thresh, paintRes, gSettings.winWidth, gSettings.winHeight));
    GL_CALL(glUniform2f(gColMapLocation, gSettings.baseHue, gSettings.hueScale));
    GL_CALL(glUniform1f(gPaintZoomLocation, gPaintZoom));
    
    GL_CALL(glViewport(0, 0, gSettings.winWidth, gSettings.winHeight));

    GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 2*3));
 
}
