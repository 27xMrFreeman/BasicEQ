
#include "LookAndFeel.h"
#include "RotarySliderWithLabels.h"

void LookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)
{
    using namespace juce;

    auto imageBounds = Rectangle<float>(x, y, width, height);
    auto sliderBounds = imageBounds.reduced(width * 0.1);
    juce::Image sliderImage = juce::ImageCache::getFromMemory(BinaryData::Knob_png, BinaryData::Knob_pngSize);

    //====================================================================================
    // slider should be ellipse, cut off with rectangle with height proportional to slider value

    g.fillEllipse(sliderBounds);

    auto sliderFilledArea = sliderBounds;
    auto sliderValMappedToHeight = jmap(slider.getValue(), slider.getMinimum(), slider.getMaximum(), 0.0, (double)sliderBounds.getHeight());
    sliderFilledArea.removeFromBottom(sliderValMappedToHeight);
    g.setColour(Colour::fromString("FFFCA311"));
    
    g.fillRect(sliderFilledArea);

    g.drawImage(sliderImage, imageBounds.toFloat(), RectanglePlacement::stretchToFit, false);

    if (auto* rswl = dynamic_cast<RotarySliderWithLabels*>(&slider))
    {        
        g.setFont(rswl->getTextHeight());                           // sets basic font with set height
        auto text = rswl->getDisplayString();                       // gets text to put in
        auto strWidth = GlyphArrangement::getStringWidthInt(g.getCurrentFont(), text);  //g.getCurrentFont().getStringWidth(text);    // gets width of text

        Rectangle<float> r;
        r.setSize(strWidth + 4, rswl->getTextHeight() + 2);         // rectangle r is little bigger than the text
        r.setCentre(sliderBounds.getCentre());                            // set centre of the rectangle to centre of bounds (slider)

        g.setColour(Colour::fromString("FFE5E5E5"));
        g.drawFittedText(text, r.toNearestInt(), juce::Justification::centred, 1);
        
    }
}

//====================================================================================
// drawing amp bypass button
void LookAndFeel::drawToggleButton(juce::Graphics& g,
    juce::ToggleButton& toggleButton,
    bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown)
{
    using namespace juce;

    auto bounds = toggleButton.getLocalBounds();
    auto size = jmin(bounds.getWidth(), bounds.getHeight()) - 5;
    auto r = bounds.withSizeKeepingCentre(size, size).toFloat();

    if (!toggleButton.getToggleState())    g.setColour(bypassButtonFillColor); //ON
    else g.setColour(Colours::black);                                         //OFF

    g.fillEllipse(bounds.reduced(bounds.getWidth() * 0.2).toFloat());
}

// drawing background image of Load IR button 
void LookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto btnImage = juce::ImageCache::getFromMemory(BinaryData::LoadIRBtn_png, BinaryData::LoadIRBtn_pngSize);
    auto bounds = button.getLocalBounds();
    bounds.reduce(bounds.getWidth() * 0.16, 0);

    g.drawImage(btnImage, bounds.toFloat(), juce::RectanglePlacement::stretchToFit, false);
    
}

void LookAndFeel::drawButtonText(juce::Graphics& g, juce::TextButton& button, bool, bool)
{
    g.setColour(juce::Colour::fromString("FFE5E5E5"));
    g.setFont(getCustomFont().withHeight(button.getLocalBounds().getHeight()*0.33).withExtraKerningFactor(0.02));
    g.drawFittedText("load", button.getLocalBounds(), juce::Justification::centred, 1);
}

// drawing IR bypass button
void LookAndFeelIRBypass::drawToggleButton(juce::Graphics& g, juce::ToggleButton& toggleButton, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    using namespace juce;

    auto bounds = toggleButton.getLocalBounds();
    
    bounds.reduce(bounds.getWidth() * 0.32, bounds.getWidth() * 0.32);

    g.setColour(Colour::fromString("FF4F83F3"));
    g.drawEllipse(bounds.toFloat(), bounds.getWidth() * 0.3);

    if (!toggleButton.getToggleState())    g.setColour(Colour::fromString("FF4F83F3")); //ON
    else g.setColour(Colour::fromString("FF14213D"));                                  //OFF

    g.fillEllipse(bounds.reduced(bounds.getWidth() * 0.1).toFloat());
}

// drawing mic and combo type buttons
void LookAndFeelChoiceButtons::drawToggleButton(juce::Graphics& g, juce::ToggleButton& toggleButton, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    using namespace juce;

    auto bounds = toggleButton.getLocalBounds();

    auto onBtnImage = ImageCache::getFromMemory(BinaryData::ChoiceOn_png, BinaryData::ChoiceOn_pngSize);
    auto offBtnImage = ImageCache::getFromMemory(BinaryData::ChoiceOff_png, BinaryData::ChoiceOff_pngSize);

    if (toggleButton.getToggleState()) {
        g.drawImage(onBtnImage, bounds.toFloat(), RectanglePlacement::stretchToFit, false);
    }
    else {
        g.drawImage(offBtnImage, bounds.toFloat(), RectanglePlacement::stretchToFit, false);
    }
    
    auto name = toggleButton.getName();
    g.setColour(Colour::fromString("FFE5E5E5"));
    g.setFont(getCustomFont().withHeight(bounds.getHeight()*0.33));
    g.drawFittedText(name, bounds, juce::Justification::centred, 1);
}

// drawing amp type buttons
void LookAndFeelAmpTypeButtons::drawToggleButton(juce::Graphics& g, juce::ToggleButton& toggleButton, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = toggleButton.getLocalBounds();
    auto h = bounds.getHeight();
    bounds = bounds.withSizeKeepingCentre(h, h);

    // semi transparent black drawn over OFF buttons
    g.setColour(juce::Colour::fromRGBA(0, 0, 0, 150));
    if (!toggleButton.getToggleState()) {
        g.fillEllipse(bounds.toFloat());
    }
}
