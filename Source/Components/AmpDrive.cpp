
#include <JuceHeader.h>
#include "AmpDrive.h"

//==============================================================================
AmpDrive::AmpDrive()
{
    // INIT Poletti functions and variables
    wsPolettiAsymNeg.asymNegGain = 1.7;
    wsPolettiAsymNeg.asymPosGain = 1.7;
    wsPolettiAsymNeg.asymNegLN = wsPolettiAsymNeg.asymPosLP = 23.6;
    wsPolettiAsymNeg.asymNegLP = wsPolettiAsymNeg.asymPosLN = 0.5;
    wsPolettiAsymNeg.symGain = 4.0;
    wsPolettiAsymNeg.symLPLN = 1.01;

    wsPolettiAsymNeg.functionToUse = [this](float x) {
        float y = 0;
        if (x <= 0)
        {
            y = (wsPolettiAsymNeg.asymNegGain * x) / (1.0 - ((wsPolettiAsymNeg.asymNegGain * x) / wsPolettiAsymNeg.asymNegLN));
        }
        else
        {
            y = (wsPolettiAsymNeg.asymNegGain * x) / (1.0 + ((wsPolettiAsymNeg.asymNegGain * x) / wsPolettiAsymNeg.asymNegLP));
        }
        return y;
    };
    wsPolettiAsymPos.functionToUse = [this](float x) {
        float y = 0;
        if (x <= 0)
        {
            y = (wsPolettiAsymPos.asymPosGain * x) / (1.0 - ((wsPolettiAsymPos.asymPosGain * x) / wsPolettiAsymPos.asymPosLN));
        }
        else
        {
            y = (wsPolettiAsymPos.asymPosGain * x) / (1.0 + ((wsPolettiAsymPos.asymPosGain * x) / wsPolettiAsymPos.asymPosLP));
        }
        return y;
        };
    wsPolettiSym.functionToUse = [this](float x) {
        float y = 0;
        if (x <= 0)
        {
            y = (wsPolettiSym.symGain * x) / (1.0 - ((wsPolettiSym.symGain * x) / wsPolettiSym.symLPLN));
        }
        else
        {
            y = (wsPolettiSym.symGain * x) / (1.0 + ((wsPolettiSym.symGain * x) / wsPolettiSym.symLPLN));
        }
        return y;
        };

}

AmpDrive::~AmpDrive()
{
}

void AmpDrive::reset()
{
    wsPolettiAsymNeg.reset();
    wsPolettiAsymPos.reset();
    wsPolettiSym.reset();
}

void AmpDrive::prepare(const juce::dsp::ProcessSpec& spec)
{
    wsPolettiAsymNeg.prepare(spec);
    wsPolettiAsymPos.prepare(spec);
    wsPolettiSym.prepare(spec);
    posBuffer.setSize(2, spec.maximumBlockSize);
    negBuffer.setSize(2, spec.maximumBlockSize);
    sumBuffer.setSize(2, spec.maximumBlockSize);
    RDCfilter.prepare(spec);
    RDCfilter.setCutoffFrequency(10);
    RDCfilter.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    LDCfilter.prepare(spec);
    LDCfilter.setCutoffFrequency(10);
    LDCfilter.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
}

//template <typename ProcessContext>
void AmpDrive::process(juce::dsp::AudioBlock<float>& block)
{
    block.copyTo(posBuffer);
    block.copyTo(negBuffer);
    block.copyTo(sumBuffer);
    juce::dsp::AudioBlock<float> posBlock(posBuffer), negBlock(negBuffer), sumBlock(sumBuffer);
    auto LposBlock = posBlock.getSingleChannelBlock(0);
    auto RposBlock = posBlock.getSingleChannelBlock(1);
    auto LnegBlock = negBlock.getSingleChannelBlock(0);
    auto RnegBlock = negBlock.getSingleChannelBlock(1);
    auto LsumBlock = sumBlock.getSingleChannelBlock(0);
    auto RsumBlock = sumBlock.getSingleChannelBlock(1);
    juce::dsp::ProcessContextReplacing<float> LcontextPos(LposBlock), RcontextPos(RposBlock), LcontextNeg(LnegBlock), RcontextNeg(RnegBlock), LcontextSum(LnegBlock), RcontextSum(RnegBlock);
    switch (simType) {
    case AmpTypeEnum::poletti:
        // asymetric waveshaping
        wsPolettiAsymNeg.process(LcontextNeg);
        wsPolettiAsymNeg.process(RcontextNeg);
        wsPolettiAsymPos.process(LcontextPos);
        wsPolettiAsymPos.process(RcontextPos);
        // removing DC offset
        LDCfilter.process(LcontextNeg);
        LDCfilter.reset();
        RDCfilter.process(RcontextNeg);
        RDCfilter.reset();
        LDCfilter.process(LcontextPos);
        LDCfilter.reset();
        RDCfilter.process(RcontextPos);
        RDCfilter.reset();
        // symetric waveshape
        wsPolettiSym.process(LcontextPos);
        wsPolettiSym.process(RcontextPos);
        wsPolettiSym.process(LcontextNeg);
        wsPolettiSym.process(RcontextNeg);
        // add together
        sumBlock.replaceWithSumOf(posBlock, negBlock);
        sumBlock.multiplyBy(0.5);
        // remove DC offset
        LDCfilter.process(LcontextSum);
        RDCfilter.process(RcontextSum);
        break;
    case AmpTypeEnum::placeholder:
        break;
    }
}

void AmpDrive::simTypeChanged(int simTypeID)
{
    simType = static_cast<AmpTypeEnum>(simTypeID);
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