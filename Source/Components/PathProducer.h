
#pragma once
#include <JuceHeader.h>
#include "AnalyzerPathGenerator.h"
#include "FFTDataGenerator.h"
#include "SingleChannelSampleFifo.h"
#include "../PluginProcessor.h"


struct PathProducer
{
    PathProducer(SingleChannelSampleFifo<BasicEQAudioProcessor::BlockType>& scsf) :
        leftChannelFifo(&scsf)
    {
        leftChannelFFTDataGenerator.changeOrder(FFTOrder::order4096); // init FFT with set order
        monoBuffer.setSize(1, leftChannelFFTDataGenerator.getFFTSize());  // init mono buffer with 1 channel and as many samples as DataGenerator uses
    }

    void process(juce::Rectangle<float> fftBounds, double sampleRate);
    juce::Path getPath() { return leftChannelFFTPath; }

    AnalyzerPathGenerator<juce::Path> pathProducer;

    juce::Path leftChannelFFTPath;

    FFTDataGenerator<std::vector<float>> leftChannelFFTDataGenerator;

private:
    SingleChannelSampleFifo<BasicEQAudioProcessor::BlockType>* leftChannelFifo;

    juce::AudioBuffer<float> monoBuffer;
};