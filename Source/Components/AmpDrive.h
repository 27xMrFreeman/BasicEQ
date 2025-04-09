#pragma once

#include <JuceHeader.h>
#include "CustomWaveShaper.cpp"

//==============================================================================
/*
*/

enum AmpTypeEnum {
    poletti,
    placeholder
};
class AmpDrive  : public juce::Component
{
public:
    AmpDrive();
    ~AmpDrive() override;

    void reset();
    void prepare(const juce::dsp::ProcessSpec& spec);
    //template <typename ProcessContext>
    void process(juce::dsp::AudioBlock<float>& block);

    void simTypeChanged(int simTypeID);

    void paint (juce::Graphics&) override;
    void resized() override;
    AmpTypeEnum simType;
    juce::AudioBuffer<float> posBuffer, negBuffer, sumBuffer;
    juce::dsp::LinkwitzRileyFilter<float> LDCfilter, RDCfilter;
private:
    CustomWaveShaper<float, std::function<float(float)>> wsPolettiAsymPos, wsPolettiAsymNeg, wsPolettiSym;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmpDrive)
};
