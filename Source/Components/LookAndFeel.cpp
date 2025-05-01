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

    auto bounds = Rectangle<float>(x, y, width, height);

   /* g.setColour(Colour(Colours::red));
    g.drawRect(bounds);*/

    //g.setColour(Colour(72u, 30u, 20u));
    //g.setColour(findColour(juce::Slider::rotarySliderFillColourId));
    //g.setColour(sliderColour);
    g.fillEllipse(bounds);

    //g.setColour(Colour(242u, 97u, 63u));
    g.drawEllipse(bounds, 1.5);

    if (auto* rswl = dynamic_cast<RotarySliderWithLabels*>(&slider))
    {
        auto center = bounds.getCentre();
        //whatever we want to rotate needs to be in a path
        Path p;

        Rectangle<float> r;
        r.setLeft(center.getX() - 2);   // left side of rectangle 2 pixels left of center
        r.setRight(center.getX() + 2);  // right side of rectangle 2 pixels right of center
        r.setTop(bounds.getY());        // top of rectangle = top of bounds
        r.setBottom(center.getY() - rswl->getTextHeight() * 1.5);     // bottom of rectangle = text height above center

        p.addRoundedRectangle(r, 2.f);

        jassert(rotaryStartAngle < rotaryEndAngle); // check if start angle is smaller than end angle

        auto sliderAngRad = jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle); // mapping normalised slider value to angles

        p.applyTransform(AffineTransform().rotated(sliderAngRad, center.getX(), center.getY())); // transform rotates path by sliderAngRad with x and y of pivot point

        g.fillPath(p);

        //// TRYING TO PUT IMAGE HERE
        //juce::Image knobRed = ImageCache::getFromMemory(BinaryData::knob_red_png, BinaryData::knob_red_pngSize);
        //juce::Image knobRedRescaled = knobRed.rescaled(bounds.getWidth() / knobRed.getWidth() * knobRed.getWidth(), bounds.getHeight() / knobRed.getHeight() * knobRed.getHeight(), Graphics::highResamplingQuality);
        //
        //AffineTransform rotator;
        ////if (!slider.isMouseOverOrDragging())
        ////{
        ////    g.drawImage(knobRed, bounds, RectanglePlacement::stretchToFit, false);
        ////}
        ////else
        ////{
        ////    //g.drawImage(knobRed, x, y, width, height, 0, 0, width, height, false);
        ////    g.drawImage(knobRed, bounds, RectanglePlacement::stretchToFit, false);
        ////}
        ////g.drawImageTransformed(knobRed, x, y, width, height, rotator.rotated((float)sliderPosProportional * rotaryEndAngle, (float)(knobRed.getWidth() / 2), (float)(knobRed.getHeight() / 2)), false);
        ////g.drawImage(knobRedRescaled, bounds, RectanglePlacement::stretchToFit, false);
        //int origX = g.getClipBounds().getX();
        //int origY = g.getClipBounds().getY();
        //g.drawImageTransformed(knobRedRescaled, rotator.rotated(sliderAngRad, knobRedRescaled.getWidth() / 2, knobRedRescaled.getHeight() / 2).translated(bounds.getX()-origX, bounds.getY()-origY));


        // TRYING TO PUT IMAGE HERE


        if (slider.isMouseOverOrDragging())
        {
            g.setFont(rswl->getTextHeight());                           // sets basic font with set height
            auto text = rswl->getDisplayString();                       // gets text to put in
            auto strWidth = GlyphArrangement::getStringWidthInt(g.getCurrentFont(), text);  //g.getCurrentFont().getStringWidth(text);    // gets width of text

            r.setSize(strWidth + 4, rswl->getTextHeight() + 2);         // rectangle r is little bigger than the text
            r.setCentre(bounds.getCentre());                            // set centre of the rectangle to centre of bounds (slider)

            g.setColour(Colours::black);
            g.fillRect(r);

            g.setColour(Colours::white);
            g.drawFittedText(text, r.toNearestInt(), juce::Justification::centred, 1);
        }


    }

}

void LookAndFeel::drawToggleButton(juce::Graphics& g,
    juce::ToggleButton& toggleButton,
    bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown)
{
    using namespace juce;

    Path powerButton;

    auto bounds = toggleButton.getLocalBounds();
    auto size = jmin(bounds.getWidth(), bounds.getHeight()) - 5;
    auto r = bounds.withSizeKeepingCentre(size, size).toFloat();

    float ang = 30.f;

    size -= 6;

    powerButton.addCentredArc(r.getCentreX(), r.getCentreY(), size * 0.5, size * 0.5, 0.f, degreesToRadians(ang), degreesToRadians(360.f - ang), true);

    powerButton.startNewSubPath(r.getCentreX(), r.getY() + 3);
    powerButton.lineTo(r.getCentre());

    PathStrokeType pst(2.f, PathStrokeType::JointStyle::curved);

    auto color = toggleButton.getToggleState() ? Colours::dimgrey : Colours::lightgreen;

    g.setColour(color);
    g.strokePath(powerButton, pst);
    size = jmin(bounds.getWidth(), bounds.getHeight()) - 3;
    r = bounds.withSizeKeepingCentre(size, size).toFloat();
    g.setColour(Colours::silver);
    g.drawEllipse(r, 2);

}