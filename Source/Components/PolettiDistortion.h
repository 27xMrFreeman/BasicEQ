/*
  ==============================================================================

    PolettiDistortion.h
    Created: 15 Apr 2025 6:00:14pm
    Author:  knize

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "CustomWaveShaper.cpp"

enum PolettiType
{
    AsymPos,
    AsymNeg,
    Sym
};

enum processChainEnum
{
    posAsymWaveShaper,
    posLRFilter,
    posTPTFilter,
    posLadderFilter,
    posBTRFilter,
    posSVFilter,
    posSymWaveShaper/*,
    SecondDCFilter*/
};
class PolettiDistortion
{
public:
    void prepare(juce::dsp::ProcessSpec& spec, size_t oversamplingFactor);

    void process(juce::dsp::AudioBlock<float>& inputBlock);

    void reset();

    float sampleRate{ 0 };

    processChainEnum filterType = processChainEnum::posTPTFilter;

    bool asymBypassed{ false }, filterBypassed{ false }, symBypassed{ false }, osBypassed{ false };

    juce::AudioBuffer<float> posBuffer, negBuffer;

    //juce::dsp::LinkwitzRileyFilter<float> LDCfilter, RDCfilter; // even more phase shift
    //juce::dsp::FirstOrderTPTFilter<float> TPTfilter; // smaller phase shift but too gentle
    //juce::dsp::LadderFilter<float> LDRfilter; // same phase shift as BTRFilter
    //juce::dsp::IIR::Filter<float> LeftPosBTRFilter, LeftNegBTRFilter; // quite a big phase shift
    //juce::dsp::StateVariableTPTFilter<float> SVfilter; // same phase shift as TPTfilter and StateVariableFilter

    using WaveShaper = CustomWaveShaper<float, std::function<float(float)>>;
    using Lr = juce::dsp::LinkwitzRileyFilter<float>;
    using Tpt = juce::dsp::FirstOrderTPTFilter<float>;
    using Ldr = juce::dsp::LadderFilter<float>;
    using Btr = juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>;
    using Sv = juce::dsp::StateVariableTPTFilter<float>;
    juce::dsp::ProcessorChain<WaveShaper, Lr, Tpt, Ldr, Btr, Sv, WaveShaper> posProcessorChain, negProcessorChain;
};