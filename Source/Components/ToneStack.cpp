/*
  ==============================================================================

    ToneStack.cpp
    Created: 12 Apr 2025 10:02:55pm
    Author:  knize

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ToneStack.h"

//==============================================================================
ToneStack::ToneStack()
{
}

ToneStack::~ToneStack()
{
}

void ToneStack::prepare(juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    
    tsBandPass.reset();
    *tsBandPass.state = *juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, 50, 0.222);
    tsBandPass.prepare(spec);

    tsBandPassGain.reset();
    tsBandPassGain.setGainDecibels(3.5);
    tsBandPassGain.prepare(spec);

    tsHighPass.reset();
    *tsHighPass.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass(sampleRate, 750);
    tsHighPass.prepare(spec);

    tsHighPassGain.reset();
    tsHighPassGain.setGainDecibels(2);
    tsHighPassGain.prepare(spec);

    tsLowShelf.reset();
    *tsLowShelf.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, 180, 0.5, 1);
    tsLowShelf.prepare(spec);

    tsMidPeak.reset();
    *tsMidPeak.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 500, 0.707, 1);
    tsMidPeak.prepare(spec);

    tsHighShelf.reset();
    *tsHighShelf.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, 1250, 0.5, 1);
    tsHighShelf.prepare(spec);

    bufferBP.setSize(spec.numChannels, spec.maximumBlockSize);
    bufferHP.setSize(spec.numChannels, spec.maximumBlockSize);

}

void ToneStack::reset()
{
    tsBandPass.reset();
    tsBandPassGain.reset();
    tsHighPass.reset();
    tsHighPassGain.reset();
    tsLowShelf.reset();
    tsMidPeak.reset();
    tsHighShelf.reset();
}

void ToneStack::process(juce::dsp::AudioBlock<float>& inputBlock)
{
    juce::dsp::AudioBlock<float> blockBP(bufferBP), blockHP(bufferHP);
    
    // processing first two filters in parallel, these shape the response when all controls are neutral
    tsHighPass.process(juce::dsp::ProcessContextNonReplacing<float>(inputBlock, blockHP));
    tsHighPassGain.process(juce::dsp::ProcessContextReplacing<float>(blockHP));

    tsBandPass.process(juce::dsp::ProcessContextNonReplacing<float>(inputBlock, blockBP));
    tsBandPassGain.process(juce::dsp::ProcessContextNonReplacing<float>(blockBP, inputBlock));

    inputBlock.add(blockHP);
    // processing main (controlled) tone stack filters in series
    tsLowShelf.process(juce::dsp::ProcessContextReplacing<float>(inputBlock));
    tsMidPeak.process(juce::dsp::ProcessContextReplacing<float>(inputBlock));
    tsHighShelf.process(juce::dsp::ProcessContextReplacing<float>(inputBlock));

}

void ToneStack::setLowGain(float newGainDB)
{
    float newGain = juce::Decibels::decibelsToGain(newGainDB);
    *tsLowShelf.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, 180, 0.5, newGain);
}

void ToneStack::setMidGain(float newGainDB)
{
    float newGain = juce::Decibels::decibelsToGain(newGainDB);
    *tsMidPeak.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 500, 0.707, newGain);
}

void ToneStack::setHighGain(float newGainDB)
{
    float newGain = juce::Decibels::decibelsToGain(newGainDB);
    *tsHighShelf.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, 1250, 0.5, newGain);
}

