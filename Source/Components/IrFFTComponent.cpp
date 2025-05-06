/*
  ==============================================================================

    IrFFTComponent.cpp
    Created: 25 Feb 2025 7:24:37pm
    Author:  knize

  ==============================================================================
*/

#include "IrFFTComponent.h"

IrFFTComponent::IrFFTComponent(BasicEQAudioProcessor& p) : audioProcessor(p),
leftPathProducer(audioProcessor.leftChannelFifo),
rightPathProducer(audioProcessor.rightChannelFifo)
{
    /*const auto& params = audioProcessor.getParameters();
    for (auto param : params)
    {
        param->addListener(this);
    }*/
}

IrFFTComponent::~IrFFTComponent()
{
    /*const auto& params = audioProcessor.getParameters();
    for (auto param : params)
    {
        param->removeListener(this);
    }*/
}


// when we change the IR in onChange lambdas of UI elements, we need to:
// call this function
// pass it the selected IR wav file
// convert wav to vector of floats - currently in AudioBuffer which is pretty much the same, have to see if it will go into FFT or not
// compute FFT
// store FFT data into a path
// call repaint
void IrFFTComponent::loadedIRChanged(juce::File newIR)
{
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    if (!newIR.existsAsFile()) { /*DBG("loadedIRChanged: loaded file is not a file");*/ return; }

    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(newIR));
    if (reader.get() == nullptr) { /*DBG("loadedIRChanged: nullptr in reader");*/ return; }

    leftPathProducer.leftChannelFFTDataGenerator.changeOrder(FFTOrder::order16384);

    auto fileSampleRate = reader->sampleRate;
    auto lengthInSamples = reader->lengthInSamples;
    auto fftBounds = getAnalysisArea().toFloat();
    const auto fftSize = leftPathProducer.leftChannelFFTDataGenerator.getFFTSize();
    const auto binWidth = fileSampleRate / (double)fftSize; // e.g. 48000 / 2048 = 23 Hz - frequency width of one fft bin, casting fftSize to double because sampleRate is double

    // 
    //juce::AudioBuffer<float> audioBuffer(reader->numChannels, lengthInSamples);
    //reader->read(&audioBuffer, 0, 8192, 0, true, true); // reader should return zeros if the file it reads is shorter than 4096 samples, this size must be 2 * FFT size

    // THIS IS FOR LEFT CH ONLY
    juce::AudioBuffer<float> audioBuffer(1, fftSize); // allocate buffer for 1 channel with how many samples are needed for FFT
    reader->read(&audioBuffer, 0, fftSize, 0, true, false);

    //fft.performFrequencyOnlyForwardTransform(audioBuffer.getWritePointer(Channel::Left), false);

    leftPathProducer.leftChannelFFTDataGenerator.produceFFTDataForRendering(audioBuffer, -130.f);

    // if there are FFT data buffers to pull, try to pull it and generate path from it
    // fftBounds is where it should draw the path


    while (leftPathProducer.leftChannelFFTDataGenerator.getNumAvailableFFTDataBlocks() > 0)
    {
        std::vector<float> fftData;
        if (leftPathProducer.leftChannelFFTDataGenerator.getFFTData(fftData))
        {
            leftPathProducer.pathProducer.generatePath(fftData, fftBounds, fftSize, binWidth, -90.f);
        }
    }

    // if there are paths that can be pulled, pull as many as possible, display the most recent one
    while (leftPathProducer.pathProducer.getNumPathsAvailable())
    {
        leftPathProducer.pathProducer.getPath(leftPathProducer.leftChannelFFTPath);
    }

    repaint();
}

// const reference since we dont need to change original objects
void IrFFTComponent::loadedIRChanged(const juce::AudioBuffer<float>& newIR, const int& sampleRate)
{
    //juce::AudioFormatManager formatManager;
    //formatManager.registerBasicFormats();

    //if (!newIR.existsAsFile()) { /*DBG("loadedIRChanged: loaded file is not a file");*/ return; }

    //std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(newIR));
    //if (reader.get() == nullptr) { /*DBG("loadedIRChanged: nullptr in reader");*/ return; }

    //leftPathProducer.leftChannelFFTDataGenerator.changeOrder(FFTOrder::order16384);

    //auto fileSampleRate = reader->sampleRate;
    //auto lengthInSamples = reader->lengthInSamples;
    auto fftBounds = getAnalysisArea().toFloat();
    const auto fftSize = leftPathProducer.leftChannelFFTDataGenerator.getFFTSize();
    const auto binWidth = sampleRate / (double)fftSize; // e.g. 48000 / 2048 = 23 Hz - frequency width of one fft bin, casting fftSize to double because sampleRate is double

    //// 
    ////juce::AudioBuffer<float> audioBuffer(reader->numChannels, lengthInSamples);
    ////reader->read(&audioBuffer, 0, 8192, 0, true, true); // reader should return zeros if the file it reads is shorter than 4096 samples, this size must be 2 * FFT size

    //// THIS IS FOR LEFT CH ONLY
    //juce::AudioBuffer<float> audioBuffer(1, fftSize); // allocate buffer for 1 channel with how many samples are needed for FFT
    //reader->read(&audioBuffer, 0, fftSize, 0, true, false);

    //fft.performFrequencyOnlyForwardTransform(audioBuffer.getWritePointer(Channel::Left), false);

    leftPathProducer.leftChannelFFTDataGenerator.produceFFTDataForRendering(newIR, -130.f);

    // if there are FFT data buffers to pull, try to pull it and generate path from it
    // fftBounds is where it should draw the path


    while (leftPathProducer.leftChannelFFTDataGenerator.getNumAvailableFFTDataBlocks() > 0)
    {
        std::vector<float> fftData;
        if (leftPathProducer.leftChannelFFTDataGenerator.getFFTData(fftData))
        {
            leftPathProducer.pathProducer.generatePath(fftData, fftBounds, fftSize, binWidth, -90.f);
        }
    }

    // if there are paths that can be pulled, pull as many as possible, display the most recent one
    while (leftPathProducer.pathProducer.getNumPathsAvailable())
    {
        leftPathProducer.pathProducer.getPath(leftPathProducer.leftChannelFFTPath);
    }

    repaint();
}

