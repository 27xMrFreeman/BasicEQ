
#pragma once
#include <JuceHeader.h>
#include "PolettiDistortion.h"
#include "CustomYamahaClassB.h"
#include "WaveFolderDistortion.h"

enum AmpTypeEnum
{
    Poletti,
    Yamaha,
    WaveFolder
};

template <typename SampleType>
class Distortion 
{
public:
    Distortion();
    void prepare(juce::dsp::ProcessSpec& spec);
    void reset();

    void process(juce::dsp::AudioBlock<float>& inputBlock)
    {
        juce::dsp::AudioBlock<float> upsampledBlock(posBuffer);

        polettiDistortion.posProcessorChain.get<posAsymWaveShaper>().asymPosGain = drive * 4;
        polettiDistortion.negProcessorChain.get<posAsymWaveShaper>().asymNegGain = drive * 4;
        polettiDistortion.posProcessorChain.get<posSymWaveShaper>().symGain = std::pow((0.316f * drive), 2) * 4;
        polettiDistortion.negProcessorChain.get<posSymWaveShaper>().symGain = std::pow((0.316f * drive), 2) * 4;

        yamahaClassBDistortion.a = 0.2 * std::pow(drive, 2);
        yamahaClassBDistortion.b = 0.0003 * std::pow(drive, 4);

        //==========================================================================================================================
        //oversampling
        if (!osBypassed) {
            upsampledBlock = oversampler.processSamplesUp(inputBlock);
        }
        else {
            upsampledBlock = inputBlock;
        }
        const auto numChannels = inputBlock.getNumChannels();
        const auto numSamples = inputBlock.getNumSamples();

        jassert(inputBlock.getNumChannels() == numChannels);
        jassert(inputBlock.getNumSamples() == numSamples);

        //==========================================================================================================================
        // distortion processing
        switch (ampType) {
        case AmpTypeEnum::Poletti:
            
            preAmpGain.setGainDecibels((drive * 3) - 10);
            preAmpGain.process(juce::dsp::ProcessContextReplacing<float>(upsampledBlock));

            polettiDistortion.process(upsampledBlock);
            
            postAmpGain.setGainDecibels((std::pow(drive, 0.46f) * 15) - 21); // when drive = 0.1 postampGain should be -16dB, with drive = 10, postampGain should be >>4db
            postAmpGain.process(juce::dsp::ProcessContextReplacing<float>(upsampledBlock));
            
            break;
            
        case AmpTypeEnum::Yamaha:
            preAmpGain.setGainDecibels(drive * 2.4);
            preAmpGain.process(juce::dsp::ProcessContextReplacing<float>(upsampledBlock));

            yamahaClassBDistortion.process(upsampledBlock);

            postAmpGain.setGainDecibels(60 * std::exp(-1.34 * drive) - 7/*postGain*/); // [(0.1,53),(0.2,41),(0.3,34),(0.4,28),(1,11),(2,-2),(3,-6),(4,-7),(5,-7),(6,-7),(7,-7),(8,-7),(9,-7),(10,-7)]
            postAmpGain.process(juce::dsp::ProcessContextReplacing<float>(upsampledBlock));

            break;

        case AmpTypeEnum::WaveFolder:
            preAmpGain.setGainDecibels(drive * 10);
            preAmpGain.process(juce::dsp::ProcessContextReplacing<float>(upsampledBlock));

            waveFolderDistortion.process(upsampledBlock);
            
            break;
        }
        

        //==========================================================================================================================
        //downsampling
        if (!osBypassed) {
            oversampler.processSamplesDown(inputBlock);
        }
        //==========================================================================================================================
        else {
            // GUI doesnt have a switch for oversampling
        }
        //==========================================================================================================================

    }


    void setInput(SampleType newInput);
    void setMix(SampleType newMix);
    void setOutput(SampleType newOutput);
    void setAmpType(AmpTypeEnum newAmpType);
    
    void updateCoefficients(juce::dsp::IIR::Filter<float>::CoefficientsPtr& old, const juce::dsp::IIR::Filter<float>::CoefficientsPtr& replacements)
    {
        *old = *replacements;
    }

    AmpTypeEnum ampType = AmpTypeEnum::Poletti;

    bool osBypassed{ false };

    juce::AudioBuffer<float> posBuffer;
    
    juce::dsp::Gain<float> preAmpGain, postAmpGain;
    float drive{ 0 };

    PolettiDistortion polettiDistortion;
    CustomYamahaClassB yamahaClassBDistortion;
    WaveFolderDistortion waveFolderDistortion;

private:
    juce::SmoothedValue<float> input;
    juce::SmoothedValue<float> mix;
    juce::SmoothedValue<float> output;

    juce::dsp::Oversampling<float> oversampler;

    float sampleRate{ 0 };
    
    PolettiType polettiType = PolettiType::AsymNeg;
};

