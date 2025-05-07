/*
  ==============================================================================

    HorizontalMeter.cpp
    Created: 25 May 2024 5:11:02pm
    Author:  knize

  ==============================================================================
*/

#include "HorizontalMeter.h"

void HorizontalMeterLeft::paint(juce::Graphics& g)
{
    using namespace juce;
    auto bounds = getLocalBounds().toFloat();
    auto r = bounds;

    // maps level value in decibels to height of meter
    const auto scaledX = jmap(level, -60.f, +6.f, 0.f, static_cast<float>(getHeight()));
    // if level clips, meter turns red
    if (level >= 0.f) { g.setColour(juce::Colours::red); }
    else { g.setColour(Colour::fromString("FFFCA311")); }
    g.fillRoundedRectangle(bounds.removeFromBottom(scaledX), 17.f);

    // marking 0 dB line
    const auto zeroPos = jmap(0.f, -60.f, +6.f, 0.f, static_cast<float>(getHeight()));
    r.removeFromBottom(zeroPos);
    r.removeFromTop(r.getHeight() * 0.95);
    r.reduce(r.getWidth() * 0.0625, 0);
    g.setColour(juce::Colour::fromString("FFE5E5E5"));
    g.fillRect(r);


}

void HorizontalMeterRight::paint(juce::Graphics& g)
{
    using namespace juce;
    auto bounds = getLocalBounds().toFloat();
    auto r = bounds;

    if (level >= 0.f) { g.setColour(juce::Colours::red); }
    else { g.setColour(Colour::fromString("FFFCA311")); }
    const auto scaledX = jmap(level, -60.f, +6.f, 0.f, static_cast<float>(getHeight()));
    g.fillRoundedRectangle(bounds.removeFromBottom(scaledX), 17.f);

    const auto zeroPos = jmap(0.f, -60.f, +6.f, 0.f, static_cast<float>(getHeight()));
    r.removeFromBottom(zeroPos);
    r.removeFromTop(r.getHeight() * 0.95);
    r.reduce(r.getWidth() * 0.0625, 0);
    g.setColour(juce::Colour::fromString("FFE5E5E5"));
    g.fillRect(r);
}