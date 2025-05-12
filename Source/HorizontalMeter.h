
#pragma once

#include <JuceHeader.h>

struct HorizontalMeterLeft : public juce::Component
{
public:
    void paint(juce::Graphics& g) override;
    void setLevel(const float value) { level = value; }

private:
    float level = -60.f;
};

struct HorizontalMeterRight : public juce::Component
{
public:
    void paint(juce::Graphics& g) override;
    void setLevel(const float value) { level = value; }

private:
    float level = -60.f;
};

struct DecibelGrid : public juce::Component
{
public:
    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        /*g.setColour(juce::Colours::red);
        g.drawRect(bounds);*/
        bounds.reduce(bounds.getWidth() * 0.25, 0);
        auto w = bounds.getWidth();
        auto cX = bounds.getCentreX();
        g.setColour(juce::Colour::fromString("FF555555"));
        auto customFont = juce::Font(juce::Typeface::createSystemTypefaceFor(BinaryData::InstrumentSansBold_ttf,
            BinaryData::InstrumentSansBold_ttfSize));
        g.setFont(customFont.withHeight(w*0.8));
        for (int i = 0; i > -30; i = i - 6) {
            auto pos = juce::jmap(i, -60, +6, getHeight(), 0);
            //bounds.removeFromTop(zeroPos);
            g.drawHorizontalLine(pos, cX - w / 2, cX + w / 2);
            auto str = juce::String(i);
            g.drawFittedText(str, cX - w/2, pos + getHeight() * 0.02, w, w * 0.3, juce::Justification::centred, 1, 0.0f);
        }

    }
};
