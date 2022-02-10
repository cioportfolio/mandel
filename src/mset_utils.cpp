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
    for (int i = 0; i < 2; i++)
    {
        GL_CALL(glDeleteProgram(gTexturePID[i]));
    }
    GL_CALL(glDeleteProgram(gScreenPID));
    GL_CALL(glDeleteVertexArrays(1, &gGenericVertexArray));
    GL_CALL(glDeleteBuffers(1, &gVertexBufferObject));
    GL_CALL(glDeleteTextures(2, gTexture));
    GL_CALL(glDeleteFramebuffers(1, &gTextureFrameBuffer));
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

void Mset::saveFrame(const char *filename) {
    GLint viewport[4];
    GL_CALL(glGetIntegerv(GL_VIEWPORT, viewport));

    int x = viewport[0];
    int y = viewport[1];
    int width = viewport[2];
    int height = viewport[3];

    char *data = (char*) malloc((size_t) (width * height * 3)); // 3 components (R, G, B)

    if (!data)
        gLog("No screen data to save");

    GL_CALL(glPixelStorei(GL_PACK_ALIGNMENT, 1));
    GL_CALL(glReadPixels(x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE, data));

    int saved = stbi_write_jpg(filename, width, height, 3, data, 0);

    free(data);
}
