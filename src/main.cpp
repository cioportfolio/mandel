#define _CRT_SECURE_NO_WARNINGS
#define GL_SILENCE_DEPRECATION
#define GLEW_STATIC

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
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

void initImGui()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(gGLWindow, gContext);
	ImGui_ImplOpenGL3_Init("#version 130");

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);
}


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
					initImGui();
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

int gDragStartx, gDragStarty;
bool gDragging = false;

void handleMouse(SDL_Event e)
{
	const Uint8* state = SDL_GetKeyboardState(NULL);
	bool shiftKey = (state[SDL_SCANCODE_LSHIFT] == 1 || state[SDL_SCANCODE_RSHIFT] == 1);
	bool ctrlKey = (state[SDL_SCANCODE_LCTRL] == 1 || state[SDL_SCANCODE_RCTRL] == 1);
	if (gDragging && e.type == SDL_MOUSEMOTION)
	{
		int nx, ny;
		SDL_GetMouseState(&nx, &ny);
		int x = nx - gDragStartx;
		int y = ny - gDragStarty;
		gDragStartx = nx;
		gDragStarty = ny;

		if (ctrlKey)
		{
			gSettings.hueScale -= gSettings.hueStep * y / 100.0;
			gSettings.baseHue += gSettings.hueFraction + x / 100.0;
			render();
		}
		else
		{
			gSettings.centrer = gSettings.centrer.add(gSettings.scaler.mul(Quad((double)-x / gSettings.winWidth * 2.0)));
			gSettings.centrei = gSettings.centrei.add(gSettings.scalei.mul(Quad((double)y / gSettings.winHeight *2.0)));
			gM.restart();
			gM.iterate();
			render();
		}
	}
	if (e.type == SDL_MOUSEWHEEL)
	{
		if (ctrlKey)
		{
			if (e.wheel.y + e.wheel.x > 0)
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
				if (gSettings.thresh == gSettings.minThresh)
					return;
				gSettings.thresh /= 1.0 + gSettings.threshFraction;
				if (gSettings.thresh < gSettings.minThresh)
					gSettings.thresh = gSettings.minThresh;
				gM.restart();
				gM.iterate();
				render();
			}
		}
		else
		{
			int dir = (e.wheel.y + e.wheel.x > 0) ? 1.0 : -1.0;
			int x = 0, y = 0;
			SDL_GetMouseState(&x, &y);
			gSettings.centrer = gSettings.centrer.add(gSettings.scaler.mul(Quad(2.0 * x / gSettings.winWidth - 1.0)));
			gSettings.centrei = gSettings.centrei.add(gSettings.scalei.mul(Quad(1.0 - 2.0 * y / gSettings.winHeight)));
			gSettings.scaler = gSettings.scaler.mul(Quad(1.0 - dir * gSettings.zoomFraction));
			gSettings.scalei = gSettings.scaler.mul(Quad((double)gSettings.winHeight / gSettings.winWidth));
			SDL_WarpMouseInWindow(gGLWindow, gSettings.winWidth / 2, gSettings.winHeight / 2);
			gM.restart();
			gM.iterate();
			render();
		}
	}
	if (e.type == SDL_MOUSEBUTTONUP)
	{
		gDragging = false;
		SDL_MouseButtonEvent m = e.button;
		int x = 0, y = 0;
		SDL_GetMouseState(&x, &y);
		if (m.button == SDL_BUTTON_LEFT && m.clicks == 2)
		{
			gSettings.centrer = gSettings.centrer.add(gSettings.scaler.mul(Quad(2.0 * x / gSettings.winWidth - 1.0)));
			gSettings.centrei = gSettings.centrei.add(gSettings.scalei.mul(Quad(1.0 - 2.0 * y / gSettings.winHeight)));
			gSettings.scaler = gSettings.scaler.mul(Quad(1.0 - gSettings.zoomFraction));
			gSettings.scalei = gSettings.scaler.mul(Quad((double)gSettings.winHeight / gSettings.winWidth));
			SDL_WarpMouseInWindow(gGLWindow, gSettings.winWidth / 2, gSettings.winHeight / 2);
			gM.restart();
			gM.iterate();
			render();
		}
		else if (m.button == SDL_BUTTON_RIGHT && m.clicks == 2)
		{
			gSettings.centrer = gSettings.centrer.add(gSettings.scaler.mul(Quad(2.0 * x / gSettings.winWidth - 1.0)));
			gSettings.centrei = gSettings.centrei.add(gSettings.scalei.mul(Quad(1.0 - 2.0 * y / gSettings.winHeight)));
			gSettings.scaler = gSettings.scaler.mul(Quad(1.0 + gSettings.zoomFraction));
			gSettings.scalei = gSettings.scaler.mul(Quad((double)gSettings.winHeight / gSettings.winWidth));
			SDL_WarpMouseInWindow(gGLWindow, gSettings.winWidth / 2, gSettings.winHeight / 2);
			gM.restart();
			gM.iterate();
			render();
		}
	}
	if (e.type == SDL_MOUSEBUTTONDOWN)
	{
		SDL_GetMouseState(&gDragStartx, &gDragStarty);
		gDragging = true;
	}
}

