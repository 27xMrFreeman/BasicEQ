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

    //g.setColour(Colours::red);
    //g.drawRect(bounds);

    //g.setColour(Colours::white.withBrightness(0.1f));
    //g.fillRoundedRectangle(bounds, 5.f);

    g.setColour(Colour::fromString("FFFCA311"));
    /*g.fillRoundedRectangle(bounds.reduced(0, 50), 17);*/

    //g.setColour(Colours::lightgreen);
    const auto scaledX = jmap(level, -60.f, +6.f, 0.f, static_cast<float>(getHeight()));
    g.fillRoundedRectangle(bounds.removeFromBottom(scaledX), 17.f);

    //g.setColour(Colours::silver);
    //g.drawRoundedRectangle(r, 10.f, 5.f);

}

void HorizontalMeterRight::paint(juce::Graphics& g)
{
    using namespace juce;
    auto bounds = getLocalBounds().toFloat();
    auto r = bounds;

    //g.setColour(Colours::red);
    //g.drawRect(bounds);

    //g.setColour(Colours::white.withBrightness(0.1f));
    //g.fillRoundedRectangle(bounds, 5.f);

    //g.setColour(Colours::lightgreen);
    g.setColour(Colour::fromString("FFFCA311"));
    const auto scaledX = jmap(level, -60.f, +6.f, 0.f, static_cast<float>(getHeight()));
    g.fillRoundedRectangle(bounds.removeFromBottom(scaledX), 17.f);


    //g.setColour(Colours::silver);
    //g.drawRoundedRectangle(r, 5.f, 5.f);

}