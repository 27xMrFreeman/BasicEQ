/*
  ==============================================================================

    LookAndFeel.cpp
    Created: 25 Feb 2025 7:23:45pm
    Author:  knize

  ==============================================================================
*/

#include "LookAndFeel.h"
#include "RotarySliderWithLabels.h"

void LookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)
{
    using namespace juce;

    auto imageBounds = Rectangle<float>(x, y, width, height);
    auto sliderBounds = imageBounds.reduced(width * 0.1);
    juce::Image sliderImage = juce::ImageCache::getFromMemory(BinaryData::Knob_png, BinaryData::Knob_pngSize);

    // for debugging
    /*g.setColour(Colour(Colours::red));
    g.drawRect(imageBounds);
    g.drawRect(sliderBounds);*/

    //g.setColour(Colour(72u, 30u, 20u));
    //g.setColour(findColour(juce::Slider::rotarySliderFillColourId));
    //g.setColour(sliderColour);

    //====================================================================================
    // slider should be rectangle with height proportional to slider value

    g.fillEllipse(sliderBounds);

    auto sliderFilledArea = sliderBounds;
    auto sliderValMappedToHeight = jmap(slider.getValue(), slider.getMinimum(), slider.getMaximum(), 0.0, (double)sliderBounds.getHeight());
    sliderFilledArea.removeFromBottom(sliderValMappedToHeight);
    g.setColour(Colour::fromString("FFFCA311"));
    
    g.fillRect(sliderFilledArea);

    g.drawImage(sliderImage, imageBounds.toFloat(), RectanglePlacement::stretchToFit, false);

    if (auto* rswl = dynamic_cast<RotarySliderWithLabels*>(&slider))
    {
        //if (slider.isMouseOverOrDragging())
        {
            g.setFont(rswl->getTextHeight());                           // sets basic font with set height
            auto text = rswl->getDisplayString();                       // gets text to put in
            auto strWidth = GlyphArrangement::getStringWidthInt(g.getCurrentFont(), text);  //g.getCurrentFont().getStringWidth(text);    // gets width of text

            Rectangle<float> r;
            r.setSize(strWidth + 4, rswl->getTextHeight() + 2);         // rectangle r is little bigger than the text
            r.setCentre(sliderBounds.getCentre());                            // set centre of the rectangle to centre of bounds (slider)

            /*g.setColour(Colours::black);
            g.fillRect(r);*/

            g.setColour(Colours::white);
            g.drawFittedText(text, r.toNearestInt(), juce::Justification::centred, 1);
        }
    }
    //====================================================================================
    // old circular sliders
    //g.fillEllipse(bounds);

    //g.setColour(Colour(242u, 97u, 63u));
    //g.drawEllipse(bounds, 1.5);

    //if (auto* rswl = dynamic_cast<RotarySliderWithLabels*>(&slider))
    //{
    //    auto center = bounds.getCentre();
    //    //whatever we want to rotate needs to be in a path
    //    Path p;

    //    Rectangle<float> r;
    //    r.setLeft(center.getX() - 2);   // left side of rectangle 2 pixels left of center
    //    r.setRight(center.getX() + 2);  // right side of rectangle 2 pixels right of center
    //    r.setTop(bounds.getY());        // top of rectangle = top of bounds
    //    r.setBottom(center.getY() - rswl->getTextHeight() * 1.5);     // bottom of rectangle = text height above center

    //    p.addRoundedRectangle(r, 2.f);

    //    jassert(rotaryStartAngle < rotaryEndAngle); // check if start angle is smaller than end angle

    //    auto sliderAngRad = jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle); // mapping normalised slider value to angles

    //    p.applyTransform(AffineTransform().rotated(sliderAngRad, center.getX(), center.getY())); // transform rotates path by sliderAngRad with x and y of pivot point

    //    g.fillPath(p);

    //    //// TRYING TO PUT IMAGE HERE
    //    //juce::Image knobRed = ImageCache::getFromMemory(BinaryData::knob_red_png, BinaryData::knob_red_pngSize);
    //    //juce::Image knobRedRescaled = knobRed.rescaled(bounds.getWidth() / knobRed.getWidth() * knobRed.getWidth(), bounds.getHeight() / knobRed.getHeight() * knobRed.getHeight(), Graphics::highResamplingQuality);
    //    //
    //    //AffineTransform rotator;
    //    ////if (!slider.isMouseOverOrDragging())
    //    ////{
    //    ////    g.drawImage(knobRed, bounds, RectanglePlacement::stretchToFit, false);
    //    ////}
    //    ////else
    //    ////{
    //    ////    //g.drawImage(knobRed, x, y, width, height, 0, 0, width, height, false);
    //    ////    g.drawImage(knobRed, bounds, RectanglePlacement::stretchToFit, false);
    //    ////}
    //    ////g.drawImageTransformed(knobRed, x, y, width, height, rotator.rotated((float)sliderPosProportional * rotaryEndAngle, (float)(knobRed.getWidth() / 2), (float)(knobRed.getHeight() / 2)), false);
    //    ////g.drawImage(knobRedRescaled, bounds, RectanglePlacement::stretchToFit, false);
    //    //int origX = g.getClipBounds().getX();
    //    //int origY = g.getClipBounds().getY();
    //    //g.drawImageTransformed(knobRedRescaled, rotator.rotated(sliderAngRad, knobRedRescaled.getWidth() / 2, knobRedRescaled.getHeight() / 2).translated(bounds.getX()-origX, bounds.getY()-origY));


    //    // TRYING TO PUT IMAGE HERE
    //}

}

