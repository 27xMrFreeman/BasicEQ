
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

    posBuffer.setSize(spec.numChannels, spec.maximumBlockSize * oversampler.getOversamplingFactor());
    preAmpGain.reset();
    preAmpGain.prepare(spec);
    preAmpGain.setGainDecibels(0);
    postAmpGain.reset();
    postAmpGain.prepare(spec);
    postAmpGain.setGainDecibels(0);

    polettiDistortion.prepare(spec, oversampler.getOversamplingFactor());

    yamahaClassBDistortion.prepare(spec, oversampler.getOversamplingFactor());

    waveFolderDistortion.prepare(spec, oversampler.getOversamplingFactor());

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

template class Distortion<float>;
template class Distortion<double>;