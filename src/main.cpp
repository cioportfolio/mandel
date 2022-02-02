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

void render();

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
		render();

		printf("Canvas Size %d / %d", w, h);

		return true;
	}
	return false;
}

void handleKeys(SDL_Keysym key)
{
	bool shiftKey = key.mod & KMOD_SHIFT;
	bool ctrlKey = key.mod & KMOD_CTRL;

	switch (key.sym) {
	case SDLK_LEFT:
		if (ctrlKey)
		{
			gSettings.baseHue -= gSettings.hueFraction;
			render();
		}
		else
		{
			gSettings.centrer = gSettings.centrer.add(gSettings.scaler.mul(Quad(gSettings.moveFraction * (shiftKey ? 10 : 1))));
			gM.restart();
			gM.iterate();
			render();
		}
		break;
	case SDLK_RIGHT:
		if (ctrlKey)
		{
			gSettings.baseHue += gSettings.hueFraction;
			render();
		}
		else
		{
			gSettings.centrer = gSettings.centrer.add(gSettings.scaler.mul(Quad(-gSettings.moveFraction * (shiftKey ? 10 : 1))));
			gM.restart();
			gM.iterate();
			render();
		}
		break;
	case SDLK_UP:
		if (ctrlKey)
		{
			gSettings.hueScale += gSettings.hueStep;
			render();
		}
		else
		{
			gSettings.centrei = gSettings.centrei.add(gSettings.scalei.mul(Quad(-gSettings.moveFraction * (shiftKey ? 10 : 1))));
			gM.restart();
			gM.iterate();
			render();
		}
		break;
	case SDLK_DOWN:
		if (ctrlKey)
		{
			gSettings.hueScale -= gSettings.hueStep;
			render();
		}
		else
		{
			gSettings.centrei = gSettings.centrei.add(gSettings.scalei.mul(Quad(gSettings.moveFraction * (shiftKey ? 10 : 1))));
			gM.restart();
			gM.iterate();
			render();
		}
		break;
	case SDLK_PAGEUP:
		if (ctrlKey)
		{
			if (gSettings.thresh == gSettings.minThresh)
				return;
			gSettings.thresh /= 1.0 + gSettings.threshFraction;
			if (gSettings.thresh < gSettings.minThresh)
				gSettings.thresh = gSettings.minThresh;
			gM.restart();
			gM.iterate();
			render();
		}
		else
		{
			gSettings.scaler = gSettings.scaler.mul(Quad(1.0 + gSettings.moveFraction * (shiftKey ? 10 : 1)));
			gSettings.scalei = gSettings.scaler.mul(Quad((double)gSettings.winHeight / gSettings.winWidth));
			gM.restart();
			gM.iterate();
			render();
		}
		break;
	case SDLK_PAGEDOWN:
		if (ctrlKey)
		{
			if (gSettings.thresh == gSettings.maxThresh)
				return;
			gSettings.thresh *= 1.0 + gSettings.threshFraction;
			if (gSettings.thresh > gSettings.maxThresh)
				gSettings.thresh = gSettings.maxThresh;
			gM.restart();
			gM.iterate();
			render();
		}
		else
		{
			gSettings.scaler = gSettings.scaler.mul(Quad(1.0 - gSettings.moveFraction * (shiftKey ? 10 : 1)));
			gSettings.scalei = gSettings.scaler.mul(Quad((double)gSettings.winHeight / gSettings.winWidth));
			gM.restart();
			gM.iterate();
			render();
		}
		break;
//	case SDLK_ESCAPE:
//		gAlert(wxT("Press OK to continue or CANCEL to exit"));
//		break;
	case SDLK_TAB:
		gM.precision();
		gM.restart();
		gM.iterate();
		render();
	}
}

void handleMouse(SDL_Event e)
{
	/*const Uint8* state = SDL_GetKeyboardState(NULL);
	bool shiftKey = (state[SDL_SCANCODE_LSHIFT] == 1 || state[SDL_SCANCODE_RSHIFT] == 1);
	int direction = 1;
	
	if (e.type == SDL_MOUSEBUTTONUP)
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
		
	} */
}

void render()
{
	gM.paint();
	glFlush();
	SDL_GL_SwapWindow(gGLWindow);
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
		bool quit = false;

		//Event handler
		SDL_Event e;

		//While application is running
		while (!quit)
		{
			if (gM.iterating())
			{
				gM.iterate();
				render();
			}

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
					else
					{
						handleKeys(e.key.keysym);
					}
				}
				else if (e.type == SDL_WINDOWEVENT)
				{
					handleWindow(e.window);
				}
				else if (e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEWHEEL || e.type == SDL_MOUSEBUTTONUP || e.type == SDL_MOUSEBUTTONDOWN)
				{
					handleMouse(e);
				}
			}
		}

		//Free resources and close SDL
		close();

		return 0;
	}
}


