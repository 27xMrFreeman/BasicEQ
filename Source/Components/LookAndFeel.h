/*
  ==============================================================================

    LookAndFeel.h
    Created: 25 Feb 2025 7:23:45pm
    Author:  knize

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>


struct LookAndFeel : juce::LookAndFeel_V4
{
    void drawRotarySlider(juce::Graphics&,
        int x, int y, int width, int height,
        float sliderPosProportional,
        float rotaryStartAngle,
        float rotaryEndAngle,
        juce::Slider&) override;

    void drawToggleButton(juce::Graphics& g,
        juce::ToggleButton& toggleButton,
        bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown) override;

    void drawButtonBackground(juce::Graphics& g,
        juce::Button& button,
        const juce::Colour& backgroundColour,
        bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown) override;

    void drawButtonText(juce::Graphics& g,
        juce::TextButton& button,
        bool /*shouldDrawButtonAsHighlighted*/,
        bool /*shouldDrawButtonAsDown*/) override;

    juce::Typeface::Ptr getTypefaceForFont(const juce::Font& f) override
    {
        static juce::Typeface::Ptr myFont = juce::Typeface::createSystemTypefaceFor(BinaryData::InstrumentSansBold_ttf,
                                                                                    BinaryData::InstrumentSansBold_ttfSize);
        return myFont;
    }

    static const juce::Font& getCustomFont()
    {
        static juce::Font customFont(juce::Font(juce::Typeface::createSystemTypefaceFor(BinaryData::InstrumentSansBold_ttf,
                                                                                        BinaryData::InstrumentSansBold_ttfSize)));
        return customFont;
    }

    juce::Colour bypassButtonEdgeColor{ juce::Colours::white }, bypassButtonFillColor{ juce::Colours::white };
};

struct LookAndFeelIRBypass : juce::LookAndFeel_V4
{
    void drawToggleButton(juce::Graphics& g,
        juce::ToggleButton& toggleButton,
        bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown) override;
};

struct LookAndFeelChoiceButtons : juce::LookAndFeel_V4
{
    void drawToggleButton(juce::Graphics& g,
        juce::ToggleButton& toggleButton,
        bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown) override;

    static const juce::Font& getCustomFont()
    {
        static juce::Font customFont(juce::Font(juce::Typeface::createSystemTypefaceFor(BinaryData::InstrumentSansBold_ttf,
            BinaryData::InstrumentSansBold_ttfSize)));
        return customFont;
    }
};

struct LookAndFeelAmpTypeButtons : juce::LookAndFeel_V4
{
    void drawToggleButton(juce::Graphics& g,
        juce::ToggleButton& toggleButton,
        bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown) override;
};