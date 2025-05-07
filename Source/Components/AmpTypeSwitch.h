
#pragma once

#include <JuceHeader.h>
#include "LookAndFeel.h"
//==============================================================================
/*
*/
class AmpTypeSwitch  : public juce::Component
{
public:
    AmpTypeSwitch();
    ~AmpTypeSwitch() override;

    void paint (juce::Graphics&) override;
    void resized() override;


    enum RadioButtonIDs { AmpTypeButtons = 1001, MicTypeButtons = 1002, CabTypeButtons = 1003 };

    juce::ComboBox ampTypeBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> ampTypeBoxAttachment;
    juce::ToggleButton yamahaAmpTypeButton, polettiAmpTypeButton, wavefoldAmpTypeButton;

    juce::Rectangle<float> imageBounds, yBtnBounds, pBtnBounds, wBtnBounds;

    LookAndFeelAmpTypeButtons lnf;
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmpTypeSwitch)
};
