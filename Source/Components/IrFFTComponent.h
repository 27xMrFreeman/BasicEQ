
#pragma once
#include <JuceHeader.h>
#include "../PluginProcessor.h"
#include "PathProducer.h"

struct IrFFTComponent : juce::Component/*,
    juce::AudioProcessorParameter::Listener*/
{
    IrFFTComponent(BasicEQAudioProcessor&);
    ~IrFFTComponent();

    void loadedIRChanged(juce::File newIR);
    void loadedIRChanged(const juce::AudioBuffer<float>& newIR, const int& sampleRate);
    void paint(juce::Graphics& g) override;
    void resized() override;
private:
    BasicEQAudioProcessor& audioProcessor;
    juce::Atomic<bool> parametersChanged{ false };
    MonoChain monoChain;

    juce::Image background;

    juce::dsp::FFT fft{ FFTOrder::order4096 };
    PathProducer leftPathProducer, rightPathProducer;

    juce::Rectangle<int> getRenderArea();
    juce::Rectangle<int> getAnalysisArea();
};