
#include "RotarySliderWithLabels.h"

void RotarySliderWithLabels::paint(juce::Graphics& g)
{
    using namespace juce;

    auto startAng = degreesToRadians(180.f + 35.f);     // angle starts at 12´, 180° = 6´
    auto endAng = degreesToRadians(180.f - 35.f) + MathConstants<float>::twoPi; // adding 2pi to ensure startAng < endAng

    //auto range = getRange();

    auto sliderBounds = getSliderBounds();

    auto nameBounds = getLocalBounds();

    g.setFont(lnf.getCustomFont().withHeight(nameBounds.getHeight()*0.6));
    
    g.setColour(sliderColor);
    getLookAndFeel().drawRotarySlider(g,
        sliderBounds.getX(),
        sliderBounds.getY(),
        sliderBounds.getWidth(),
        sliderBounds.getHeight(),
        param->convertTo0to1(getValue()),
        //jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0), // converts range of slider to 0->1
        startAng,
        endAng,
        *this);
}

juce::Rectangle<int> RotarySliderWithLabels::getSliderBounds() const
{
    auto bounds = getLocalBounds();

    auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());

    juce::Rectangle<int> r;
    r.setSize(size, size);
    r.setCentre(bounds.getCentreX(), 0);
    r.setY(2);
    return r;
}

juce::String RotarySliderWithLabels::getDisplayString() const
{
    if (auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(param)) // if we can convert param to parameter choice, then it is a choice, so we return choice name
    {
        return choiceParam->getCurrentChoiceName();
    }

    juce::String str;
    bool addK = false;

    if (auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param)) // if we can convert param to float then it is float
    {
        // truncating Hz -> kHz
        float val = getValue();
        if (val > 999.f)
        {
            val /= 1000.f;
            addK = true;
        }
        str = juce::String(val, (addK ? 2 : 0)); // limit to 2 decimal places, otherwise use default value
    }
    else
    {
        jassertfalse; // this shouldnt happen, happens only if there is a parameter thats not choice or float
    }

    if (suffix.isNotEmpty())
        str << " ";
    if (addK) str << "k"; // adds the k to kHz
    str << suffix;

    return str;
}

void RotarySliderWithLabels::setSliderColor(juce::Colour newColor)
{
    sliderColor = newColor;
}
