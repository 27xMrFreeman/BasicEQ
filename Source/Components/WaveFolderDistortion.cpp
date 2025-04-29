/*
  ==============================================================================

    WaveFolderDistortion.cpp
    Created: 22 Apr 2025 6:35:25pm
    Author:  knize

  ==============================================================================
*/

#include "WaveFolderDistortion.h"

void WaveFolderDistortion::prepare(juce::dsp::ProcessSpec& spec, size_t oversamplingFactor)
{
    sampleRate = spec.sampleRate;

    posBuffer.setSize(spec.numChannels, spec.maximumBlockSize * oversamplingFactor);
    negBuffer.setSize(spec.numChannels, spec.maximumBlockSize * oversamplingFactor);

    waveFolderTri.functionToUse = [this](float x){
        /*if (x > 0) {
            y = std::abs(x - std::round(x));
        }
        else {
            y = - std::abs(x - std::round(x));
        }*/
        float y = std::abs(x - std::round(x));
        return y;
        };

    waveFolderSin.functionToUse = [this](float x) {
        float y = 0.9 * std::tanh(x) + 0.1 * std::sin(8 * x);
        return y;
        };

    waveShaper.functionToUse = [this](float x) {
        //float y = 0;
        if (x > 0) {
            float y = (2 / (1 + std::exp(-5 * x + 1)) - 1);
            return y;
        }
        else {
            float y = (2 / (1 + std::exp(-5 * x + 3)) - 0.5);
            return y;
        }
        };

    DCFilter.reset();
    DCFilter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    DCFilter.setCutoffFrequency(5);
    DCFilter.prepare(spec);
}

void WaveFolderDistortion::process(juce::dsp::AudioBlock<float>& inputBlock)
{
    //init buffer blocks
    juce::dsp::AudioBlock<float> posBlock(posBuffer), negBlock(negBuffer);
    //ws positive
    waveFolderTri.process(juce::dsp::ProcessContextNonReplacing<float>(inputBlock, posBlock));
    waveFolderSin.process(juce::dsp::ProcessContextReplacing<float>(inputBlock));
    inputBlock += posBlock;
    //inputBlock += negBlock;
    inputBlock.multiplyBy(0.5);
    waveShaper.process(juce::dsp::ProcessContextReplacing<float>(inputBlock));
    DCFilter.process(juce::dsp::ProcessContextReplacing<float>(inputBlock));
}

void WaveFolderDistortion::reset()
{

}
