#pragma once

#include <string>
#include "quad.h"

class Settings
{
public:
    Settings(const char* filename);
    int minRes = 128;
    int maxThresh = 1000000;
    int minThresh = 1000;
    int thresh = 1000;
    long long frameCap = 25000 * 1024;
    float durationTarget = 400;
    float durationFilter = 0.3;
    int highCapFactor = 8;
    float quadZoom = 1e-13;

    int moveStep = 1;
    double zoomFraction = 0.05;
    double threshFraction = 0.25;
    double hueFraction = 1.0/12.0;
    double hueStep = 0.5;
    float hueScale = 2.0/3.0;
    float baseHue = 3.0;

    int winWidth = 1024;
    int winHeight = 512;

    Quad centrer = Quad(-0.5);
    Quad centrei = Quad(0.0);
    float zoomExp = 0.0;

    std::string lowShader = "";
    std::string highShader = "";
    std::string lowShaderJulia = "";
    std::string highShaderJulia = "";

    bool load (const char* filename);
    bool save (const char* filename);

    double movZoomStart = 1.0;
    double movZoomEnd = 1e-12;
    int movFrames = 100;
    float movHueScaleStart = 2.0 / 3.0;
    float movHueScaleEnd = 2.0;
    int movThresh = 1000;
};
