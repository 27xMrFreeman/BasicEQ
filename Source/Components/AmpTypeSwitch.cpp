/*
  ==============================================================================

    AmpTypeSwitch.cpp
    Created: 5 May 2025 4:40:34pm
    Author:  knize

  ==============================================================================
*/

#include <JuceHeader.h>
#include "AmpTypeSwitch.h"

//==============================================================================
AmpTypeSwitch::AmpTypeSwitch()
{
    ampTypeBox.addItem("Poletti", 1);
    ampTypeBox.addItem("Yamaha", 2);
    ampTypeBox.addItem("WaveFolder", 3);

    polettiAmpTypeButton.setRadioGroupId(RadioButtonIDs::AmpTypeButtons);
    polettiAmpTypeButton.setClickingTogglesState(true);
    polettiAmpTypeButton.setLookAndFeel(&lnf);

    yamahaAmpTypeButton.setRadioGroupId(RadioButtonIDs::AmpTypeButtons);
    yamahaAmpTypeButton.setClickingTogglesState(true);
    yamahaAmpTypeButton.setLookAndFeel(&lnf);

    wavefoldAmpTypeButton.setRadioGroupId(RadioButtonIDs::AmpTypeButtons);
    wavefoldAmpTypeButton.setClickingTogglesState(true);
    wavefoldAmpTypeButton.setLookAndFeel(&lnf);

    addAndMakeVisible(ampTypeBox);
    addAndMakeVisible(yamahaAmpTypeButton);
    addAndMakeVisible(polettiAmpTypeButton);
    addAndMakeVisible(wavefoldAmpTypeButton);
}

AmpTypeSwitch::~AmpTypeSwitch()
{
    polettiAmpTypeButton.setLookAndFeel(nullptr);
    yamahaAmpTypeButton.setLookAndFeel(nullptr);
    wavefoldAmpTypeButton.setLookAndFeel(nullptr);
}

void AmpTypeSwitch::paint (juce::Graphics& g)
{
    juce::Image switchImage = juce::ImageCache::getFromMemory(BinaryData::AmpTypeSwitch_png, BinaryData::AmpTypeSwitch_pngSize);

    g.setColour(juce::Colours::blue);
    g.fillEllipse(yBtnBounds);
    g.setColour(juce::Colours::red);
    g.fillEllipse(pBtnBounds);
    g.setColour(juce::Colours::purple);
    g.fillEllipse(wBtnBounds);

    g.drawImage(switchImage, imageBounds, juce::RectanglePlacement::stretchToFit, false);
}

void AmpTypeSwitch::resized()
{
    auto bounds = getLocalBounds().toFloat();
    imageBounds = bounds;

    // 30px top and bottom space, 25px button, 17px between buttons when height = 169px
    float btnHeight = (bounds.getHeight() / 169.f) * 25;
    float outsideSpace = (bounds.getHeight() / 169.f) * 30;
    float insideSpace = (bounds.getHeight() / 169.f) * 17;
    float leftSpace = (bounds.getWidth() / 87.f) * 32;
    float rightSpace = (bounds.getWidth() / 87.f) * 30;

    bounds.removeFromLeft(leftSpace/2);
    bounds.removeFromRight(rightSpace/2);
    bounds.removeFromTop(outsideSpace);
    yBtnBounds = bounds.removeFromTop(btnHeight);
    yamahaAmpTypeButton.setBounds(yBtnBounds.toNearestInt());

    bounds.removeFromTop(insideSpace);
    pBtnBounds = bounds.removeFromTop(btnHeight);
    polettiAmpTypeButton.setBounds(pBtnBounds.toNearestInt());

    bounds.removeFromTop(insideSpace);
    wBtnBounds = bounds.removeFromTop(btnHeight);
    wavefoldAmpTypeButton.setBounds(wBtnBounds.toNearestInt());
}
