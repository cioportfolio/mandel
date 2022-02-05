#ifndef SETTINGS_H_INCLUDED
#define SETTINGS_H_INCLUDED

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

    double moveFraction = 1.0/64.0;
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
    Quad scaler = Quad(1.0);
    Quad scalei = Quad(1.0);

    std::string lowShader = "";
    std::string highShader = "";

    bool load (const char* filename);
    bool save (const char* filename);

//    double movStartZoom = 1.0;
    float movZoomFact = 1.41;
    int movFrames = 100;
    float movHueScaleFact = 1.01;
};
#endif // SETTINGS_H_INCLUDED
