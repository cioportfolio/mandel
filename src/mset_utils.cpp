#include "globals.h"
#include "mset.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

bool Mset::init()
{
    bool success = true;
    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK)
    {
        printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError));
    }
    if (!initGL())
    {
        printf("Unable to initialize OpenGL!\n");
        success = false;
    }
    gInitialised = success;
    stbi_flip_vertically_on_write(1);
    return success;
}

void Mset::close()
{
    if (!gInitialised) return;
    //Deallocate program
    glDeleteProgram(gProgramID[0]);
    checkGLError(__LINE__);
    glDeleteProgram(gProgramID[1]);
    checkGLError(__LINE__);
}

void Mset::checkGLError(int l)
{
    GLenum err;
    //	printf("Checking line %i\n", l);
    while ((err = glGetError()))
    {
        printf("GL error at line %i : %i\n", l, err);
    }
}

bool Mset::precision()
{
    gProgram = 1-gProgram;

    gCentreLocation = glGetUniformLocation(gProgramID[gProgram], "centre");
    checkGLError(__LINE__);
    if (gCentreLocation == -1)
    {
        gLog("Failed to get centre location");
        return false;
    }

    gScaleLocation = glGetUniformLocation(gProgramID[gProgram], "scale");
    checkGLError(__LINE__);
    if (gScaleLocation == -1)
    {
        gLog("Failed to get scale locations");
        return false;
    }

    gParamsLocation = glGetUniformLocation(gProgramID[gProgram], "params");
    checkGLError(__LINE__);
    if (gParamsLocation == -1)
    {
        gLog("Failed to get params locations");
        return false;
    }

    gInTexLocation = glGetUniformLocation(gProgramID[gProgram], "inTex");
    checkGLError(__LINE__);
    if (gInTexLocation == -1)
    {
        gLog("Failed to get inTex location");
        return false;
    }
    checkGLError(__LINE__);

/*    if (gProgram)
        gLog("Switched to Quad(128bit) precision\n");
    else
        gLog("Switched to Double(64bit) precision\n");*/

    return true;
}

void Mset::saveFrame(const char *filename) {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    int x = viewport[0];
    int y = viewport[1];
    int width = viewport[2];
    int height = viewport[3];

    char *data = (char*) malloc((size_t) (width * height * 3)); // 3 components (R, G, B)

    if (!data)
        gLog("No screen data to save");

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

    int saved = stbi_write_jpg(filename, width, height, 3, data, 0);

    free(data);
}
