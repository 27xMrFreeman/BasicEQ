/*
  ==============================================================================

    HelpButton.h
    Created: 17 May 2025 3:56:41pm
    Author:  knize

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
struct HelpButton  : public juce::Component, public juce::SettableTooltipClient
{
public:
    void paint (juce::Graphics& g) override
    {
        auto bounds = getLocalBounds();
        g.setColour(juce::Colour::fromString("FF555555"));
        auto customFont = juce::Font(juce::Typeface::createSystemTypefaceFor(BinaryData::InstrumentSansBold_ttf,
            BinaryData::InstrumentSansBold_ttfSize));
        g.setFont(customFont.withHeight(bounds.getHeight()).withExtraKerningFactor(0.06));
        g.drawFittedText("help", bounds, juce::Justification::bottomLeft, 1);
    }
};