//====================================================================================

void LookAndFeel::drawToggleButton(juce::Graphics& g,
    juce::ToggleButton& toggleButton,
    bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown)
{
    using namespace juce;

    //Path powerButton;

    auto bounds = toggleButton.getLocalBounds();
    auto size = jmin(bounds.getWidth(), bounds.getHeight()) - 5;
    auto r = bounds.withSizeKeepingCentre(size, size).toFloat();

    /*g.setColour(Colours::red);
    g.drawRect(bounds);*/

    //g.setColour(bypassButtonEdgeColor);
    //g.drawEllipse(bounds.toFloat(),bounds.getWidth()*0.05);

    if (toggleButton.getToggleState())    g.setColour(bypassButtonFillColor); //ON
    else g.setColour(Colours::black);                                         //OFF

    g.fillEllipse(bounds.reduced(bounds.getWidth() * 0.2).toFloat());
    //auto name = toggleButton.getName();

    //float ang = 30.f;

    //size -= 6;

    //powerButton.addCentredArc(r.getCentreX(), r.getCentreY(), size * 0.5, size * 0.5, 0.f, degreesToRadians(ang), degreesToRadians(360.f - ang), true);

    //powerButton.startNewSubPath(r.getCentreX(), r.getY() + 3);
    //powerButton.lineTo(r.getCentre());

    //PathStrokeType pst(2.f, PathStrokeType::JointStyle::curved);

    //auto color = toggleButton.getToggleState() ? Colours::dimgrey : Colours::lightgreen;

    //g.setColour(color);
    ////g.strokePath(powerButton, pst);
    //size = jmin(bounds.getWidth(), bounds.getHeight()) - 3;
    //r = bounds.withSizeKeepingCentre(size, size).toFloat();
    //g.setColour(Colours::silver);
    ////g.drawEllipse(r, 2);

}

void LookAndFeelIRBypass::drawToggleButton(juce::Graphics& g, juce::ToggleButton& toggleButton, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    using namespace juce;

    auto bounds = toggleButton.getLocalBounds();

    g.setColour(Colours::red);
    g.drawRect(bounds);
    
    g.setColour(Colour::fromString("FF4F83F3"));
    g.drawEllipse(bounds.reduced(bounds.getWidth() * 0.06).toFloat(), bounds.getWidth() * 0.1);

    if (toggleButton.getToggleState())    g.setColour(Colour::fromString("FF4F83F3")); //ON
    else g.setColour(Colour::fromString("FF14213D"));                                  //OFF

    g.fillEllipse(bounds.reduced(bounds.getWidth() * 0.1).toFloat());
}

void LookAndFeelChoiceButtons::drawToggleButton(juce::Graphics& g, juce::ToggleButton& toggleButton, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    using namespace juce;

    auto bounds = toggleButton.getLocalBounds();

    g.setColour(Colours::red);
    g.drawRect(bounds);

    auto onBtnImage = ImageCache::getFromMemory(BinaryData::ChoiceOn_png, BinaryData::ChoiceOn_pngSize);
    auto offBtnImage = ImageCache::getFromMemory(BinaryData::ChoiceOff_png, BinaryData::ChoiceOff_pngSize);

    if (toggleButton.getToggleState()) {
        g.drawImage(onBtnImage, bounds.toFloat(), RectanglePlacement::stretchToFit, false);
    }
    else {
        g.drawImage(offBtnImage, bounds.toFloat(), RectanglePlacement::stretchToFit, false);
    }

}

void LookAndFeelAmpTypeButtons::drawToggleButton(juce::Graphics& g, juce::ToggleButton& toggleButton, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = toggleButton.getLocalBounds();
    //auto w = bounds.getWidth();
    auto h = bounds.getHeight();
    bounds = bounds.withSizeKeepingCentre(h, h);

    g.setColour(juce::Colour::fromRGBA(0, 0, 0, 150));
    if (!toggleButton.getToggleState()) {
        g.fillEllipse(bounds.toFloat());
    }
}
