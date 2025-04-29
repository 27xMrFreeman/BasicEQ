/*
  ==============================================================================

    CustomYamahaClassB.h
    Created: 15 Apr 2025 9:20:04pm
    Author:  knize

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "CustomWaveShaper.cpp"

class CustomYamahaClassB
{
public:
    void prepare(juce::dsp::ProcessSpec& spec, size_t oversamplingFactor);

    void process(juce::dsp::AudioBlock<float>& inputBlock);

    void reset();

    CustomWaveShaper<float, std::function<float(float)>> waveShaperPos, waveShaperNeg;
    float sampleRate{ 0 }, a{ 8 }, b{ 1 };

    juce::AudioBuffer<float> posBuffer, negBuffer;

};