#define _CRT_SECURE_NO_WARNINGS
#define GL_SILENCE_DEPRECATION
#define GLEW_STATIC

#include <SDL.h>

#include <GL\glew.h>
#include <GL\gl.h>
#include <GL\glu.h>

#include <SDL_opengl.h>
#include <cstdio>

#include "globals.h"

#define POINT_SCALE 1

bool handleWindow(SDL_WindowEvent w);

bool buildWinData();

//Input handler
bool handleKeys(unsigned char key, int x, int y);

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window *gGLWindow = nullptr;

//OpenGL context
SDL_GLContext gContext;


bool init()
{
	//Initialization flag
	bool success = true;
	
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Use OpenGL 3.1 core
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
		//Create window
		gGLWindow = SDL_CreateWindow("Mandel", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, gSettings.winWidth, gSettings.winHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
		if (gGLWindow == nullptr)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create context
			gContext = SDL_GL_CreateContext(gGLWindow);
			if (gContext == nullptr)
			{
				printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize OpenGL
				if (!gM.init())
				{
					printf("Unable to initialize OpenGL!\n");
					success = false;
				}
				else
				{
					//Use Vsync
					if (SDL_GL_SetSwapInterval(1) < 0)
					{
						printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
					}
					gM.restart();
					gM.iterate();
				}

			}
		}
	}

	return success;
}

bool handleWindow(SDL_WindowEvent we)
{
	if (we.event == SDL_WINDOWEVENT_SIZE_CHANGED)
	{
		int w = we.data1;
		int h = we.data2;

		if (gSettings.winWidth == w && gSettings.winHeight == h)
			return false;
		gSettings.winWidth = w;
		gSettings.winHeight = h;
		gM.resize();
		gM.restart();
		gM.iterate();

		printf("Canvas Size %d / %d", w, h);

		return true;
	}
	return false;
}

bool handleKeys(SDL_Keysym key, int x, int y)
{
/*	if (key.mod & KMOD_SHIFT)
	{
		if (key.sym == SDLK_f)
		{
			gView = gSMoFo * gView;
		}
		if (key.sym == SDLK_b)
		{
			gView = gSMoBa * gView;
		}
		if (key.sym == SDLK_UP)
		{
			gView = gSRoUp * gView;
			return true;
		}
		if (key.sym == SDLK_DOWN)
		{
			gView = gSRoDo * gView;
			return true;
		}
		if (key.sym == SDLK_LEFT)
		{
			vec4 a4 = gView*vec4(0.0,0.0,1.0,0.0);
			vec3 a3 = vec3(a4.x,a4.y,a4.z);
			gSRoLe = rotate(mat4(1.0f),float(-gRoSt/180.0*M_PI*gShiftScale), a3);
			gView = gSRoLe * gView;
			return true;
		}
		if (key.sym == SDLK_RIGHT)
		{
			vec4 a4 = gView*vec4(0.0,0.0,1.0,0.0);
			vec3 a3 = vec3(a4.x,a4.y,a4.z);
			gSRoRi = rotate(mat4(1.0f),float(gRoSt/180.0*M_PI*gShiftScale), a3);
			gView = gSRoRi * gView;
			return true;
		}

		return false;
	}
	else
	{
		if (key.sym == SDLK_p) gFly = !gFly;

		if (key.sym == SDLK_f)
		{
			gView = gMoFo * gView;
		}
		if (key.sym == SDLK_b)
		{
			gView = gMoBa * gView;
		}
		if (key.sym == SDLK_UP)
		{
			gView = gRoUp * gView;
			return true;
		}
		if (key.sym == SDLK_DOWN)
		{
			gView = gRoDo * gView;
			return true;
		}
		if (key.sym == SDLK_LEFT)
		{
			vec4 a4 = gView*vec4(0.0,0.0,1.0,0.0);
			vec3 a3 = vec3(a4.x,a4.y,a4.z);
			gRoLe = rotate(mat4(1.0f),float(-gRoSt/180.0*M_PI), a3);
			gView = gRoLe * gView;
			return true;
		}
		if (key.sym == SDLK_RIGHT)
		{
			vec4 a4 = gView*vec4(0.0,0.0,1.0,0.0);
			vec3 a3 = vec3(a4.x,a4.y,a4.z);
			gRoRi = rotate(mat4(1.0f),float(gRoSt/180.0*M_PI), a3);
			gView = gRoRi * gView;
			return true;
		}
		if (key.sym == SDLK_n)
		{
			update();
			return false;
		}
		if (key.sym == SDLK_u)
		{
			gUpdate = !gUpdate;
			return true;
		}
	} */
	return false;
}

