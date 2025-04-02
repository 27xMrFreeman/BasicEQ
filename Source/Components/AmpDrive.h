#pragma once

#include <JuceHeader.h>
#include "CustomWaveShaper.cpp"

//==============================================================================
/*
*/
class AmpDrive  : public juce::Component
{
public:
    AmpDrive();
    ~AmpDrive() override;

    void reset();
    void prepare(const juce::dsp::ProcessSpec& spec);
    template <typename ProcessContext>
    void process(const ProcessContext& context);

    void simTypeChanged(int simTypeID);

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    CustomWaveShaper<float, std::function<float (float)>> wsPolettiAsymPos, wsPolettiAsymNeg, wsPolettiSym;
    float asymPosGain, asymNegGain, symGain, asymPosLP, asymPosLN, asymNegLP, asymNegLN, symLPLN;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmpDrive)
};
