/*
  ==============================================================================

    HorizontalMeter.h
    Created: 25 May 2024 5:11:02pm
    Author:  knize

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct HorizontalMeterLeft : public juce::Component
{
public:
    void paint(juce::Graphics& g) override;
    void setLevel(const float value) { level = value; }

private:
    float level = -60.f;
};

struct HorizontalMeterRight : public juce::Component
{
public:
    void paint(juce::Graphics& g) override;
    void setLevel(const float value) { level = value; }

private:
    float level = -60.f;
};