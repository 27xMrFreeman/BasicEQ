/*
  ==============================================================================

    ToneStack.h
    Created: 12 Apr 2025 10:02:55pm
    Author:  knize

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class ToneStack
{
public:
    ToneStack();
    ~ToneStack();

    void prepare(juce::dsp::ProcessSpec& spec);
    void reset();

    void process(juce::dsp::AudioBlock<float>& inputBlock);
    
    void setLowGain(float newGainDB);
    void setMidGain(float newGainDB);
    void setHighGain(float newGainDB);

private:
    float sampleRate{ 0 };

    juce::AudioBuffer<float> bufferBP, bufferHP;
    
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> tsBandPass, tsHighPass, tsLowShelf, tsMidPeak, tsHighShelf;

    juce::dsp::Gain<float> tsBandPassGain, tsHighPassGain;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ToneStack)
};