bool handleMouse(SDL_Event e)
{
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	bool shiftKey = (state[SDL_SCANCODE_LSHIFT] == 1 || state[SDL_SCANCODE_RSHIFT] == 1);
	int direction = 1;
	
	/*if (e.type == SDL_MOUSEBUTTONUP)
	{
		SDL_MouseButtonEvent m = e.button;
		if (m.button == SDL_BUTTON_LEFT && m.clicks == 2)
		{
			return true;
		}
		if (m.button == SDL_BUTTON_RIGHT && m.clicks == 2)
		{
			return true;
		}
	}
	if (e.type == SDL_MOUSEWHEEL)
	{

		if (shiftKey)
		{
			if ((e.wheel.y+e.wheel.x) * direction > 0)
			{
				return true;
			}
			if ((e.wheel.y+e.wheel.x) *direction < 0)
			{
				return true;
			}
			return false;
		}
		int x = 0, y = 0;
		SDL_GetMouseState(&x, &y);
		if (e.wheel.y*direction != 0)
		{
			gView = translate(mat4(1.0f), vec3(0.0,0.0,gMoSt*float(e.wheel.y * direction))) * gView;
		}
	}
	if (e.type == SDL_MOUSEMOTION)
	{
		if (gFly) {
			if (e.motion.yrel != 0)
			{
				gView = rotate(mat4(1.0f),float(gRoSt/180.0*M_PI*e.motion.yrel), vec3(1.0,0.0,0.0)) * gView;
			}
			if (e.motion.xrel < 0)
			{
				vec4 a4 = gView*vec4(0.0,0.0,1.0,0.0);
				vec3 a3 = vec3(a4.x,a4.y,a4.z);
				gRoLe = rotate(mat4(1.0f),float(-gRoSt/180.0*M_PI*gFlyScale), a3);
				gView = gRoLe * gView;
			}
			if (e.motion.xrel > 0)
			{
				vec4 a4 = gView*vec4(0.0,0.0,1.0,0.0);
				vec3 a3 = vec3(a4.x,a4.y,a4.z);
				gRoRi = rotate(mat4(1.0f),float(gRoSt/180.0*M_PI*gFlyScale), a3);
				gView = gRoRi * gView;
			}
			return true;
		}
		if (shiftKey)
		{
			return false;
		}
		if ((e.motion.state & SDL_BUTTON_LMASK) > 0)
		{
			if (e.motion.yrel != 0)
			{
				gView = rotate(mat4(1.0f),float(-gRoSt/180.0*M_PI*e.motion.yrel), vec3(1.0,0.0,0.0)) * gView;
			}
			if (e.motion.xrel != 0)
			{
				vec4 a4 = gView*vec4(0.0,0.0,1.0,0.0);
				vec3 a3 = vec3(a4.x,a4.y,a4.z);
				gRoLe = rotate(mat4(1.0f),float(-gRoSt/180.0*M_PI*e.motion.xrel), a3);
				gView = gRoLe * gView;
			}
			return true;
		}
		
	}*/

	return false;
}


void close()
{
	gM.close();
	//Destroy window
	SDL_DestroyWindow(gGLWindow);
	gGLWindow = nullptr;
	//Quit SDL subsystems
	SDL_Quit();
}

int main(int argc, char *args[])
{
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
		return 1;
	}
	else
	{
		//Main loop flag
		bool quit = false, redraw, haltLoop;

		//Event handler
		SDL_Event e;

		//While application is running
		while (!quit)
		{
			if (gM.iterating())
			{
				gM.iterate();
				gM.paint();
				glFlush();
				SDL_GL_SwapWindow(gGLWindow);
			}

			redraw = false;
			while (SDL_PollEvent(&e) != 0 && !quit)
			{
				//User requests quit
				if (e.type == SDL_QUIT)
				{
					quit = true;
				}
				else if (e.type == SDL_KEYDOWN)
				{
					if (e.key.keysym.sym == SDLK_q)
					{
						quit = true;
					}
/*				}
				else if (e.type == SDL_KEYUP)
				{*/
					int x = 0, y = 0;
					SDL_GetMouseState(&x, &y);
					redraw = redraw || handleKeys(e.key.keysym, x, y);
				}
				else if (e.type == SDL_WINDOWEVENT)
				{
					redraw = redraw || handleWindow(e.window);
				}
				else if (e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEWHEEL || e.type == SDL_MOUSEBUTTONUP || e.type == SDL_MOUSEBUTTONDOWN)
					redraw = redraw || handleMouse(e);
			}
			if (redraw) haltLoop = true;
		}

		//Free resources and close SDL
		close();

		return 0;
	}
}


