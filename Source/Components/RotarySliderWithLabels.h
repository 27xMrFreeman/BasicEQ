/*
  ==============================================================================

    RotarySliderWithLabels.h
    Created: 25 Feb 2025 7:23:59pm
    Author:  knize

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "LookAndFeel.h"

struct RotarySliderWithLabels : juce::Slider
{
    RotarySliderWithLabels(juce::RangedAudioParameter& rap, const juce::String& unitSuffix) :
        juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
            juce::Slider::TextEntryBoxPosition::NoTextBox),
        param(&rap),
        suffix(unitSuffix)
    {
        setLookAndFeel(&lnf); // set lnf as the look and feel object
    }

    ~RotarySliderWithLabels()
    {
        setLookAndFeel(nullptr);
    }

    struct LabelPos
    {
        float pos;
        juce::String label;
    };

    juce::Array<LabelPos> labels;

    juce::String name{ "name" };

    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getSliderBounds() const;
    int getTextHeight() const { return 14; }
    juce::String getDisplayString() const;
    void setSliderColor(juce::Colour newColor);

private:
    LookAndFeel lnf;
    /*LookAndFeelBlue lnfb;
    LookAndFeelGreen lnfg;
    LookAndFeelBlack lnfk;*/
    juce::Colour sliderColor{ juce::Colours::black };
    juce::RangedAudioParameter* param;
    juce::String suffix;
};