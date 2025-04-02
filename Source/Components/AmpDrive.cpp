/*
  ==============================================================================

    AmpDrive.cpp
    Created: 2 Apr 2025 6:23:52pm
    Author:  knize

  ==============================================================================
*/

#include <JuceHeader.h>
#include "AmpDrive.h"

//==============================================================================
AmpDrive::AmpDrive()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    asymNegGain = 1.7;
    asymNegLN = asymPosLP = 23.6;
    asymNegLP = asymPosLN = 0.5;
    symGain = 4.0;
    symLPLN = 1.01;

    wsPolettiAsymNeg.functionToUse = [this](float x) {
        float y = 0;
        if (x <= 0)
        {
            y = (wsPolettiAsymNeg.asymNegGain * x) / (1.0 - ((asymNegGain * x) / asymNegLN));
        }
        else
        {
            y = (wsPolettiAsymNeg.asymNegGain * x) / (1.0 + ((asymNegGain * x) / asymNegLP));
        }
        return y;
        };
}

AmpDrive::~AmpDrive()
{
}

void AmpDrive::reset()
{
    wsPoletti.reset();
}

void AmpDrive::prepare(const juce::dsp::ProcessSpec& spec)
{
    wsPoletti.prepare(spec);
}

template <typename ProcessContext>
void AmpDrive::process(const ProcessContext& context)
{

}

void AmpDrive::simTypeChanged(int simTypeID)
{
    switch (simTypeID)
        case 0:
            wsPoletti.functionToUse = polettiFunc();
}

void AmpDrive::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (14.0f));
    g.drawText ("AmpDrive", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
}

void AmpDrive::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

float polettiFunc(float x, float gain, float posLim, float negLim)
{
    float y = 0;
    if (x <= 0)
    {
        y = (gain * x) / (1.0 - ((gain * x) / negLim));
    }
    else
    {
        y = (gain * x) / (1.0 + ((gain * x) / posLim));
    }
    return y;
}