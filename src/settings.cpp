#define _CRT_SECURE_NO_WARNINGS

#include "settings.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <iostream>
#include <fstream>

Settings::Settings(const char* filename)
{
    this->load(filename);
}

bool Settings::load (const char* filename)
{

    FILE* fp = fopen(filename, "rb"); // non-Windows use "r"
    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    rapidjson::Document settingsDoc;
    settingsDoc.ParseStream(is);
    fclose(fp);

    rapidjson::Value::ConstMemberIterator itr = settingsDoc.FindMember("minres");
    if (itr != settingsDoc.MemberEnd())
        minRes = itr->value.GetInt();
    itr = settingsDoc.FindMember("highcapfactor");
    if (itr != settingsDoc.MemberEnd())
        highCapFactor = itr->value.GetInt();
    itr = settingsDoc.FindMember("maxthresh");
    if (itr != settingsDoc.MemberEnd())
        maxThresh = itr->value.GetInt();
    itr = settingsDoc.FindMember("minthresh");
    if (itr != settingsDoc.MemberEnd())
        minThresh = itr->value.GetInt();
    itr = settingsDoc.FindMember("thresh");
    if (itr != settingsDoc.MemberEnd())
        thresh = itr->value.GetInt();
    itr = settingsDoc.FindMember("framecap");
    if (itr != settingsDoc.MemberEnd())
        frameCap = itr->value.GetUint64();
    itr = settingsDoc.FindMember("movefraction");
    if (itr != settingsDoc.MemberEnd())
        moveFraction = itr->value.GetDouble();
    itr = settingsDoc.FindMember("zoomfraction");
    if (itr != settingsDoc.MemberEnd())
        zoomFraction = itr->value.GetDouble();
    itr = settingsDoc.FindMember("threshfraction");
    if (itr != settingsDoc.MemberEnd())
        threshFraction = itr->value.GetDouble();
    itr = settingsDoc.FindMember("huefraction");
    if (itr != settingsDoc.MemberEnd())
        hueFraction = itr->value.GetDouble();
    itr = settingsDoc.FindMember("huestep");
    if (itr != settingsDoc.MemberEnd())
        hueStep = itr->value.GetDouble();
    itr = settingsDoc.FindMember("winwidth");
    if (itr != settingsDoc.MemberEnd())
    {
        winWidth = itr->value.GetInt();
        scalei = scaler.mul(Quad((double)winHeight/ winWidth));
    }
    itr = settingsDoc.FindMember("winheight");
    if (itr != settingsDoc.MemberEnd())
    {
        winHeight = itr->value.GetInt();
        scalei = scaler.mul(Quad((double)winHeight/ winWidth));
    }
    itr = settingsDoc.FindMember("lowshader");
    if (itr != settingsDoc.MemberEnd())
        lowShader = itr->value.GetString();
    itr = settingsDoc.FindMember("highshader");
    if (itr != settingsDoc.MemberEnd())
        highShader = itr->value.GetString();
    itr = settingsDoc.FindMember("r");
    if (itr != settingsDoc.MemberEnd())
    {
        centrer = Quad(itr->value.GetDouble());
    }
    itr = settingsDoc.FindMember("i");
    if (itr != settingsDoc.MemberEnd())
    {
        centrei = Quad(itr->value.GetDouble());
    }
    itr = settingsDoc.FindMember("scale");
    if (itr != settingsDoc.MemberEnd())
    {
        scaler = Quad(itr->value.GetDouble());
        scalei = scaler.mul(Quad((double)winHeight/ winWidth, 0.));

    }
    itr = settingsDoc.FindMember("basehue");
    if (itr != settingsDoc.MemberEnd())
        baseHue = itr->value.GetDouble();
    itr = settingsDoc.FindMember("huescale");
    if (itr != settingsDoc.MemberEnd())
        hueScale = itr->value.GetDouble();

    return true;
}

bool Settings::save (const char* filename)
{

    std::ofstream fs(filename);

    if(!fs.is_open())
        return false;

    fs<<"{"<<std::endl;
    fs<<"\"minres\": "<<minRes<<","<<std::endl;
    fs<<"\"highcapfactor\": "<<highCapFactor<<","<<std::endl;
    fs<<"\"maxthresh\": "<<maxThresh<<","<<std::endl;
    fs<<"\"minthresh\": "<<minThresh<<","<<std::endl;
    fs<<"\"thresh\": "<<thresh<<","<<std::endl;
    fs<<"\"framecap\": "<<frameCap<<","<<std::endl;
    fs<<"\"movefraction\": "<<moveFraction<<","<<std::endl;
    fs<<"\"zoomfraction\": "<<zoomFraction<<","<<std::endl;
    fs<<"\"threshfraction\": "<<threshFraction<<","<<std::endl;
    fs<<"\"huefraction\": "<<hueFraction<<","<<std::endl;
    fs<<"\"huestep\": "<<hueStep<<","<<std::endl;
    fs<<"\"winwidth\": "<<winWidth<<","<<std::endl;
    fs<<"\"winheight\": "<<winHeight<<","<<std::endl;
    fs<<"\"lowshader\": \""<<lowShader<<"\","<<std::endl;
    fs<<"\"highshader\": \""<<highShader<<"\","<<std::endl;
    fs<<"\"r\": "<<centrer.h<<","<<std::endl;
    fs<<"\"i\": "<<centrei.h<<","<<std::endl;
    fs<<"\"scale\": "<<scaler.h<<","<<std::endl;
    fs<<"\"huescale\": "<<hueScale<<","<<std::endl;
    fs<<"\"basehue\": "<<baseHue<<std::endl;
    fs<<"}"<<std::endl;
    fs.close();
    return true;
}





