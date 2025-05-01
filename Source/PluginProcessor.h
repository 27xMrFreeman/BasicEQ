/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <juce_core/juce_core.h>
#include <cmath>
#include "Components/FFTDataGenerator.h"
#include "Components/SingleChannelSampleFifo.h"
//#include "Components/AmpDrive.h"
#include "Components/Distortion.h"
#include "Components/ToneStack.h"

enum Slope
{
    Slope_12,
    Slope_24,
    Slope_36,
    Slope_48
};

enum comboTypeEnum {
    Mar,
    MM,
    SV
};

enum micTypeEnum {
    a57,
    kalib,
    sm57
};

struct ChainSettings
{
    //EQ
    float peakFreq{ 0 }, peakGainInDecibels{ 0 }, peakQuality{ 1.f };
    float lowCutFreq{ 0 }, highCutFreq{ 0 };
    Slope lowCutSlope{ Slope::Slope_12 }, highCutSlope{ Slope::Slope_12 };
    bool lowCutBypassed{ false }, peakBypassed{ false }, highCutBypassed{ false };
    //============================================================================================================================================
    // ToneStack
    float stackLowGain{ 0 }, stackMidGain{ 0 }, stackHighGain{ 0 };

    //============================================================================================================================================
    //IR
    float xPos{ 0 }, yPos{ 0 };
    comboTypeEnum comboType{comboTypeEnum::Mar};
    micTypeEnum micType{ micTypeEnum::a57 };
    bool irBypassed{ false };
    //============================================================================================================================================
    //AMP
    AmpTypeEnum ampType{ AmpTypeEnum::Poletti };
    float drive{ 0 };
    bool ampBypassed{ false }, osBypassed{ false };
    //============================================================================================================================================
    //Gain
    float inputGainInDecibels{0}, outputGainInDecibels{0};
};

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);

using Filter = juce::dsp::IIR::Filter<float>;

using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;

using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;

enum ChainPositions
{
    LowCut,
    Peak,
    HighCut
};

using Coefficients = Filter::CoefficientsPtr;
void updateCoefficients(Coefficients& old, const Coefficients& replacements);

Coefficients makePeakFilter(const ChainSettings& chainSettings, double sampleRate);

template<int Index, typename ChainType, typename CoefficientType>
void update(ChainType& chain, const CoefficientType& cutCoefficients)
{
    updateCoefficients(chain.get<Index>().coefficients, cutCoefficients[Index]);
    chain.setBypassed<Index>(false);
}

template<typename ChainType, typename CoefficientType>
void updateCutFilter(ChainType& chain, const CoefficientType& coefficients, const Slope& slope)
{
    chain.setBypassed<0>(true);
    chain.setBypassed<1>(true);
    chain.setBypassed<2>(true);
    chain.setBypassed<3>(true);

    switch (slope)
    {
    case Slope_48:
    {
        update<3>(chain, coefficients);
    }
    case Slope_36:
    {
        update<2>(chain, coefficients);
    }
    case Slope_24:
    {
        update<1>(chain, coefficients);
    }
    case Slope_12:
    {
        update<0>(chain, coefficients);
    }
    }
}

inline auto makeLowCutFilter(const ChainSettings& chainSettings, double sampleRate)
{
    return juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(chainSettings.lowCutFreq, sampleRate, 2 * (chainSettings.lowCutSlope + 1));
}

inline auto makeHighCutFilter(const ChainSettings& chainSettings, double sampleRate)
{
    return juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(chainSettings.highCutFreq, sampleRate, 2 * (chainSettings.highCutSlope + 1));
}

//==============================================================================
/**
*/
class BasicEQAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    BasicEQAudioProcessor();
    ~BasicEQAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //void updateLoadedIR(juce::AudioBuffer<float>& buffer, int& sampleRate, int comboTypeID, int mikTypeID, float yPos, float xPos);
    void loadShippedImpulseResponses();
    float getInputRMSValue(const int channel) const;
    float getOutputRMSValue(const int channel) const;

    juce::File root, savedFile;
    juce::dsp::Convolution irLoader;
    juce::Array<juce::Array<juce::Array<juce::Array<juce::File>>>> impulseResponseArray;
    static juce::AudioProcessorValueTreeState::ParameterLayout
        createParameterLayout();
    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "Parameters", createParameterLayout() };

    using BlockType = juce::AudioBuffer<float>;
    SingleChannelSampleFifo<BlockType> leftChannelFifo{ Channel::Left };
    SingleChannelSampleFifo<BlockType> rightChannelFifo{ Channel::Right };

    juce::dsp::Gain<float> inputGain, outputGain, preampGain, postampGain;
    // dont need this anymore
    //AmpDrive ampDrive;
    // dont need this anymore
    Distortion<float> ampSim;
    ToneStack toneStack;

    juce::Atomic<bool> newIRReady{ false };

    juce::AudioFormatManager formatManager;

    // AudioBuffers for data to interpolate and interpolants, 2D interpolation between 4 data points + 2 intermediate interpolants + 1 finished interpolant
    juce::AudioBuffer<float> audioBufferInterpBL, audioBufferInterpBR, audioBufferInterpTL, audioBufferInterpTR, /*audioBufferInterpBottom, audioBufferInterpTop,*/ audioBufferInterpFin, newIRAudioBuffer;

    float interpIRSampleRate{ 0 };

private:
    MonoChain leftChain, rightChain;
    //ChainSettings chainSettings;
    juce::AudioBuffer<float> bufferBPContour, bufferHPContour;
    //juce::dsp::IIR::Filter <float> LcontourBP, RcontourBP, LcontourHP, RcontourHP;
    //juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> contourHP, contourBP;
    // contour filters in parallel before tone stack -> response is not linear when all controls on 50%
    juce::dsp::FirstOrderTPTFilter<float> TPTcontourHP;
    juce::dsp::StateVariableTPTFilter<float> TPTcontourBP;
    juce::dsp::Gain<float> contourBPGain, contourHPGain;
    // tone stack filters - approximation from TI´s Tone Stack for Guitar Amplifier Reference Design
    using IIRFilter = juce::dsp::IIR::Filter<float>;
    using IIRFilterCoeff = juce::dsp::IIR::Coefficients<float>;
    juce::dsp::ProcessorChain<juce::dsp::ProcessorDuplicator<IIRFilter, IIRFilterCoeff>, juce::dsp::ProcessorDuplicator<IIRFilter, IIRFilterCoeff>, juce::dsp::ProcessorDuplicator<IIRFilter, IIRFilterCoeff>> toneStackFilters;

    void updatePeakFilter(const ChainSettings& chainSettings);
    void updateLowCutFilter(const ChainSettings& chainSettings);
    void updateHighCutFilter(const ChainSettings& chainSettings);
    void updateToneStackLow(const ChainSettings& chainSettings);
    void updateToneStackMid(const ChainSettings& chainSettings);
    void updateToneStackHigh(const ChainSettings& chainSettings);

    void updateLoadedIR(juce::AudioBuffer<float>& bufferInterp, int& sampleRate, int comboTypeID, int mikTypeID, float yPos, float xPos);

    void updateFilters();

    juce::dsp::Oscillator<float> osc;
    juce::LinearSmoothedValue<float> rmsLevelInputLeft, rmsLevelInputRight, rmsLevelOutputLeft, rmsLevelOutputRight;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicEQAudioProcessor)
};
