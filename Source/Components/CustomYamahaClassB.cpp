/*
  ==============================================================================

    CustomYamahaClassB.cpp
    Created: 15 Apr 2025 9:20:04pm
    Author:  knize

  ==============================================================================
*/

#include "CustomYamahaClassB.h"

void CustomYamahaClassB::prepare(juce::dsp::ProcessSpec& spec, size_t oversamplingFactor)
{
    sampleRate = spec.sampleRate;

    posBuffer.setSize(spec.numChannels, spec.maximumBlockSize * oversamplingFactor);
    negBuffer.setSize(spec.numChannels, spec.maximumBlockSize * oversamplingFactor);

    waveShaperPos.functionToUse = [this](float x) {
        float y = ( 2 / ( 1 + std::exp( - this->a * x + this->b )) - 1 );
        //float y;
        //x > 0 ? y = 1 : y = -1;
        return y;
        };
    waveShaperNeg.functionToUse = [this](float x) {
        float y = (2 / (1 + std::exp(-this->a * x + this->b)) - 1);
        //float y;
        //x > 0 ? y = 1 : y = -1;
        return y;
        };

}

void CustomYamahaClassB::process(juce::dsp::AudioBlock<float>& inputBlock)
{
    //init buffer blocks
    juce::dsp::AudioBlock<float> posBlock(posBuffer), negBlock(negBuffer);
    //ws positive
    waveShaperPos.process(juce::dsp::ProcessContextNonReplacing<float>(inputBlock, posBlock));
    //invert block
    inputBlock.negate();
    waveShaperNeg.process(juce::dsp::ProcessContextReplacing<float>(inputBlock));
    inputBlock.negate();
    inputBlock += posBlock;
}

void CustomYamahaClassB::reset()
{
}
