/*
  ==============================================================================

    Distortion.h
    Created: 4 Apr 2025 7:00:12pm
    Author:  knize

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
//#include "CustomWaveShaper.cpp"
#include "PolettiDistortion.h"
#include "CustomYamahaClassB.h"
#include "WaveFolderDistortion.h"

enum AmpTypeEnum
{
    Poletti,
    Yamaha,
    WaveFolder
};

//enum PolettiType
//{
//    AsymPos,
//    AsymNeg,
//    Sym
//};
//
//enum processChainEnum
//{
//    posAsymWaveShaper,
//    posLRFilter,
//    posTPTFilter,
//    posLadderFilter,
//    posBTRFilter,
//    posSVFilter,
//    posSymWaveShaper/*,
//    SecondDCFilter*/
//};

//enum FilterTypeEnum
//{
//    Linkwitz,
//    TPT,
//    Ladder,
//    Butterworth,
//    SV
//};

template <typename SampleType>
class Distortion 
{
public:
    Distortion();
    void prepare(juce::dsp::ProcessSpec& spec);
    void reset();

    //template <typename ProcessContext>
    //void process(ProcessContext& context)
    //{
    //    const auto& inputBlock = context.getInputBlock();
    //    auto& outputBlock = context.getOutputBlock();
    //    juce::dsp::AudioBlock<float> posBufferBlock(posBuffer);
    //    posBufferBlock.copyFrom(outputBlock);
    //    const auto numChannels = outputBlock.getNumChannels();
    //    const auto numSamples = outputBlock.getNumSamples();

    //    auto inputBlockLeft = outputBlock.getSingleChannelBlock(0);
    //    auto inputBlockRight = outputBlock.getSingleChannelBlock(1);
    //    auto posBufferBlockLeft = posBufferBlock.getSingleChannelBlock(0);
    //    auto posBufferBlockRight = posBufferBlock.getSingleChannelBlock(1);

    //    jassert(inputBlock.getNumChannels() == numChannels);
    //    jassert(inputBlock.getNumSamples() == numSamples);

    //    if (context.isBypassed)
    //    {
    //        outputBlock.copyFrom(inputBlock);
    //        return;
    //    }

    //    juce::dsp::ProcessContextReplacing<float> leftNegContext(inputBlockLeft);
    //    juce::dsp::ProcessContextReplacing<float> rightNegContext(inputBlockRight);
    //    juce::dsp::ProcessContextReplacing<float> leftPosContext(posBufferBlockLeft);
    //    juce::dsp::ProcessContextReplacing<float> rightPosContext(posBufferBlockRight);

    //    wsPolettiAsymNeg.process(leftNegContext);
    //    wsPolettiAsymPos.process(leftPosContext);
    //    //LDCfilter.process(leftNegContext);
    //    //RDCfilter.process(leftPosContext);
    //    wsPolettiSym.process(leftNegContext);
    //    wsPolettiSym.process(leftPosContext);
    //    outputBlock.replaceWithSumOf(outputBlock, posBufferBlock);
    //    outputBlock.multiplyBy(0.5);
    //    //LDCfilter.process(juce::dsp::ProcessContextReplacing<float>(outputBlock.getSingleChannelBlock(0)));


    //}

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
            //posBufferBlock = upsampledBlock;
            //negBufferBlock = upsampledBlock;
            //polettiDistortion.process(upsampledBlock);

