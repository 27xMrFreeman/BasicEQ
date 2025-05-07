
#include "PolettiDistortion.h"

void PolettiDistortion::prepare(juce::dsp::ProcessSpec& spec, size_t oversamplingFactor)
{
    sampleRate = spec.sampleRate;

    posProcessorChain.reset();
    posProcessorChain.get<posAsymWaveShaper>().functionToUse = [this](float x) {
        float y = 0;
        if (x <= 0)
        {
            y = (this->posProcessorChain.get<posAsymWaveShaper>().asymPosGain * x) / (1.0 - ((this->posProcessorChain.get<posAsymWaveShaper>().asymPosGain * x) / this->posProcessorChain.get<posAsymWaveShaper>().asymPosLN));
        }
        else
        {
            y = (this->posProcessorChain.get<posAsymWaveShaper>().asymPosGain * x) / (1.0 + ((this->posProcessorChain.get<posAsymWaveShaper>().asymPosGain * x) / this->posProcessorChain.get<posAsymWaveShaper>().asymPosLP));
        }
        return y;
        };
    posProcessorChain.get<posLRFilter>().setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    posProcessorChain.get<posLRFilter>().setCutoffFrequency(5);
    posProcessorChain.get<posTPTFilter>().setType(Tpt::Type::highpass);
    posProcessorChain.get<posTPTFilter>().setCutoffFrequency(5);
    posProcessorChain.get<posLadderFilter>().setMode(juce::dsp::LadderFilterMode::HPF12);
    posProcessorChain.get<posLadderFilter>().setCutoffFrequencyHz(5);
    posProcessorChain.get<posBTRFilter>().state = juce::dsp::IIR::Coefficients<float>::makeHighPass(spec.sampleRate * oversamplingFactor, 5);
    posProcessorChain.get<posSVFilter>().setType(juce::dsp::StateVariableTPTFilterType::highpass);
    posProcessorChain.get<posSVFilter>().setCutoffFrequency(5);

    posProcessorChain.get<posSymWaveShaper>().functionToUse = [this](float x) {
        float y = 0;
        if (x <= 0)
        {
            y = (this->posProcessorChain.get<posSymWaveShaper>().symGain * x) / (1.0 - ((this->posProcessorChain.get<posSymWaveShaper>().symGain * x) / this->posProcessorChain.get<posSymWaveShaper>().symLPLN));
        }
        else
        {
            y = (this->posProcessorChain.get<posSymWaveShaper>().symGain * x) / (1.0 + ((this->posProcessorChain.get<posSymWaveShaper>().symGain * x) / this->posProcessorChain.get<posSymWaveShaper>().symLPLN));
        }
        return y;
        };
    posProcessorChain.prepare(spec);

    negProcessorChain.reset();
    negProcessorChain.get<posAsymWaveShaper>().functionToUse = [this](float x) {
        float y = 0;
        if (x <= 0)
        {
            y = (this->negProcessorChain.get<posAsymWaveShaper>().asymNegGain * x) / (1.0 - ((this->negProcessorChain.get<posAsymWaveShaper>().asymNegGain * x) / this->negProcessorChain.get<posAsymWaveShaper>().asymNegLN));
        }
        else
        {
            y = (this->negProcessorChain.get<posAsymWaveShaper>().asymNegGain * x) / (1.0 + ((this->negProcessorChain.get<posAsymWaveShaper>().asymNegGain * x) / this->negProcessorChain.get<posAsymWaveShaper>().asymNegLP));
        }
        return y;
        };
    negProcessorChain.get<posLRFilter>().setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    negProcessorChain.get<posLRFilter>().setCutoffFrequency(5);
    negProcessorChain.get<posTPTFilter>().setType(Tpt::Type::highpass);
    negProcessorChain.get<posTPTFilter>().setCutoffFrequency(5);
    negProcessorChain.get<posLadderFilter>().setMode(juce::dsp::LadderFilterMode::HPF12);
    negProcessorChain.get<posLadderFilter>().setCutoffFrequencyHz(5);
    negProcessorChain.get<posBTRFilter>().state = juce::dsp::IIR::Coefficients<float>::makeHighPass(spec.sampleRate * oversamplingFactor, 5);
    negProcessorChain.get<posSVFilter>().setType(juce::dsp::StateVariableTPTFilterType::highpass);
    negProcessorChain.get<posSVFilter>().setCutoffFrequency(5);

    negProcessorChain.get<posSymWaveShaper>().functionToUse = [this](float x) {
        float y = 0;
        if (x <= 0)
        {
            y = (this->negProcessorChain.get<posSymWaveShaper>().symGain * x) / (1.0 - ((this->negProcessorChain.get<posSymWaveShaper>().symGain * x) / this->negProcessorChain.get<posSymWaveShaper>().symLPLN));
        }
        else
        {
            y = (this->negProcessorChain.get<posSymWaveShaper>().symGain * x) / (1.0 + ((this->negProcessorChain.get<posSymWaveShaper>().symGain * x) / this->negProcessorChain.get<posSymWaveShaper>().symLPLN));
        }
        return y;
        };
    negProcessorChain.prepare(spec);


    posBuffer.setSize(spec.numChannels, spec.maximumBlockSize * oversamplingFactor);
    negBuffer.setSize(spec.numChannels, spec.maximumBlockSize * oversamplingFactor);
}

