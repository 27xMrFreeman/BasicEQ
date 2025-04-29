/*
  ==============================================================================

    Distortion.cpp
    Created: 4 Apr 2025 7:00:12pm
    Author:  knize

  ==============================================================================
*/

#include "Distortion.h"
template <typename SampleType>
Distortion<SampleType>::Distortion() :
oversampler(2, 2, juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR)
{

}
template <typename SampleType>
void Distortion<SampleType>::prepare(juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;

    oversampler.initProcessing(spec.maximumBlockSize);

    /*LDCfilter.prepare(spec);
    LDCfilter.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    LDCfilter.setCutoffFrequency(10);
    LDCfilter.reset();
    RDCfilter.prepare(spec);
    RDCfilter.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    RDCfilter.setCutoffFrequency(10);
    RDCfilter.reset();

    TPTfilter.prepare(spec);
    TPTfilter.setType(juce::dsp::FirstOrderTPTFilterType::highpass);
    TPTfilter.setCutoffFrequency(5);
    TPTfilter.reset();

    LDRfilter.prepare(spec);
    LDRfilter.setMode(juce::dsp::LadderFilterMode::HPF12);
    LDRfilter.setCutoffFrequencyHz(10);
    LDRfilter.reset();*/

    //auto coeff = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(13, spec.sampleRate, 2);
    //LeftPosBTRFilter.prepare(spec);
    //LeftNegBTRFilter.prepare(spec);
    //updateCoefficients(LeftPosBTRFilter.coefficients, coeff[0]);
    //updateCoefficients(LeftNegBTRFilter.coefficients, coeff[0]);
    //LeftPosBTRFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(spec.sampleRate, 5);
    //LeftPosBTRFilter.reset();
    //LeftNegBTRFilter.reset();

    //SVfilter.reset();
    //SVfilter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    //SVfilter.setCutoffFrequency(5);
    //SVfilter.prepare(spec);
    
    /*posBTRFilter.reset();
    updateCoefficients(posBTRFilter.state, coeff[0]);
    posBTRFilter.prepare(spec);*/

    //posSVfilter.reset();
    //posSVfilter.state->type = juce::dsp::StateVariableFilter::Parameters<float>::Type::highPass;
    //posSVfilter.state->setCutOffFrequency(spec.sampleRate, 5, 1/std::sqrt(2));
    //posSVfilter.prepare(spec);

    //negSVfilter.reset();
    //negSVfilter.state->type = juce::dsp::StateVariableFilter::Parameters<float>::Type::highPass;
    //negSVfilter.state->setCutOffFrequency(spec.sampleRate, 13, 1 / std::sqrt(2));
    //negSVfilter.prepare(spec);

    /*posProcessorChain.reset();
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
    posProcessorChain.get<posBTRFilter>().state = juce::dsp::IIR::Coefficients<float>::makeHighPass(spec.sampleRate, 5);
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
    negProcessorChain.get<posBTRFilter>().state = juce::dsp::IIR::Coefficients<float>::makeHighPass(spec.sampleRate, 5);
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
    negProcessorChain.prepare(spec);*/
    posBuffer.setSize(spec.numChannels, spec.maximumBlockSize * oversampler.getOversamplingFactor());
    //negBuffer.setSize(spec.numChannels, spec.maximumBlockSize * oversampler.getOversamplingFactor());
    preAmpGain.reset();
    preAmpGain.prepare(spec);
    preAmpGain.setGainDecibels(0);
    postAmpGain.reset();
    postAmpGain.prepare(spec);
    postAmpGain.setGainDecibels(0);

    polettiDistortion.prepare(spec, oversampler.getOversamplingFactor());

    yamahaClassBDistortion.prepare(spec, oversampler.getOversamplingFactor());

    waveFolderDistortion.prepare(spec, oversampler.getOversamplingFactor());

    //posBufferBlock(posBuffer);
    // INIT Poletti functions and variables
    /*wsPolettiAsymNeg.asymNegGain = 1.7;
    wsPolettiAsymNeg.asymPosGain = 1.7;
    wsPolettiAsymNeg.asymNegLN = wsPolettiAsymNeg.asymPosLP = 23.6;
    wsPolettiAsymNeg.asymNegLP = wsPolettiAsymNeg.asymPosLN = 0.5;
    wsPolettiAsymNeg.symGain = 4.0;
    wsPolettiAsymNeg.symLPLN = 1.01;*/

    /*wsPolettiAsymNeg.functionToUse = [this](float x) {
        float y = 0;
        if (x <= 0)
        {
            y = (1.7 * x) / (1.0 - ((1.7 * x) / 23.6));
        }
        else
        {
            y = (1.7 * x) / (1.0 + ((1.7 * x) / 0.5));
        }
        return y;
        };
    wsPolettiAsymPos.functionToUse = [this](float x) {
        float y = 0;
        if (x <= 0)
        {
            y = (1.7 * x) / (1.0 - ((1.7 * x) / 0.5));
        }
        else
        {
            y = (1.7 * x) / (1.0 + ((1.7 * x) / 23.6));
        }
        return y;
        };
    wsPolettiSym.functionToUse = [this](float x) {
        float y = 0;
        if (x <= 0)
        {
            y = (4 * x) / (1.0 - ((4 * x) / 1.01));
        }
        else
        {
            y = (4 * x) / (1.0 + ((4 * x) / 1.01));
        }
        return y;
        };*/
}

template <typename SampleType>
void Distortion<SampleType>::reset()
{
    if (sampleRate <= 0) return;
    input.reset(sampleRate, 0.02);
    input.setTargetValue(0.0);

    output.reset(sampleRate, 0.02);
    output.setTargetValue(0.0);

    mix.reset(sampleRate, 0.02);
    mix.setTargetValue(1.0);

    /*negSVfilter.reset();
    posSVfilter.reset();

    wsPolettiAsymNeg.reset();
    wsPolettiAsymPos.reset();
    wsPolettiSym.reset();*/

    polettiDistortion.reset();

    

}

template<typename SampleType>
void Distortion<SampleType>::setInput(SampleType newInput)
{
    input.setTargetValue(newInput);
}

template<typename SampleType>
void Distortion<SampleType>::setMix(SampleType newMix)
{
    mix.setTargetValue(newMix);
}

template<typename SampleType>
void Distortion<SampleType>::setOutput(SampleType newOutput)
{
    output.setTargetValue(newOutput);
}

template<typename SampleType>
void Distortion<SampleType>::setAmpType(AmpTypeEnum newAmpType)
{
    ampType = newAmpType;
}

//template <typename SampleType, typename ProcessContext>
//void Distortion<SampleType>::process(ProcessContext& context)
//{
//
//}
//
//template<typename SampleType>
//void Distortion<SampleType>::processSample(SampleType inputSample)
//{
//}

template class Distortion<float>;
template class Distortion<double>;