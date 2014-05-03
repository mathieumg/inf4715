#pragma once

#include <string>

struct StartAnimation
{
    std::string Name;
    std::string Fallback;
    float TransitionTime;
    float RestartTime;

    StartAnimation()
        : TransitionTime(0), RestartTime(0)
    {}
};

struct LoopAnimation
{
    std::string Name;
    float TransitionTime;
    float RestartTime;

    LoopAnimation()
        : TransitionTime(0), RestartTime(0)
    {}
};

struct KeyFrameHit
{
    std::string Name;

    KeyFrameHit(const std::string& name)
        : Name(name)
    {}
};