void IrFFTComponent::paint(juce::Graphics& g)
{
    using namespace juce;
    //g.fillAll(Colours::black);


    auto irArea = getLocalBounds();

    //g.setColour(juce::Colours::red);
    //g.drawRect(irArea);

    auto irAreaWidth = irArea.getWidth();

    g.drawImage(background, irArea.reduced(0,irAreaWidth*0.02).toFloat());


    // here we need to paint the path from FFT values
    auto leftChannelFFTPath = leftPathProducer.getPath();
    //auto rightChannelFFTPath = rightPathProducer.getPath();

    leftChannelFFTPath.applyTransform(AffineTransform().translation(irArea.getX(), irArea.getY() - 80));
    //rightChannelFFTPath.applyTransform(AffineTransform().translation(irArea.getX(), irArea.getY()));
    g.setColour(Colours::white);
    g.strokePath(leftChannelFFTPath, PathStrokeType(1.f));

    g.setColour(Colour::fromString("FFF09500"));
    //g.setColour(Colours::green);
    g.drawRoundedRectangle(irArea.reduced(irAreaWidth*0.005).toFloat(), irAreaWidth * 0.061, irAreaWidth * 0.015); // 32 cornersize, 8 linethickness when max size

    //g.setColour(Colours::red);
    //g.drawRect(irArea);

    //g.setColour(Colours::blue);
    //g.drawRect(getAnalysisArea());

    //g.setColour(Colours::aqua);
    //g.strokePath(rightChannelFFTPath, PathStrokeType(1.f));
}

void IrFFTComponent::resized()
{
    // drawing frequency grid behind IR graph
    using namespace juce;
    background = Image(Image::PixelFormat::ARGB, getWidth(), getHeight(), true);

    Graphics g(background);

    g.setColour(Colours::transparentWhite);
    g.setOpacity(0.0);
    g.fillAll();

    Array<float> freqsDim
    {
        30, 40, 50, 60, 70, 80, 90,
        200, 300, 400, 500, 600, 700, 800, 900,
        2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000,
        20000
    };
    Array<float> freqsLight{ 100, 1000, 10000 };

    for (auto f : freqsDim)
    {
        g.setColour(Colour::fromRGB(60, 60, 60));
        auto normX = mapFromLog10(f, 20.f, 20000.f);
        g.drawVerticalLine(getWidth() * normX, 0.f, getHeight());
    }

    g.setColour(Colour::fromRGB(120, 120, 120));
    for (auto f : freqsLight)
    {
        auto normX = mapFromLog10(f, 20.f, 20000.f);
        g.drawVerticalLine(getWidth() * normX, 0.f, getHeight());
    }

    //repaint();
    /*auto leftChannelFFTPath = leftPathProducer.getPath();
    leftChannelFFTPath.applyTransform(AffineTransform().translation(getLocalBounds().getX(), getLocalBounds().getY() - 80));
    g.setColour(Colours::white);
    g.strokePath(leftChannelFFTPath, PathStrokeType(1.f));*/
}

juce::Rectangle<int> IrFFTComponent::getRenderArea()
{
    auto bounds = getLocalBounds();

    bounds.removeFromTop(bounds.getHeight() * 0.1);
    bounds.removeFromBottom(bounds.getHeight()*0.08);
    //bounds.removeFromLeft(0);
    //bounds.removeFromRight(0);

    return bounds;
}


juce::Rectangle<int> IrFFTComponent::getAnalysisArea()
{
    auto bounds = getRenderArea();
    bounds.removeFromTop(4);
    bounds.removeFromBottom(4);
   /* bounds.removeFromBottom(bounds.getHeight()*0.1);
    bounds.removeFromRight(bounds.getWidth() * 0.0187);*/
    return bounds;
}