void PolettiDistortion::process(juce::dsp::AudioBlock<float>& inputBlock)
{   
    juce::dsp::AudioBlock<float> posBufferBlock(posBuffer), negBufferBlock(negBuffer);
    posBufferBlock = inputBlock;
    negBufferBlock = inputBlock;

    //==========================================================================================================================
    // asymmetric waveshape, parallel processing
    if (!asymBypassed) {
        posProcessorChain.get<posAsymWaveShaper>().process(juce::dsp::ProcessContextReplacing(posBufferBlock));
        float asymGain = posProcessorChain.get<posAsymWaveShaper>().asymPosGain;
        posBufferBlock *= (2.0 / asymGain);

        negProcessorChain.get<posAsymWaveShaper>().process(juce::dsp::ProcessContextReplacing(negBufferBlock));
        negBufferBlock *= (2.0 / asymGain);
    }
    //==========================================================================================================================

    //==========================================================================================================================
    // DC filtering
    if (!filterBypassed) {
        switch (filterType) {
        case posLRFilter:
            posProcessorChain.get<1>().process(juce::dsp::ProcessContextReplacing(posBufferBlock));
            negProcessorChain.get<1>().process(juce::dsp::ProcessContextReplacing(negBufferBlock));
            break;
        case posTPTFilter:
            posProcessorChain.get<2>().process(juce::dsp::ProcessContextReplacing(posBufferBlock));
            negProcessorChain.get<2>().process(juce::dsp::ProcessContextReplacing(negBufferBlock));
            break;
        case posLadderFilter:
            posProcessorChain.get<3>().process(juce::dsp::ProcessContextReplacing(posBufferBlock));
            negProcessorChain.get<3>().process(juce::dsp::ProcessContextReplacing(negBufferBlock));
            break;
        case posBTRFilter:
            posProcessorChain.get<4>().process(juce::dsp::ProcessContextReplacing(posBufferBlock));
            negProcessorChain.get<4>().process(juce::dsp::ProcessContextReplacing(negBufferBlock));
            break;
        case posSVFilter:
            posProcessorChain.get<5>().process(juce::dsp::ProcessContextReplacing(posBufferBlock));
            negProcessorChain.get<5>().process(juce::dsp::ProcessContextReplacing(negBufferBlock));
            break;
        }
    }
    //==========================================================================================================================

    //==========================================================================================================================
    // symmetric waveshaping
    if (!symBypassed) {
        posProcessorChain.get<posSymWaveShaper>().process(juce::dsp::ProcessContextReplacing(posBufferBlock));
        float symGain = posProcessorChain.get<posSymWaveShaper>().symGain;
        posBufferBlock *= (2.0 / symGain);

        negProcessorChain.get<posSymWaveShaper>().process(juce::dsp::ProcessContextReplacing(negBufferBlock));
        negBufferBlock *= (2.0 / symGain);
    }

    inputBlock = posBufferBlock;
    inputBlock += negBufferBlock;
    inputBlock.multiplyBy(0.5);
}

void PolettiDistortion::reset()
{
    posProcessorChain.reset();
    negProcessorChain.reset();
}