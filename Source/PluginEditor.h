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

    // background image
    juce::Image backgroundImage;

    //===============================================================================================================================
    // meters + gain
    
    HorizontalMeterLeft meterInLeft;
    HorizontalMeterLeft meterOutLeft;
    HorizontalMeterRight meterInRight;
    HorizontalMeterRight meterOutRight;
    
    RotarySliderWithLabels inputGainSlider, outputGainSlider;
    Attachment inputGainSliderAttachment, outputGainSliderAttachment;

    //===============================================================================================================================
    // Amp GUI
    
    RotarySliderWithLabels driveSlider;
    Attachment driveSliderAttachment;

    juce::ComboBox ampTypeBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> ampTypeBoxAttachment;
    
    juce::ToggleButton polettiAmpTypeButton, yamahaAmpTypeButton, wavefoldAmpTypeButton;
    //ButtonAttachment polettiAmpTypeButtonAttachment, yamahaAmpTypeButtonAttachment, wavefoldAmpTypeButtonAttachment;
    juce::ToggleButton ampBypassButton/*, osBypassButton*/;
    ButtonAttachment ampBypassButtonAttachment/*, osBypassButtonAttachment*/;
    
    //===============================================================================================================================
    // Tone Stack GUI

    RotarySliderWithLabels lowShelfSlider, midPeakSlider, highShelfSlider;
    Attachment lowShelfSliderAttachment, midPeakSliderAttachment, highShelfSliderAttachment;
    
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

    LookAndFeel lnf;
    /*LookAndFeelBlue lnfb;
    LookAndFeelGreen lnfg;
    LookAndFeelBlack lnfk;*/

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicEQAudioProcessorEditor)
};


