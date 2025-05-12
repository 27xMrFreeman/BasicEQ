/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <algorithm>
#include "PluginProcessor.h"
#include "HorizontalMeter.h"
#include "Components/AnalyzerPathGenerator.h"
#include "Components/FFTDataGenerator.h"
#include "Components/FFTOrder.h"
#include "Components/IrFFTComponent.h"
#include "Components/LookAndFeel.h"
#include "Components/PathProducer.h"
#include "Components/ResponseCurveComponent.h"
#include "Components/RotarySliderWithLabels.h"
#include "Components/XYPad.h"
#include "Components/AmpTypeSwitch.h"

//==============================================================================
/**
*/
class BasicEQAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::MultiTimer
{
public:
    BasicEQAudioProcessorEditor (BasicEQAudioProcessor&);
    ~BasicEQAudioProcessorEditor() override;

    //==============================================================================
    void timerCallback(int timerID) override;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::SliderAttachment;
    using ButtonAttachment = APVTS::ButtonAttachment;
    
    BasicEQAudioProcessor& audioProcessor;

    enum RadioButtonIDs { AmpTypeButtons = 1001, MicTypeButtons = 1002, CabTypeButtons = 1003 };

    juce::TooltipWindow tooltipWindow;

    // background image
    juce::Image backgroundImage;

    float fontSize{ 0 };

    //===============================================================================================================================
    // meters + gain
    
    HorizontalMeterLeft meterInLeft;
    HorizontalMeterLeft meterOutLeft;
    HorizontalMeterRight meterInRight;
    HorizontalMeterRight meterOutRight;
    DecibelGrid inputDecibelGrid, outputDecibelGrid;

    RotarySliderWithLabels inputGainSlider, outputGainSlider;
    Attachment inputGainSliderAttachment, outputGainSliderAttachment;

    juce::Label inputGainLabel{ {}, "in" }, outputGainLabel{ {}, "out" };

    //===============================================================================================================================
    // Amp GUI
    
    RotarySliderWithLabels driveSlider;
    Attachment driveSliderAttachment;

    juce::ToggleButton ampBypassButton;
    ButtonAttachment ampBypassButtonAttachment;

    juce::Label driveLabel{ {}, "drive" }, ampTypeLabel{ {}, "type" }, ampBypassLabel{ {}, "bypass" };
    
    AmpTypeSwitch ampTypeSwitch;
    //===============================================================================================================================
    // Tone Stack GUI

    RotarySliderWithLabels lowShelfSlider, midPeakSlider, highShelfSlider;
    Attachment lowShelfSliderAttachment, midPeakSliderAttachment, highShelfSliderAttachment;

    juce::Label lowLabel{ {}, "low" }, midLabel{ {}, "mid" }, highLabel{ {}, "high" };
    
    //===============================================================================================================================
    // IR loader GUI:
    
    juce::TextButton loadBtn;
    std::unique_ptr<juce::FileChooser> fileChooser;
    juce::Label irNameLabel;
    RotarySliderWithLabels xPosSlider, yPosSlider;
    Attachment xPosSliderAttachment, yPosSliderAttachment;

    XYPad xyPad;

    juce::ComboBox comboTypeBox, mikTypeBox;
    std::unique_ptr< juce::AudioProcessorValueTreeState::ComboBoxAttachment > comboTypeBoxAttachment, mikTypeBoxAttachment;
    juce::ToggleButton aMicButton, bMicButton, cMicButton, aCabButton, bCabButton, cCabButton;
    juce::Atomic<bool> userIRLoaded{ false }, needIRUpdate{ false };
    juce::ToggleButton irBypassButton;
    ButtonAttachment irBypassButtonAttachment;
    IrFFTComponent irfftComponent;
    
    juce::Label xyPadLabel{ {}, "mic position" }, irfftLabel{ {}, "response" }, micButtonsLabel{ {}, "mic type" }, cabButtonsLabel{ {}, "cab type" }, irBypassLabel{ {}, "bypass" };

    std::unique_ptr<juce::AlertWindow> alertWindow;
    //===============================================================================================================================
    // EQ GUI:
        // knobs
    RotarySliderWithLabels peakFreqSlider,
        peakGainSlider,
        peakQualitySlider,
        lowCutFreqSlider,
        highCutFreqSlider,
        lowCutSlopeSlider,
        highCutSlopeSlider;
    juce::ToggleButton lowCutBypassButton, peakBypassButton, highCutBypassButton;
        // rendered response curve of EQ filters line
    ResponseCurveComponent responseCurveComponent;

        // attaching knobs to values
    Attachment
        peakFreqSliderAttachment,
        peakGainSliderAttachment,
        peakQualitySliderAttachment,
        lowCutFreqSliderAttachment,
        highCutFreqSliderAttachment,
        lowCutSlopeSliderAttachment,
        highCutSlopeSliderAttachment;
    
    ButtonAttachment lowCutBypassButtonAttachment, peakBypassButtonAttachment, highCutBypassButtonAttachment;

    std::vector<juce::Component*> getComps();
    std::vector<juce::Label*> getLabels();

    LookAndFeel lnf;
    LookAndFeelIRBypass lnfIRBypass;
    LookAndFeelChoiceButtons lnfChoices;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicEQAudioProcessorEditor)
};


