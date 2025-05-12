
#include "HorizontalMeter.h"

void HorizontalMeterLeft::paint(juce::Graphics& g)
{
    using namespace juce;
    auto bounds = getLocalBounds().toFloat();
    auto w = bounds.getWidth();

    // maps level value in decibels to height of meter
    const auto scaledX = jmap(level, -60.f, +6.f, 0.f, static_cast<float>(getHeight()));
    // if level clips, meter turns red
    if (level >= 0.f) { g.setColour(juce::Colours::red); }
    else { g.setColour(Colour::fromString("FFFCA311")); }
    g.fillRoundedRectangle(bounds.removeFromBottom(scaledX), w/2);

}

void HorizontalMeterRight::paint(juce::Graphics& g)
{
    using namespace juce;
    auto bounds = getLocalBounds().toFloat();
    auto w = bounds.getWidth();

    if (level >= 0.f) { g.setColour(juce::Colours::red); }
    else { g.setColour(Colour::fromString("FFFCA311")); }
    const auto scaledX = jmap(level, -60.f, +6.f, 0.f, static_cast<float>(getHeight()));
    g.fillRoundedRectangle(bounds.removeFromBottom(scaledX), w/2);

}