bool show_demo_window = false;
bool showControls = false;
bool showConstraints = false;

void imGuiFrame()
{


	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		ImGui::Begin("Controls", &showControls);

		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Advanced", &showConstraints);
		ImGui::Text("Window %i x %i", gSettings.winWidth, gSettings.winHeight);
		if (ImGui::SliderInt("Threshold", &gSettings.thresh, gSettings.minThresh, gSettings.maxThresh, "%d", ImGuiSliderFlags_Logarithmic))
		{
			gM.restart();
			gM.iterate();
		}
		double cr = gSettings.centrer.h;
		double ci = gSettings.centrei.h;
		if (ImGui::InputDouble("Centre ", &cr, 0, 0,"%e"))
		{
			gSettings.centrer = Quad(cr);
			gM.restart();
			gM.iterate();
		}
		ImGui::SameLine();
		if (ImGui::InputDouble("+ i", &ci,0,0,"%e"))		{
			gSettings.centrer = Quad(cr);
			gM.restart();
			gM.iterate();
		}
		float z = 1.0/(float)gSettings.scaler.h;
		if (ImGui::SliderFloat("Zoom", &z, 1, 1e30, "%e", ImGuiSliderFlags_Logarithmic))
		{
			gSettings.scaler = Quad(1.0/z);
			gSettings.scalei = gSettings.scaler.mul(Quad((double)gSettings.winHeight / gSettings.winWidth));
			gM.restart();
			gM.iterate();
		}
		ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(gSettings.baseHue, 0.8, 0.8));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::HSV(gSettings.baseHue, 1, 1));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, (ImVec4)ImColor::HSV(gSettings.baseHue, 1, 1));
		ImGui::SliderFloat("Base Hue", &gSettings.baseHue, 0.0, 1.0, "%.3f");
		ImGui::PopStyleColor(3);
		ImGui::SliderFloat("hueScale", &gSettings.hueScale, - 20.0f, 20.0f, "%.2f");
		if (gM.iterating())
		{
			ImGui::Text("%i Bit. Resolution %i %i %% Batch duration %i", (gM.gProgram == 1)?128:64, gM.gRes, gM.gProgress, (int)gM.frameDuration);
		}
		else
		{
			ImGui::Text("Completed fractal");
		}

		ImGui::End();

		if (showConstraints)
		{
			ImGui::Begin("Advanced", &showConstraints);
			ImGui::SliderInt("MinRes", &gSettings.minRes, 1, 500);
			ImGui::DragIntRange2("Threshold Range", &gSettings.minThresh, &gSettings.maxThresh, 1.0, 1, 100000000);
			ImGui::SliderFloat("Batch target time", &gSettings.durationTarget, 1.0, 1000.0);
			ImGui::SliderFloat("Duration filter", &gSettings.durationFilter, 0.0, 1.0);
			ImGui::SliderInt("Duration factor for High precision", &gSettings.highCapFactor, 1, 25);
			ImGui::InputDouble("Move fraction", &gSettings.moveFraction);
			ImGui::InputDouble("Zoom fraction", &gSettings.zoomFraction);
			ImGui::InputDouble("Threshold fraction", &gSettings.threshFraction);
			ImGui::InputDouble("Hue fraction", &gSettings.hueFraction);
			ImGui::InputDouble("Hue Step", &gSettings.hueStep);
			if (ImGui::InputFloat("Quad zoom level", &gSettings.quadZoom, 0, 0, "%e"))
			{
				bool highPrec = (gSettings.scaler.h < gSettings.quadZoom);
				if (highPrec != (gM.gProgram == 1))
				{
					gM.precision();
					gM.restart();
					gM.iterate();
				}
			}
			ImGui::End();
		}

	}
}


void render()
{
	gM.paint();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glFlush();
	SDL_GL_SwapWindow(gGLWindow);
}


void close()
{
	gM.close();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	SDL_GL_DeleteContext(gContext);
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
			}
			imGuiFrame();
			render();

			while (SDL_PollEvent(&e) != 0 && !quit)
			{
				//User requests quit
				ImGui_ImplSDL2_ProcessEvent(&e);
				ImGuiIO& io = ImGui::GetIO();
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
				else if (!io.WantCaptureMouse)
					if (e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEWHEEL || e.type == SDL_MOUSEBUTTONUP || e.type == SDL_MOUSEBUTTONDOWN)
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


