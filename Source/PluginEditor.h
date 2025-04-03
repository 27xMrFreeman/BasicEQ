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

//==============================================================================
/**
*/
class BasicEQAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    BasicEQAudioProcessorEditor (BasicEQAudioProcessor&);
    ~BasicEQAudioProcessorEditor() override;

    //==============================================================================
    void timerCallback() override;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    BasicEQAudioProcessor& audioProcessor;

    // background image
    juce::Image backgroundImage;

    // meters
    HorizontalMeterLeft meterInLeft;
    HorizontalMeterLeft meterOutLeft;
    HorizontalMeterRight meterInRight;
    HorizontalMeterRight meterOutRight;

    // Amp GUI
    RotarySliderWithLabels asymPosGainSlider,
        asymNegGainSlider,
        symGainSlider,
        symLPLNSlider,
        asymPosLPSlider,
        asymPosLNSlider,
        asymNegLPSlider,
        asymNegLNSlider;
    juce::ComboBox ampTypeBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> ampTypeBoxAttachment;
    juce::ToggleButton ampBypassButton;
    // IR loader GUI:
    juce::TextButton loadBtn;
    std::unique_ptr<juce::FileChooser> fileChooser;
    juce::Label irNameLabel;
    RotarySliderWithLabels xPosSlider, yPosSlider;
    juce::ComboBox comboTypeBox, mikTypeBox;
    std::unique_ptr< juce::AudioProcessorValueTreeState::ComboBoxAttachment > comboTypeBoxAttachment, mikTypeBoxAttachment;
    juce::Atomic<bool> userIRLoaded{ false };
    
    IrFFTComponent irfftComponent;

    // EQ GUI:
        // knobs
    RotarySliderWithLabels peakFreqSlider,
        peakGainSlider,
        peakQualitySlider,
        lowCutFreqSlider,
        highCutFreqSlider,
        lowCutSlopeSlider,
        highCutSlopeSlider,
        inputGainSlider,
        outputGainSlider;
    juce::ToggleButton lowCutBypassButton, peakBypassButton, highCutBypassButton, irBypassButton;
        // rendered response curve of EQ filters line
    ResponseCurveComponent responseCurveComponent;

        // attaching knobs to values
    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::SliderAttachment;

    Attachment
        peakFreqSliderAttachment,
        peakGainSliderAttachment,
        peakQualitySliderAttachment,
        lowCutFreqSliderAttachment,
        highCutFreqSliderAttachment,
        lowCutSlopeSliderAttachment,
        highCutSlopeSliderAttachment,
        xPosSliderAttachment,
        yPosSliderAttachment,
        inputGainSliderAttachment,
        outputGainSliderAttachment,
        asymPosGainSliderAttachment,
        asymNegGainSliderAttachment,
        symGainSliderAttachment,
        symLPLNSliderAttachment,
        asymPosLPSliderAttachment,
        asymPosLNSliderAttachment,
        asymNegLPSliderAttachment,
        asymNegLNSliderAttachment;

    using ButtonAttachment = APVTS::ButtonAttachment;
    ButtonAttachment lowCutBypassButtonAttachment, peakBypassButtonAttachment, highCutBypassButtonAttachment, irBypassButtonAttachment, ampBypassButtonAttachment;

    std::vector<juce::Component*> getComps();

    LookAndFeel lnf;
    /*LookAndFeelBlue lnfb;
    LookAndFeelGreen lnfg;
    LookAndFeelBlack lnfk;*/

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicEQAudioProcessorEditor)
};


