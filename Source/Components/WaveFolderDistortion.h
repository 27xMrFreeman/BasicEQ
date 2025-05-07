
#pragma once
#include <JuceHeader.h>
#include "CustomWaveShaper.cpp"

class WaveFolderDistortion
{
public:
    void prepare(juce::dsp::ProcessSpec& spec, size_t oversamplingFactor);

    void process(juce::dsp::AudioBlock<float>& inputBlock);

    void reset();

    CustomWaveShaper<float, std::function<float(float)>> waveFolderTri, waveFolderSin, waveShaper;
    float sampleRate{ 0 }, a{ 8 }, b{ 1 };

    juce::AudioBuffer<float> posBuffer, negBuffer;

    juce::dsp::StateVariableTPTFilter<float> DCFilter;

};