            //yamahaClassBDistortion.process(upsampledBlock);
        }
        else {
            upsampledBlock = inputBlock;
            //posBufferBlock = inputBlock;
            //negBufferBlock = inputBlock;
            //polettiDistortion.process(inputBlock);

            //yamahaClassBDistortion.process(inputBlock);
        }
        //negBufferBlock.multiplyBy(0.5);
        const auto numChannels = inputBlock.getNumChannels();
        const auto numSamples = inputBlock.getNumSamples();

        //auto inputBlockLeft = inputBlock.getSingleChannelBlock(0);
        //auto inputBlockRight = inputBlock.getSingleChannelBlock(1);
        /*auto posBufferBlockLeft = posBufferBlock.getSingleChannelBlock(0);
        auto posBufferBlockRight = posBufferBlock.getSingleChannelBlock(1);
        auto negBufferBlockLeft = negBufferBlock.getSingleChannelBlock(0);
        auto negBufferBlockRight = negBufferBlock.getSingleChannelBlock(1);*/

        jassert(inputBlock.getNumChannels() == numChannels);
        jassert(inputBlock.getNumSamples() == numSamples);

        //if (context.isBypassed)
        //{
        //    //inputBlock.copyFrom(inputBlock);
        //    return;
        //}

        /*juce::dsp::ProcessContextReplacing<float> leftNegContext(negBufferBlockLeft);
        juce::dsp::ProcessContextReplacing<float> rightNegContext(negBufferBlockRight);
        juce::dsp::ProcessContextReplacing<float> leftPosContext(posBufferBlockLeft);
        juce::dsp::ProcessContextReplacing<float> rightPosContext(posBufferBlockRight);*/

        /*wsPolettiAsymPos.process(juce::dsp::ProcessContextNonReplacing<float>(inputBlock, posBufferBlock));
        wsPolettiAsymNeg.process(juce::dsp::ProcessContextNonReplacing<float>(inputBlock, negBufferBlock));*/
        
        //posProcessorChain.reset();
        /*posProcessorChain.process(juce::dsp::ProcessContextReplacing<float>(posBufferBlock));
        negProcessorChain.process(juce::dsp::ProcessContextReplacing<float>(negBufferBlock));*/

        //LDCfilter.process(juce::dsp::ProcessContextReplacing<float>(inputBlockLeft));
        //RDCfilter.process(juce::dsp::ProcessContextReplacing<float>(inputBlockLeft));
        //TPTfilter.process(juce::dsp::ProcessContextReplacing<float>(inputBlock));
        //LDRfilter.process(juce::dsp::ProcessContextReplacing<float>(inputBlockLeft));
        //LeftPosBTRFilter.process(juce::dsp::ProcessContextReplacing<float>(inputBlockLeft));
        //LeftNegBTRFilter.process(leftNegContext);
        //SVfilter.process(juce::dsp::ProcessContextReplacing<float>(inputBlock)); // -1.5 dB @ 20Hz, ~0.2-3 dB ripple, same as BTRFilter
        //posBTRFilter.reset();
        
        /*auto coeff = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(13, sampleRate, 2);
        updateCoefficients(posBTRFilter.state, coeff[0]);
        posBTRFilter.process(juce::dsp::ProcessContextReplacing<float>(posBufferBlock));*/

        //posSVfilter.state->type = juce::dsp::StateVariableFilter::Parameters<float>::Type::highPass;
        //posSVfilter.state->setCutOffFrequency(sampleRate, 8, 1/std::sqrt(2));
        //posSVfilter.process(juce::dsp::ProcessContextNonReplacing<float>(inputBlock, posBufferBlock));

        //negSVfilter.state->type = juce::dsp::StateVariableFilter::Parameters<float>::Type::highPass;
        //negSVfilter.state->setCutOffFrequency(sampleRate, 8, 1 / std::sqrt(2));
        //negSVfilter.process(juce::dsp::ProcessContextNonReplacing<float>(posBufferBlock, blockAfterPosWS));

        /*wsPolettiSym.reset();
        wsPolettiSym.process(leftNegContext);
        wsPolettiSym.reset();
        wsPolettiSym.process(leftPosContext);*/

        /*posSVfilter.state->type = juce::dsp::StateVariableFilter::Parameters<float>::Type::highPass;
        posSVfilter.state->setCutOffFrequency(sampleRate, 8, 1/std::sqrt(2));
        posSVfilter.process(juce::dsp::ProcessContextNonReplacing<float>(inputBlock, posBufferBlock;

        negSVfilter.state->type = juce::dsp::StateVariableFilter::Parameters<float>::Type::highPass;
        negSVfilter.state->setCutOffFrequency(sampleRate, 8, 1 / std::sqrt(2));
        negSVfilter.process(juce::dsp::ProcessContextNonReplacing<float>(inputBlock, negBufferBlock));*/

        /*posProcessorChain.process(juce::dsp::ProcessContextReplacing(posBufferBlock));
        negProcessorChain.process(juce::dsp::ProcessContextReplacing(negBufferBlock));
        inputBlock = posBufferBlock;
        inputBlock += negBufferBlock;*/

        ////==========================================================================================================================
        ////both positive and negative WS work by themselves fine like this
        //if (!asymBypassed) {
        //    posProcessorChain.get<posAsymWaveShaper>().process(juce::dsp::ProcessContextReplacing(posBufferBlock));
        //    float asymGain = posProcessorChain.get<posAsymWaveShaper>().asymPosGain;
        //    posBufferBlock *= (2.0 / asymGain);

        //    negProcessorChain.get<posAsymWaveShaper>().process(juce::dsp::ProcessContextReplacing(negBufferBlock));
        //    negBufferBlock *= (2.0 / asymGain);
        //}
        ////==========================================================================================================================
        //
        ////==========================================================================================================================
        ////both filters work fine by themselves
        //if (!filterBypassed) {
        //    switch (filterType) {
        //    case posLRFilter:
        //        posProcessorChain.get<1>().process(juce::dsp::ProcessContextReplacing(posBufferBlock));
        //        negProcessorChain.get<1>().process(juce::dsp::ProcessContextReplacing(negBufferBlock));
        //        break;
        //    case posTPTFilter:
        //        posProcessorChain.get<2>().process(juce::dsp::ProcessContextReplacing(posBufferBlock));
        //        negProcessorChain.get<2>().process(juce::dsp::ProcessContextReplacing(negBufferBlock));
        //        break;
        //    case posLadderFilter:
        //        posProcessorChain.get<3>().process(juce::dsp::ProcessContextReplacing(posBufferBlock));
        //        negProcessorChain.get<3>().process(juce::dsp::ProcessContextReplacing(negBufferBlock));
        //        break;
        //    case posBTRFilter:
        //        posProcessorChain.get<4>().process(juce::dsp::ProcessContextReplacing(posBufferBlock));
        //        negProcessorChain.get<4>().process(juce::dsp::ProcessContextReplacing(negBufferBlock));
        //        break;
        //    case posSVFilter:
        //        posProcessorChain.get<5>().process(juce::dsp::ProcessContextReplacing(posBufferBlock));
        //        negProcessorChain.get<5>().process(juce::dsp::ProcessContextReplacing(negBufferBlock));
        //        break;
        //    }
        //    //negProcessorChain.get<FirstDCFilter>().process(juce::dsp::ProcessContextReplacing(inputBlock));
        //}
        ////==========================================================================================================================
        // 
        ////==========================================================================================================================
        ////both work by themselves just fine
        //if (!symBypassed) {
        //    posProcessorChain.get<posSymWaveShaper>().process(juce::dsp::ProcessContextReplacing(posBufferBlock));
        //    float symGain = posProcessorChain.get<posSymWaveShaper>().symGain;
        //    posBufferBlock *= (2.0 / symGain);

        //    negProcessorChain.get<posSymWaveShaper>().process(juce::dsp::ProcessContextReplacing(negBufferBlock));
        //    negBufferBlock *= (2.0 / symGain);
        //}
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
            //upsampledBlock = posBufferBlock;
            //upsampledBlock += negBufferBlock;
            //upsampledBlock.multiplyBy(0.5);
            oversampler.processSamplesDown(inputBlock);
        }
        //==========================================================================================================================
        else {
            /*inputBlock = posBufferBlock;
            inputBlock += negBufferBlock;
            inputBlock.multiplyBy(0.5);*/
            //inputBlock.multiplyBy(1 / (posProcessorChain.get<posSymWaveShaper>().symGain));
        }
        //==========================================================================================================================

        

        //posProcessorChain.get<FirstDCFilter>().process(juce::dsp::ProcessContextReplacing(inputBlock));
        /*juce::Range<float> minMax = inputBlock.findMinAndMax();
        float max = std::abs(minMax.getEnd());
        float min = std::abs(minMax.getStart());
        if (min > max)
        {
            max = min;
        }
        inputBlock.multiplyBy(0.99 / max);*/

        /*inputBlock.replaceWithSumOf(posBufferBlock, posBufferBlock);
        inputBlock.multiplyBy(0.5);
        juce::Range<float> minMax = inputBlock.findMinAndMax();
        float max = std::abs(minMax.getEnd());
        float min = std::abs(minMax.getStart());
        if (min > max)
        {
            max = min;
        }
        inputBlock.multiplyBy(0.99 / max);*/

        //posSVfilter.process(juce::dsp::ProcessContextReplacing<float>(inputBlock));

        //LDCfilter.process(juce::dsp::ProcessContextReplacing<float>(outputBlock.getSingleChannelBlock(0)));


    }

    /*SampleType processSample(SampleType inputSample)
    {
        switch (ampType) {
            case AmpTypeEnum::Poletti:
            {
                polettiBufferAsymNeg = DCfilter.processSample(0,processPolettiSample(inputSample, PolettiType::AsymNeg));
                polettiBufferAsymPos = DCfilter.processSample(0,processPolettiSample(inputSample, PolettiType::AsymPos));
                polettiBufferSym = processPolettiSample(polettiBufferAsymNeg, PolettiType::Sym);
                polettiBufferSym += processPolettiSample(polettiBufferAsymPos, PolettiType::Sym);
                polettiBufferSym = polettiBufferSym / 2;
                polettiBufferSym = DCfilter.processSample(0, polettiBufferSym);
                return polettiBufferSym;
                break;
            }
            case AmpTypeEnum::Placeholder:
            {
                return processPlaceholderSample(inputSample);
                break;
            }
        }
    }

    float processPolettiSample(float xn, PolettiType polType)
    {
        switch (polType)
        {
            case PolettiType::AsymNeg:
            {
                if (xn <= 0)
                    return ((asymNegGain * xn) / (1.0 - ((asymNegGain * xn) / asymNegLN)));
                else
                    return ((asymNegGain * xn) / (1.0 + ((asymNegGain * xn) / asymNegLP)));
            }
            case PolettiType::AsymPos:
            {
                if (xn <= 0)
                    return (asymPosGain * xn) / (1.0 - ((asymPosGain * xn) / asymPosLN));
                else
                    return (asymPosGain * xn) / (1.0 + ((asymPosGain * xn) / asymPosLP));
            }
            case PolettiType::Sym:
            {
                if (xn <= 0)
                    return (symGain * xn) / (1.0 - ((symGain * xn) / symLPLN));
                else
                    return (symGain * xn) / (1.0 + ((symGain * xn) / symLPLN));
            }
        }
    }

    SampleType processPlaceholderSample(SampleType inputSample)
    {
        return inputSample;
    }*/

    void setInput(SampleType newInput);
    void setMix(SampleType newMix);
    void setOutput(SampleType newOutput);
    void setAmpType(AmpTypeEnum newAmpType);
    
    void updateCoefficients(juce::dsp::IIR::Filter<float>::CoefficientsPtr& old, const juce::dsp::IIR::Filter<float>::CoefficientsPtr& replacements)
    {
        *old = *replacements;
    }

    //float polettiBufferAsymPos = 0, polettiBufferAsymNeg = 0, polettiBufferSym = 0;

    AmpTypeEnum ampType = AmpTypeEnum::Poletti;
    //processChainEnum filterType = processChainEnum::posTPTFilter;

    //bool asymBypassed{ false }, filterBypassed{ false }, symBypassed{ false }, osBypassed{ false };
    bool osBypassed{ false };

    juce::AudioBuffer<float> posBuffer;
    
    juce::dsp::Gain<float> preAmpGain, postAmpGain;
    float drive{ 0 }/*, postGain{ 0 }*/;

    //juce::dsp::LinkwitzRileyFilter<float> LDCfilter, RDCfilter; // even more phase shift
    //juce::dsp::FirstOrderTPTFilter<float> TPTfilter; // smaller phase shift but too gentle
    //juce::dsp::LadderFilter<float> LDRfilter; // same phase shift as BTRFilter
    //juce::dsp::IIR::Filter<float> LeftPosBTRFilter, LeftNegBTRFilter; // quite a big phase shift
    //juce::dsp::StateVariableTPTFilter<float> SVfilter; // same phase shift as TPTfilter and StateVariableFilter

    //juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> posBTRFilter, negBTRfilter;
    //juce::dsp::ProcessorDuplicator<juce::dsp::StateVariableFilter::Filter<float>, juce::dsp::StateVariableFilter::Parameters<float>> posSVfilter, negSVfilter;
    /*using WaveShaper = CustomWaveShaper<float, std::function<float(float)>>;
    using Lr = juce::dsp::LinkwitzRileyFilter<float>;
    using Tpt = juce::dsp::FirstOrderTPTFilter<float>;
    using Ldr = juce::dsp::LadderFilter<float>;
    using Btr = juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>;
    using Sv = juce::dsp::StateVariableTPTFilter<float>;
    juce::dsp::ProcessorChain<WaveShaper, Lr, Tpt, Ldr, Btr, Sv, WaveShaper> posProcessorChain, negProcessorChain;*/

    PolettiDistortion polettiDistortion;
    CustomYamahaClassB yamahaClassBDistortion;
    WaveFolderDistortion waveFolderDistortion;

    //CustomWaveShaper<float, std::function<float(float)>> wsPolettiAsymPos, wsPolettiAsymNeg, wsPolettiSym;
private:
    juce::SmoothedValue<float> input;
    juce::SmoothedValue<float> mix;
    juce::SmoothedValue<float> output;

    juce::dsp::Oversampling<float> oversampler;

    /*float asymPosGain = 1.7, asymNegGain = 1.7, symGain = 4.0, asymPosLP = 23.6, asymPosLN = 0.5, asymNegLP = 0.5, asymNegLN = 23.6, symLPLN = 1.01;

    juce::dsp::LinkwitzRileyFilter<float> DCfilter;*/

    float sampleRate{ 0 };
    
    PolettiType polettiType = PolettiType::AsymNeg;
};

