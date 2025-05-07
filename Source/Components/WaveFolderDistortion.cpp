
#include "WaveFolderDistortion.h"

void WaveFolderDistortion::prepare(juce::dsp::ProcessSpec& spec, size_t oversamplingFactor)
{
    sampleRate = spec.sampleRate;
    // preallocating buffers
    posBuffer.setSize(spec.numChannels, spec.maximumBlockSize * oversamplingFactor);
    negBuffer.setSize(spec.numChannels, spec.maximumBlockSize * oversamplingFactor);

    // even triangle function
    waveFolderTri.functionToUse = [this](float x){
        float y = std::abs(x - std::round(x));
        return y;
        };
    // odd function
    waveFolderSin.functionToUse = [this](float x) {
        float y = 0.9 * std::tanh(x) + 0.1 * std::sin(8 * x);
        return y;
        };

    waveShaper.functionToUse = [this](float x) {
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
    // processing in parallel
    waveFolderTri.process(juce::dsp::ProcessContextNonReplacing<float>(inputBlock, posBlock));
    waveFolderSin.process(juce::dsp::ProcessContextReplacing<float>(inputBlock));
    // join blocks
    inputBlock += posBlock;
    inputBlock.multiplyBy(0.5);

    waveShaper.process(juce::dsp::ProcessContextReplacing<float>(inputBlock));
    DCFilter.process(juce::dsp::ProcessContextReplacing<float>(inputBlock));
}

void WaveFolderDistortion::reset()
{
    DCFilter.reset();
}
