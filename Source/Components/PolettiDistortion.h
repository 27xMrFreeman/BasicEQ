
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
    posSymWaveShaper
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

    using WaveShaper = CustomWaveShaper<float, std::function<float(float)>>;
    using Lr = juce::dsp::LinkwitzRileyFilter<float>;
    using Tpt = juce::dsp::FirstOrderTPTFilter<float>;
    using Ldr = juce::dsp::LadderFilter<float>;
    using Btr = juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>;
    using Sv = juce::dsp::StateVariableTPTFilter<float>;
    juce::dsp::ProcessorChain<WaveShaper, Lr, Tpt, Ldr, Btr, Sv, WaveShaper> posProcessorChain, negProcessorChain;
};