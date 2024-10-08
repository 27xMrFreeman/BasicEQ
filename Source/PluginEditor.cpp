/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

void LookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)
{
    using namespace juce;

    auto bounds = Rectangle<float>(x, y, width, height);

    /*g.setColour(Colour(72u, 30u, 20u));
    g.fillEllipse(bounds);

    g.setColour(Colour(242u, 97u, 63u));
    g.drawEllipse(bounds, 1.5);*/

    if( auto* rswl = dynamic_cast<RotarySliderWithLabels*>(&slider))
    {
        auto center = bounds.getCentre();
        //whatever we want to rotate needs to be in a path
        Path p;

        Rectangle<float> r;
        r.setLeft(center.getX() - 2);   // left side of rectangle 2 pixels left of center
        r.setRight(center.getX() + 2);  // right side of rectangle 2 pixels right of center
        r.setTop(bounds.getY());        // top of rectangle = top of bounds
        r.setBottom(center.getY()-rswl->getTextHeight()*1.5);     // bottom of rectangle = text height above center

        p.addRoundedRectangle(r, 2.f);
        
        jassert(rotaryStartAngle < rotaryEndAngle); // check if start angle is smaller than end angle

        auto sliderAngRad = jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle); // mapping normalised slider value to angles

        p.applyTransform(AffineTransform().rotated(sliderAngRad, center.getX(), center.getY())); // transform rotates path by sliderAngRad with x and y of pivot point

        //g.fillPath(p);

        // TRYING TO PUT IMAGE HERE
        juce::Image knobRed = ImageCache::getFromMemory(BinaryData::knob_red_png, BinaryData::knob_red_pngSize);
        juce::Image knobRedRescaled = knobRed.rescaled(bounds.getWidth() / knobRed.getWidth() * knobRed.getWidth(), bounds.getHeight() / knobRed.getHeight() * knobRed.getHeight(), Graphics::highResamplingQuality);
        
        AffineTransform rotator;
        //if (!slider.isMouseOverOrDragging())
        //{
        //    g.drawImage(knobRed, bounds, RectanglePlacement::stretchToFit, false);
        //}
        //else
        //{
        //    //g.drawImage(knobRed, x, y, width, height, 0, 0, width, height, false);
        //    g.drawImage(knobRed, bounds, RectanglePlacement::stretchToFit, false);
        //}
        //g.drawImageTransformed(knobRed, x, y, width, height, rotator.rotated((float)sliderPosProportional * rotaryEndAngle, (float)(knobRed.getWidth() / 2), (float)(knobRed.getHeight() / 2)), false);
        //g.drawImage(knobRedRescaled, bounds, RectanglePlacement::stretchToFit, false);
        int origX = g.getClipBounds().getX();
        int origY = g.getClipBounds().getY();
        g.drawImageTransformed(knobRedRescaled, rotator.rotated(sliderAngRad, knobRedRescaled.getWidth() / 2, knobRedRescaled.getHeight() / 2).translated(bounds.getX()-origX, bounds.getY()-origY));
        

        // TRYING TO PUT IMAGE HERE


        if (slider.isMouseOverOrDragging())
        {
            g.setFont(rswl->getTextHeight());                           // sets basic font with set height
            auto text = rswl->getDisplayString();                       // gets text to put in
            auto strWidth = g.getCurrentFont().getStringWidth(text);    // gets width of text

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
    auto size = jmin(bounds.getWidth(), bounds.getHeight())-5;
    auto r = bounds.withSizeKeepingCentre(size, size).toFloat();

    float ang = 30.f;

    size -= 6;

    powerButton.addCentredArc(r.getCentreX(), r.getCentreY(), size * 0.5, size * 0.5, 0.f, degreesToRadians(ang), degreesToRadians(360.f - ang), true);

    powerButton.startNewSubPath(r.getCentreX(), r.getY()+3);
    powerButton.lineTo(r.getCentre());

    PathStrokeType pst(2.f, PathStrokeType::JointStyle::curved);

    auto color = toggleButton.getToggleState() ? Colours::dimgrey : Colours::lightgreen;
    
    g.setColour(color);
    g.strokePath(powerButton, pst);
    size = jmin(bounds.getWidth(), bounds.getHeight())-3;
    r = bounds.withSizeKeepingCentre(size, size).toFloat();
    g.setColour(Colours::silver);
    g.drawEllipse(r, 2);

}

void LookAndFeelBlue::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)
{
    using namespace juce;

    auto bounds = Rectangle<float>(x, y, width, height);

    /*g.setColour(Colour(72u, 30u, 20u));
    g.fillEllipse(bounds);

    g.setColour(Colour(242u, 97u, 63u));
    g.drawEllipse(bounds, 1.5);*/

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

        //g.fillPath(p);

        // TRYING TO PUT IMAGE HERE
        juce::Image knobRed = ImageCache::getFromMemory(BinaryData::knob_blue_png, BinaryData::knob_blue_pngSize);
        juce::Image knobRedRescaled = knobRed.rescaled(bounds.getWidth() / knobRed.getWidth() * knobRed.getWidth(), bounds.getHeight() / knobRed.getHeight() * knobRed.getHeight(), Graphics::highResamplingQuality);

        AffineTransform rotator;
        //if (!slider.isMouseOverOrDragging())
        //{
        //    g.drawImage(knobRed, bounds, RectanglePlacement::stretchToFit, false);
        //}
        //else
        //{
        //    //g.drawImage(knobRed, x, y, width, height, 0, 0, width, height, false);
        //    g.drawImage(knobRed, bounds, RectanglePlacement::stretchToFit, false);
        //}
        //g.drawImageTransformed(knobRed, x, y, width, height, rotator.rotated((float)sliderPosProportional * rotaryEndAngle, (float)(knobRed.getWidth() / 2), (float)(knobRed.getHeight() / 2)), false);
        //g.drawImage(knobRedRescaled, bounds, RectanglePlacement::stretchToFit, false);
        int origX = g.getClipBounds().getX();
        int origY = g.getClipBounds().getY();
        g.drawImageTransformed(knobRedRescaled, rotator.rotated(sliderAngRad, knobRedRescaled.getWidth() / 2, knobRedRescaled.getHeight() / 2).translated(bounds.getX() - origX, bounds.getY() - origY));


        // TRYING TO PUT IMAGE HERE


        if (slider.isMouseOverOrDragging())
        {
            g.setFont(rswl->getTextHeight());                           // sets basic font with set height
            auto text = rswl->getDisplayString();                       // gets text to put in
            auto strWidth = g.getCurrentFont().getStringWidth(text);    // gets width of text

            r.setSize(strWidth + 4, rswl->getTextHeight() + 2);         // rectangle r is little bigger than the text
            r.setCentre(bounds.getCentre());                            // set centre of the rectangle to centre of bounds (slider)

            g.setColour(Colours::black);
            g.fillRect(r);

            g.setColour(Colours::white);
            g.drawFittedText(text, r.toNearestInt(), juce::Justification::centred, 1);
        }
    }
}

void LookAndFeelGreen::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)
{
    using namespace juce;

    auto bounds = Rectangle<float>(x, y, width, height);

    /*g.setColour(Colour(72u, 30u, 20u));
    g.fillEllipse(bounds);

    g.setColour(Colour(242u, 97u, 63u));
    g.drawEllipse(bounds, 1.5);*/

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

        //g.fillPath(p);

        // TRYING TO PUT IMAGE HERE
        juce::Image knobRed = ImageCache::getFromMemory(BinaryData::knob_green_png, BinaryData::knob_green_pngSize);
        juce::Image knobRedRescaled = knobRed.rescaled(bounds.getWidth() / knobRed.getWidth() * knobRed.getWidth(), bounds.getHeight() / knobRed.getHeight() * knobRed.getHeight(), Graphics::highResamplingQuality);

        AffineTransform rotator;
        //if (!slider.isMouseOverOrDragging())
        //{
        //    g.drawImage(knobRed, bounds, RectanglePlacement::stretchToFit, false);
        //}
        //else
        //{
        //    //g.drawImage(knobRed, x, y, width, height, 0, 0, width, height, false);
        //    g.drawImage(knobRed, bounds, RectanglePlacement::stretchToFit, false);
        //}
        //g.drawImageTransformed(knobRed, x, y, width, height, rotator.rotated((float)sliderPosProportional * rotaryEndAngle, (float)(knobRed.getWidth() / 2), (float)(knobRed.getHeight() / 2)), false);
        //g.drawImage(knobRedRescaled, bounds, RectanglePlacement::stretchToFit, false);
        int origX = g.getClipBounds().getX();
        int origY = g.getClipBounds().getY();
        g.drawImageTransformed(knobRedRescaled, rotator.rotated(sliderAngRad, knobRedRescaled.getWidth() / 2, knobRedRescaled.getHeight() / 2).translated(bounds.getX() - origX, bounds.getY() - origY));


        // TRYING TO PUT IMAGE HERE


        if (slider.isMouseOverOrDragging())
        {
            g.setFont(rswl->getTextHeight());                           // sets basic font with set height
            auto text = rswl->getDisplayString();                       // gets text to put in
            auto strWidth = g.getCurrentFont().getStringWidth(text);    // gets width of text

            r.setSize(strWidth + 4, rswl->getTextHeight() + 2);         // rectangle r is little bigger than the text
            r.setCentre(bounds.getCentre());                            // set centre of the rectangle to centre of bounds (slider)

            g.setColour(Colours::black);
            g.fillRect(r);

            g.setColour(Colours::white);
            g.drawFittedText(text, r.toNearestInt(), juce::Justification::centred, 1);
        }
    }
}

void LookAndFeelBlack::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)
{
    using namespace juce;

    auto bounds = Rectangle<float>(x, y, width, height);

    /*g.setColour(Colour(72u, 30u, 20u));
    g.fillEllipse(bounds);

    g.setColour(Colour(242u, 97u, 63u));
    g.drawEllipse(bounds, 1.5);*/

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

        //g.fillPath(p);

        // TRYING TO PUT IMAGE HERE
        juce::Image knobRed = ImageCache::getFromMemory(BinaryData::knob_black_png, BinaryData::knob_black_pngSize);
        juce::Image knobRedRescaled = knobRed.rescaled(bounds.getWidth() / knobRed.getWidth() * knobRed.getWidth(), bounds.getHeight() / knobRed.getHeight() * knobRed.getHeight(), Graphics::highResamplingQuality);

        AffineTransform rotator;
        //if (!slider.isMouseOverOrDragging())
        //{
        //    g.drawImage(knobRed, bounds, RectanglePlacement::stretchToFit, false);
        //}
        //else
        //{
        //    //g.drawImage(knobRed, x, y, width, height, 0, 0, width, height, false);
        //    g.drawImage(knobRed, bounds, RectanglePlacement::stretchToFit, false);
        //}
        //g.drawImageTransformed(knobRed, x, y, width, height, rotator.rotated((float)sliderPosProportional * rotaryEndAngle, (float)(knobRed.getWidth() / 2), (float)(knobRed.getHeight() / 2)), false);
        //g.drawImage(knobRedRescaled, bounds, RectanglePlacement::stretchToFit, false);
        int origX = g.getClipBounds().getX();
        int origY = g.getClipBounds().getY();
        g.drawImageTransformed(knobRedRescaled, rotator.rotated(sliderAngRad, knobRedRescaled.getWidth() / 2, knobRedRescaled.getHeight() / 2).translated(bounds.getX() - origX, bounds.getY() - origY));


        // TRYING TO PUT IMAGE HERE


        if (slider.isMouseOverOrDragging())
        {
            g.setFont(rswl->getTextHeight());                           // sets basic font with set height
            auto text = rswl->getDisplayString();                       // gets text to put in
            auto strWidth = g.getCurrentFont().getStringWidth(text);    // gets width of text

            r.setSize(strWidth + 4, rswl->getTextHeight() + 2);         // rectangle r is little bigger than the text
            r.setCentre(bounds.getCentre());                            // set centre of the rectangle to centre of bounds (slider)

            g.setColour(Colours::black);
            g.fillRect(r);

            g.setColour(Colours::white);
            g.drawFittedText(text, r.toNearestInt(), juce::Justification::centred, 1);
        }
    }
}
//==============================================================================

void RotarySliderWithLabels::paint(juce::Graphics& g)
{
    using namespace juce;

    auto startAng = degreesToRadians(180.f + 35.f);     // angle starts at 12�, 180� = 6�
    auto endAng = degreesToRadians(180.f - 35.f) + MathConstants<float>::twoPi; // adding 2pi to ensure startAng < endAng

    auto range = getRange();

    auto sliderBounds = getSliderBounds();

    /*g.setColour(Colours::red);
    g.drawRect(getLocalBounds());
    g.setColour(Colours::azure);
    g.drawRect(sliderBounds); */        // bounding boxes of sliders for debugging purposes

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

    auto center = sliderBounds.toFloat().getCentre();
    auto radius = sliderBounds.getWidth()*0.5f;

    g.setColour(Colours::white);
    g.setFont(getTextHeight());

    auto numChoices = labels.size();
    for (int i = 0; i < numChoices; ++i)
    {
        auto pos = labels[i].pos;
        jassert(0.f <= pos);
        jassert(pos <= 1.f);        // make sure position is between 0 and 1

        auto ang = jmap(pos, 0.f, 1.f, startAng, endAng); // mapped to angles
        // text needs to be a bit out from the slider
        auto c = center.getPointOnCircumference(radius + getTextHeight() * 0.5f, ang);          // this gets the centre of the text a bit out from the circle

        Rectangle<float> r; // rectangle for bounding box of text
        auto str = labels[i].label; // get text from labels
        r.setSize(g.getCurrentFont().getStringWidth(str), getTextHeight());     // set size of rectangle depending on font width and height
        r.setCentre(c);     // set centre of rectangle to c -> text could be touching circle
        r.setY(r.getY() + getTextHeight());         // move rectangle down, so that text doesnt touch the circle

        g.drawFittedText(str, r.toNearestInt(), juce::Justification::centred, 1);
    }
}

juce::Rectangle<int> RotarySliderWithLabels::getSliderBounds() const
{
    //return getLocalBounds();
    auto bounds = getLocalBounds();

    auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());

    size -= getTextHeight() * 2;
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
        // this whole block is for truncating Hz -> kHz
        float val = getValue(); 
        if (val > 999.f)
        {
            val /= 1000.f;
            addK = true;
        }
        str = juce::String(val, (addK ? 2 : 0)); // if addK is true, limit to 2 decimal places, otherwise use default value
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
//==============================================================================


ResponseCurveComponent::ResponseCurveComponent(BasicEQAudioProcessor& p) : audioProcessor(p), //leftChannelFifo(&audioProcessor.leftChannelFifo)
leftPathProducer(audioProcessor.leftChannelFifo),
rightPathProducer(audioProcessor.rightChannelFifo)
{
    const auto& params = audioProcessor.getParameters();
    for (auto param : params)
    {
        param->addListener(this);
    }

    updateChain();

    startTimerHz(60);
}

ResponseCurveComponent::~ResponseCurveComponent()
{
    const auto& params = audioProcessor.getParameters();
    for (auto param : params)
    {
        param->removeListener(this);
    }
}

void ResponseCurveComponent::parameterValueChanged(int parameterIndex, float newValue)
{
    //DBG("param changed" << parameterIndex);
    parametersChanged.set(true);
}

void PathProducer::process(juce::Rectangle<float> fftBounds, double sampleRate)
{
    juce::AudioBuffer<float> tempIncomingBuffer;
    // if there is a buffer available in the FIFO, send it to FFT data generator
    while (leftChannelFifo->getNumCompleteBuffersAvailable() > 0)
    {
        if (leftChannelFifo->getAudioBuffer(tempIncomingBuffer))    // if we can get the buffer
        {
            auto size = tempIncomingBuffer.getNumSamples();
            // this shifts stuff in the buffer by how big the sent out buffer is
            juce::FloatVectorOperations::copy(monoBuffer.getWritePointer(0, 0),   // where to copy - to the start of the buffer
                monoBuffer.getReadPointer(0, size), // source to copy - starts where the previous buffer block ended
                monoBuffer.getNumSamples() - size); // how many values to copy - all values except the ones in the previous buffer block

            juce::FloatVectorOperations::copy(monoBuffer.getWritePointer(0, monoBuffer.getNumSamples() - size),   // copy to the end of the buffer
                tempIncomingBuffer.getReadPointer(0, 0),                            // copy from start of incoming buffer
                size);                                                              // copy as many values as are in incoming buffer
            // block above effectively shifts audio in monoBuffer left by the block size, appending new data from tempIncomingBuffer at the end of monoBuffer //

            leftChannelFFTDataGenerator.produceFFTDataForRendering(monoBuffer, -92.f); // pass monoBuffer to the FFT, negativeInfinity set to -48dB (what level of audio will be the lowest)
        }
    }

    // if there are FFT data buffers to pull, try to pull it and generate path from it
    // fftBounds is where it should draw the path
    const auto fftSize = leftChannelFFTDataGenerator.getFFTSize();
    const auto binWidth = sampleRate / (double)fftSize; // e.g. 48000 / 2048 = 23 Hz - frequency width of one fft bin, casting fftSize to double because sampleRate is double

    while (leftChannelFFTDataGenerator.getNumAvailableFFTDataBlocks() > 0)
    {
        std::vector<float> fftData;
        if (leftChannelFFTDataGenerator.getFFTData(fftData))
        {
            pathProducer.generatePath(fftData, fftBounds, fftSize, binWidth, -92.f);
        }
    }

    // if there are paths that can be pulled, pull as many as possible, display the most recent one
    while (pathProducer.getNumPathsAvailable())
    {
        pathProducer.getPath(leftChannelFFTPath);
    }
}

void ResponseCurveComponent::timerCallback()
{
    auto fftBounds = getAnalysisArea().toFloat();
    auto sampleRate = audioProcessor.getSampleRate();

    leftPathProducer.process(fftBounds, sampleRate);
    rightPathProducer.process(fftBounds, sampleRate);

    if (parametersChanged.compareAndSetBool(false, true))
    {
        updateChain();
        //signal a repaint
        //repaint();
    }
    // need to be repainting all the time, not only when a parameter changes
    repaint();
}

void ResponseCurveComponent::updateChain()
{
    //update mono chain
    auto chainSettings = getChainSettings(audioProcessor.apvts);

    monoChain.setBypassed<ChainPositions::LowCut>(chainSettings.lowCutBypassed);
    monoChain.setBypassed<ChainPositions::HighCut>(chainSettings.highCutBypassed);
    monoChain.setBypassed<ChainPositions::Peak>(chainSettings.peakBypassed);

    auto peakCoefficients = makePeakFilter(chainSettings, audioProcessor.getSampleRate());
    updateCoefficients(monoChain.get<ChainPositions::Peak>().coefficients, peakCoefficients);

    auto lowCutCoefficients = makeLowCutFilter(chainSettings, audioProcessor.getSampleRate());
    auto highCutCoefficients = makeHighCutFilter(chainSettings, audioProcessor.getSampleRate());

    updateCutFilter(monoChain.get<ChainPositions::LowCut>(), lowCutCoefficients, chainSettings.lowCutSlope);
    updateCutFilter(monoChain.get<ChainPositions::HighCut>(), highCutCoefficients, chainSettings.highCutSlope);
}

void ResponseCurveComponent::parameterGestureChanged(int parameterIndex, bool gestureIsStarting) {  };

void ResponseCurveComponent::paint(juce::Graphics& g)
{
    using namespace juce;
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(Colours::black);

    g.drawImage(background, getLocalBounds().toFloat());

    auto responseArea = getLocalBounds();

    auto w = responseArea.getWidth();

    auto& lowcut = monoChain.get<ChainPositions::LowCut>();
    auto& peak = monoChain.get<ChainPositions::Peak>();
    auto& highcut = monoChain.get<ChainPositions::HighCut>();

    auto sampleRate = audioProcessor.getSampleRate();

    std::vector<double> mags;

    mags.resize(w);

    for (int i = 0; i < w; ++i)
    {
        double mag = 1.f;
        auto freq = mapToLog10(double(i) / double(w), 20.0, 20000.0);

        if (!monoChain.isBypassed<ChainPositions::Peak>())
            mag *= peak.coefficients->getMagnitudeForFrequency(freq, sampleRate);

        if (!monoChain.isBypassed<ChainPositions::LowCut>())
        {
            if (!lowcut.isBypassed<0>())
                mag *= lowcut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            if (!lowcut.isBypassed<1>())
                mag *= lowcut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            if (!lowcut.isBypassed<2>())
                mag *= lowcut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            if (!lowcut.isBypassed<3>())
                mag *= lowcut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        }
        if (!monoChain.isBypassed<ChainPositions::HighCut>())
        {
            if (!highcut.isBypassed<0>())
                mag *= highcut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            if (!highcut.isBypassed<1>())
                mag *= highcut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            if (!highcut.isBypassed<2>())
                mag *= highcut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
            if (!highcut.isBypassed<3>())
                mag *= highcut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        }
        mags[i] = Decibels::gainToDecibels(mag);
    }

    Path filterResponseCurve;

    const double outputMin = responseArea.getBottom()-10;
    const double outputMax = responseArea.getY();
    auto map = [outputMin, outputMax](double input)
        {
            return jmap(input, -24.0, 24.0, outputMin, outputMax);
        };

    filterResponseCurve.startNewSubPath(responseArea.getX(), map(mags.front()));

    for (size_t i = 1; i < mags.size(); ++i)
    {
        filterResponseCurve.lineTo(responseArea.getX() + i, map(mags[i]));
    }

    auto leftChannelFFTPath = leftPathProducer.getPath();
    auto rightChannelFFTPath = rightPathProducer.getPath();

    leftChannelFFTPath.applyTransform(AffineTransform().translation(responseArea.getX(), responseArea.getY()));
    rightChannelFFTPath.applyTransform(AffineTransform().translation(responseArea.getX(), responseArea.getY()));
    g.setColour(Colours::lawngreen);
    g.strokePath(leftChannelFFTPath, PathStrokeType(1.f));

    g.setColour(Colours::orangered);
    g.strokePath(rightChannelFFTPath, PathStrokeType(1.f));

    g.setColour(Colours::white);
    g.strokePath(filterResponseCurve, PathStrokeType(2.f));
    
    g.setColour(Colours::silver);
    g.drawRoundedRectangle(responseArea.toFloat(), 6.f, 5.f);
}

void ResponseCurveComponent::resized()
{

    // drawing frequency grid behind EQ response graph
    using namespace juce;
    background = Image(Image::PixelFormat::RGB, getWidth(), getHeight(), true);

    Graphics g(background);

    Array<float> freqsDim
    {
        20, 30, 40, 50, 60, 70, 80, 90,
        200, 300, 400, 500, 600, 700, 800, 900,
        2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000,
        20000
    };
    Array<float> freqsLight{ 100, 1000, 10000 };

    for (auto f : freqsDim)
    {
        g.setColour(Colour::fromRGB(60,60,60));
        auto normX = mapFromLog10(f, 20.f, 20000.f);
        g.drawVerticalLine(getWidth() * normX, 0.f, getHeight());
    }

    g.setColour(Colour::fromRGB(120, 120, 120));
    for (auto f : freqsLight)
    {
        auto normX = mapFromLog10(f, 20.f, 20000.f);
        g.drawVerticalLine(getWidth() * normX, 0.f, getHeight());
    }

}

juce::Rectangle<int> ResponseCurveComponent::getRenderArea()
{
    auto bounds = getLocalBounds();

    bounds.removeFromTop(0);
    bounds.removeFromBottom(0);
    bounds.removeFromLeft(0);
    bounds.removeFromRight(0);

    return bounds;
}


juce::Rectangle<int> ResponseCurveComponent::getAnalysisArea()
{
    auto bounds = getRenderArea();
    bounds.removeFromTop(4);
    bounds.removeFromBottom(4);
    return bounds;
}

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

    leftPathProducer.leftChannelFFTDataGenerator.produceFFTDataForRendering(audioBuffer, - 130.f);

    // if there are FFT data buffers to pull, try to pull it and generate path from it
    // fftBounds is where it should draw the path
    

    while (leftPathProducer.leftChannelFFTDataGenerator.getNumAvailableFFTDataBlocks() > 0)
    {
        std::vector<float> fftData;
        if (leftPathProducer.leftChannelFFTDataGenerator.getFFTData(fftData))
        {
            leftPathProducer.pathProducer.generatePath(fftData, fftBounds, fftSize, binWidth, - 90.f);
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
    g.fillAll(Colours::black);

    g.drawImage(background, getLocalBounds().toFloat());

    auto irArea = getLocalBounds();

   

    // here we need to paint the path from FFT values
    auto leftChannelFFTPath = leftPathProducer.getPath();
    //auto rightChannelFFTPath = rightPathProducer.getPath();

    leftChannelFFTPath.applyTransform(AffineTransform().translation(irArea.getX(), irArea.getY()-80));
    //rightChannelFFTPath.applyTransform(AffineTransform().translation(irArea.getX(), irArea.getY()));
    g.setColour(Colours::white);
    g.strokePath(leftChannelFFTPath, PathStrokeType(1.f));

    g.setColour(Colours::silver);
    g.drawRoundedRectangle(irArea.toFloat(), 6.f, 5.f);

    //g.setColour(Colours::aqua);
    //g.strokePath(rightChannelFFTPath, PathStrokeType(1.f));
}

void IrFFTComponent::resized()
{
    // drawing frequency grid behind IR graph
    using namespace juce;
    background = Image(Image::PixelFormat::RGB, getWidth(), getHeight(), true);

    Graphics g(background);

    Array<float> freqsDim
    {
        20, 30, 40, 50, 60, 70, 80, 90,
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
}

juce::Rectangle<int> IrFFTComponent::getRenderArea()
{
    auto bounds = getLocalBounds();

    bounds.removeFromTop(0);
    bounds.removeFromBottom(0);
    bounds.removeFromLeft(0);
    bounds.removeFromRight(0);

    return bounds;
}


juce::Rectangle<int> IrFFTComponent::getAnalysisArea()
{
    auto bounds = getRenderArea();
    bounds.removeFromTop(4);
    bounds.removeFromBottom(4);
    return bounds;
}

//==============================================================================
BasicEQAudioProcessorEditor::BasicEQAudioProcessorEditor(BasicEQAudioProcessor& p) : AudioProcessorEditor(&p), audioProcessor(p),
peakFreqSlider(*audioProcessor.apvts.getParameter("Peak Freq"), "Hz"),
peakGainSlider(*audioProcessor.apvts.getParameter("Peak Gain"), "dB"),
peakQualitySlider(*audioProcessor.apvts.getParameter("Peak Q"), ""),
lowCutFreqSlider(*audioProcessor.apvts.getParameter("LowCut Freq"), "Hz"),
lowCutSlopeSlider(*audioProcessor.apvts.getParameter("LowCut Slope"), "dB/Oct"),
highCutFreqSlider(*audioProcessor.apvts.getParameter("HighCut Freq"), "Hz"),
highCutSlopeSlider(*audioProcessor.apvts.getParameter("HighCut Slope"), "dB/Oct"),
xPosSlider(*audioProcessor.apvts.getParameter("X Position"), "cm"),
yPosSlider(*audioProcessor.apvts.getParameter("Y Position"), "cm"),
outputGainSlider(*audioProcessor.apvts.getParameter("Output Gain"), "dB"),
responseCurveComponent(audioProcessor),
irfftComponent(audioProcessor),
peakFreqSliderAttachment(audioProcessor.apvts, "Peak Freq", peakFreqSlider),
peakGainSliderAttachment(audioProcessor.apvts, "Peak Gain", peakGainSlider),
peakQualitySliderAttachment(audioProcessor.apvts, "Peak Q", peakQualitySlider),
lowCutFreqSliderAttachment(audioProcessor.apvts, "LowCut Freq", lowCutFreqSlider),
lowCutSlopeSliderAttachment(audioProcessor.apvts, "LowCut Slope", lowCutSlopeSlider),
highCutFreqSliderAttachment(audioProcessor.apvts, "HighCut Freq", highCutFreqSlider),
highCutSlopeSliderAttachment(audioProcessor.apvts, "HighCut Slope", highCutSlopeSlider),
xPosSliderAttachment(audioProcessor.apvts, "X Position", xPosSlider),
yPosSliderAttachment(audioProcessor.apvts, "Y Position", yPosSlider),
outputGainSliderAttachment(audioProcessor.apvts, "Output Gain", outputGainSlider),
lowCutBypassButtonAttachment(audioProcessor.apvts, "LowCut Bypassed", lowCutBypassButton),
highCutBypassButtonAttachment(audioProcessor.apvts, "HighCut Bypassed", highCutBypassButton),
peakBypassButtonAttachment(audioProcessor.apvts, "Peak Bypassed", peakBypassButton),
irBypassButtonAttachment(audioProcessor.apvts, "IR Bypassed", irBypassButton)

{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    peakFreqSlider.labels.add({ 0.f, "10Hz" });
    peakFreqSlider.labels.add({ 1.f, "20kHz" });
    peakGainSlider.labels.add({ 0.f, "-24dB" });
    peakGainSlider.labels.add({ 1.f, "24dB" });
    peakQualitySlider.labels.add({ 0.f, "0.1" });
    peakQualitySlider.labels.add({ 1.f, "10" });
    lowCutFreqSlider.labels.add({ 0.f, "10Hz" });
    lowCutFreqSlider.labels.add({ 1.f, "20kHz" });
    lowCutSlopeSlider.labels.add({ 0.f, "12" });
    lowCutSlopeSlider.labels.add({ 1.f, "48" });
    highCutFreqSlider.labels.add({ 0.f, "10Hz" });
    highCutFreqSlider.labels.add({ 1.f, "20kHz" });
    highCutSlopeSlider.labels.add({ 0.f, "12" });
    highCutSlopeSlider.labels.add({ 1.f, "48" });
    xPosSlider.labels.add({ 0.f, "0cm" });
    xPosSlider.labels.add({ 1.f, "8cm" });
    yPosSlider.labels.add({ 0.f, "0cm" });
    yPosSlider.labels.add({ 1.f, "40cm" });

    for (auto* comp : getComps())
    {
        addAndMakeVisible(comp);
    }

    lowCutBypassButton.setLookAndFeel(&lnf);
    highCutBypassButton.setLookAndFeel(&lnf);
    peakBypassButton.setLookAndFeel(&lnf);
    irBypassButton.setLookAndFeel(&lnf);
    lowCutFreqSlider.setLookAndFeel(&lnfb);
    lowCutSlopeSlider.setLookAndFeel(&lnfb);
    highCutFreqSlider.setLookAndFeel(&lnf);
    highCutSlopeSlider.setLookAndFeel(&lnf);
    peakFreqSlider.setLookAndFeel(&lnfg);
    peakGainSlider.setLookAndFeel(&lnfg);
    peakQualitySlider.setLookAndFeel(&lnfg);
    xPosSlider.setLookAndFeel(&lnfk);
    yPosSlider.setLookAndFeel(&lnfk);
    outputGainSlider.setLookAndFeel(&lnfk);

    comboTypeBox.addItem("Mar", 1);
    comboTypeBox.addItem("MM", 2);
    comboTypeBox.addItem("SV", 3);
    comboTypeBox.setSelectedId(1);
    comboTypeBox.onChange = [this]() { 
        //DBG("changed combo"); 
        juce::File newIR = audioProcessor.updateLoadedIR(comboTypeBox.getSelectedId() - 1, mikTypeBox.getSelectedId() - 1, yPosSlider.getValue(), xPosSlider.getValue()); 
        userIRLoaded = false; 
        irfftComponent.loadedIRChanged(newIR);
        };

    mikTypeBox.addItem("57A", 1);
    mikTypeBox.addItem("kalib", 2);
    mikTypeBox.addItem("sm57", 3);
    mikTypeBox.setSelectedId(1);
    mikTypeBox.onChange = [this]() { 
        //DBG("changed mic"); 
        juce::File newIR = audioProcessor.updateLoadedIR(comboTypeBox.getSelectedId()-1, mikTypeBox.getSelectedId()-1, yPosSlider.getValue(), xPosSlider.getValue());
        userIRLoaded = false;
        irfftComponent.loadedIRChanged(newIR);
        };

    yPosSlider.onValueChange = [this]() { 
        //DBG("changed yPos to " << yPosSlider.getValue());
        juce::File newIR = audioProcessor.updateLoadedIR(comboTypeBox.getSelectedId() - 1, mikTypeBox.getSelectedId() - 1, yPosSlider.getValue(), xPosSlider.getValue());
        userIRLoaded = false;
        irfftComponent.loadedIRChanged(newIR);
        };
    xPosSlider.onValueChange = [this]() { 
        //DBG("changed xPos to " << xPosSlider.getValue());
        juce::File newIR = audioProcessor.updateLoadedIR(comboTypeBox.getSelectedId() - 1, mikTypeBox.getSelectedId() - 1, yPosSlider.getValue(), xPosSlider.getValue());
        userIRLoaded = false;
        irfftComponent.loadedIRChanged(newIR);
        };

    loadBtn.setButtonText("Load IR");
    loadBtn.onClick = [this]()
    {
            fileChooser = std::make_unique<juce::FileChooser>("Choose Impulse Response", audioProcessor.root, "*wav", true); // declare file window, at root directory, only .wav files allowed
            // set file chooser flags
            const auto fileChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::canSelectDirectories;
            // open window with set flags
            fileChooser->launchAsync(fileChooserFlags,
                [this](const juce::FileChooser& chooser)
                {
                    // save chosen file
                    juce::File result(chooser.getResult());
                    if (!result.existsAsFile()) { /*DBG("pressed cancel");*/ return; }
                    audioProcessor.savedFile = result;
                    audioProcessor.root = result.getParentDirectory().getFullPathName();    // set root directory to where the file was selected from
                    irNameLabel.setText( result.getFileNameWithoutExtension(), juce::dontSendNotification );
                    audioProcessor.irLoader.reset();
                    // load IR, stereo, trimmed, normalized, size 0 = original IR size
                    audioProcessor.irLoader.loadImpulseResponse(result, juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::yes, 0, juce::dsp::Convolution::Normalise::yes);
                    irfftComponent.loadedIRChanged(result);
                });
            userIRLoaded = true;
            //DBG("loaded ir " << (int)userIRLoaded.compareAndSetBool(true, true) << "with length " << audioProcessor.irLoader.getCurrentIRSize());
    };

    outputGainSlider.onValueChange = [this] { audioProcessor.outputGain.setGainDecibels(outputGainSlider.getValue()); /*DBG("Output gain set to " << outputGainSlider.getValue());*/ };

    setSize (800, 600);

    startTimerHz(30);
}

BasicEQAudioProcessorEditor::~BasicEQAudioProcessorEditor()
{
    lowCutBypassButton.setLookAndFeel(nullptr);
    highCutBypassButton.setLookAndFeel(nullptr);
    peakBypassButton.setLookAndFeel(nullptr);
    lowCutFreqSlider.setLookAndFeel(nullptr);
    lowCutSlopeSlider.setLookAndFeel(nullptr);
    highCutFreqSlider.setLookAndFeel(nullptr);
    highCutSlopeSlider.setLookAndFeel(nullptr);
    peakFreqSlider.setLookAndFeel(nullptr);
    peakGainSlider.setLookAndFeel(nullptr);
    peakQualitySlider.setLookAndFeel(nullptr);
    xPosSlider.setLookAndFeel(nullptr);
    yPosSlider.setLookAndFeel(nullptr);
    irBypassButton.setLookAndFeel(nullptr);
    outputGainSlider.setLookAndFeel(nullptr);
}

//==============================================================================
void BasicEQAudioProcessorEditor::timerCallback()
{
    meterLeft.setLevel(audioProcessor.getRMSValue(0));
    meterRight.setLevel(audioProcessor.getRMSValue(1));
    meterLeft.repaint();
    meterRight.repaint();
}

void BasicEQAudioProcessorEditor::paint (juce::Graphics& g)
{
    using namespace juce;
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll (Colours::black);
    backgroundImage = ImageCache::getFromMemory(BinaryData::darkbrushedmetaltexturesteelblackstockphotoscratchwallpaper_png, BinaryData::darkbrushedmetaltexturesteelblackstockphotoscratchwallpaper_pngSize);
    g.drawImage(backgroundImage, getLocalBounds().toFloat(), RectanglePlacement::stretchToFit);
}

void BasicEQAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    auto bounds = getLocalBounds();
    auto responseArea = bounds.removeFromTop(bounds.getHeight() * 0.33);

    auto responseCurveComponentBounds = responseArea.removeFromRight(responseArea.getWidth() * 0.5);
    responseCurveComponent.setBounds(responseCurveComponentBounds.reduced(responseCurveComponentBounds.getWidth()*0.05, 0).removeFromBottom(responseCurveComponentBounds.getHeight()*0.95));

    auto GainArea = bounds;
    auto IRArea = bounds.removeFromLeft(bounds.getWidth() * 0.5);
    auto IRSlidersArea = IRArea.removeFromBottom(IRArea.getHeight() * 0.7);
    IRSlidersArea.reduce(IRSlidersArea.getWidth()*0.05, 0);
    IRSlidersArea.removeFromBottom(IRSlidersArea.getHeight() * 0.4);
    xPosSlider.setBounds(IRSlidersArea.removeFromRight(IRSlidersArea.getWidth() * 0.5));
    yPosSlider.setBounds(IRSlidersArea);

    auto comboBoxArea = IRArea;
    auto mikBoxArea = IRArea;
    auto irBypassButtonArea = IRArea;
    comboBoxArea = comboBoxArea.removeFromLeft(comboBoxArea.getWidth() * 0.5);
    comboBoxArea = comboBoxArea.removeFromLeft(comboBoxArea.getWidth() * 0.75);
    comboBoxArea = comboBoxArea.removeFromRight(comboBoxArea.getWidth() * 0.67);
    comboBoxArea.reduce(0, comboBoxArea.getHeight() * 0.35);
    comboTypeBox.setBounds(comboBoxArea);

    mikBoxArea = mikBoxArea.removeFromRight(mikBoxArea.getWidth() * 0.5);
    mikBoxArea = mikBoxArea.removeFromRight(mikBoxArea.getWidth() * 0.75);
    mikBoxArea = mikBoxArea.removeFromLeft(mikBoxArea.getWidth() * 0.67);
    mikBoxArea.reduce(0, mikBoxArea.getHeight() * 0.35);
    mikTypeBox.setBounds(mikBoxArea);

    irBypassButtonArea.reduce(irBypassButtonArea.getWidth() * 0.37, irBypassButtonArea.getHeight() * 0.15);
    irBypassButtonArea.removeFromTop(irBypassButtonArea.getHeight() * 0.55);
    irBypassButton.setBounds(irBypassButtonArea);

    auto IrFFTComponentBounds = responseArea;
    irfftComponent.setBounds(IrFFTComponentBounds.reduced(IrFFTComponentBounds.getWidth() * 0.05, 0).removeFromBottom(IrFFTComponentBounds.getHeight() * 0.95));

    //bounds.removeFromTop(5);
    auto EQArea = bounds.removeFromRight(bounds.getWidth());
    EQArea.reduce(EQArea.getWidth() * 0.05, 0);
    EQArea.removeFromBottom(EQArea.getHeight() * 0.279);
    EQArea.removeFromTop(EQArea.getHeight() * 0.05);
    auto lowCutArea = EQArea.removeFromLeft(EQArea.getWidth() * 0.33);
    auto highCutArea = EQArea.removeFromRight(EQArea.getWidth() * 0.5);

    lowCutBypassButton.setBounds(lowCutArea.removeFromTop(30));
    lowCutArea.removeFromTop(lowCutArea.getHeight() * 0.02);
    lowCutFreqSlider.setBounds(lowCutArea.removeFromTop(lowCutArea.getHeight() * 0.66));
    lowCutSlopeSlider.setBounds(lowCutArea);

    highCutBypassButton.setBounds(highCutArea.removeFromTop(30));
    highCutArea.removeFromTop(highCutArea.getHeight() * 0.02);
    highCutFreqSlider.setBounds(highCutArea.removeFromTop(highCutArea.getHeight() * 0.66 ));
    highCutSlopeSlider.setBounds(highCutArea);

    peakBypassButton.setBounds(EQArea.removeFromTop(30));
    EQArea.removeFromTop(EQArea.getHeight() * 0.02);
    peakFreqSlider.setBounds(EQArea.removeFromTop(EQArea.getHeight() * 0.33));
    peakGainSlider.setBounds(EQArea.removeFromTop(EQArea.getHeight() * 0.5));
    peakQualitySlider.setBounds(EQArea);

    auto loadBtnArea = IRArea;
    loadBtn.setBounds(loadBtnArea.removeFromLeft(loadBtnArea.getWidth()*0.6).removeFromRight(loadBtnArea.getWidth()*0.5).removeFromBottom(loadBtnArea.getHeight()*0.9).removeFromTop(loadBtnArea.getHeight()*0.3));
    irNameLabel.setBounds(loadBtnArea);

    GainArea.removeFromTop(GainArea.getHeight() * 0.5);
    GainArea.removeFromTop(GainArea.getHeight() * 0.6);
    GainArea.reduce(GainArea.getWidth() * 0.02, 0);
    
    auto OutputGainArea = GainArea;
    OutputGainArea.reduce(OutputGainArea.getWidth() * 0.44, 0);
    outputGainSlider.setBounds(OutputGainArea);

    auto meterLeftArea = GainArea.removeFromLeft(GainArea.getWidth() * 0.5);
    auto meterRightArea = GainArea;
    meterLeftArea.removeFromRight(OutputGainArea.getWidth() * 0.6);
    meterLeftArea.removeFromLeft(OutputGainArea.getWidth() * 0.6);
    meterRightArea.removeFromLeft(OutputGainArea.getWidth() * 0.6);
    meterRightArea.removeFromRight(OutputGainArea.getWidth() * 0.6);
    meterLeftArea.reduce(0, meterLeftArea.getHeight() * 0.35);
    meterRightArea.reduce(0, meterRightArea.getHeight() * 0.35);
    meterLeftArea.translate(0, meterLeftArea.getHeight() * (-0.5));
    meterRightArea.translate(0, meterRightArea.getHeight() * (-0.5));

    meterLeft.setBounds(meterLeftArea);
    meterRight.setBounds(meterRightArea);

}

std::vector<juce::Component*> BasicEQAudioProcessorEditor::getComps()
{
    return
    {
        &peakFreqSlider,
        &peakGainSlider,
        &peakQualitySlider,
        &lowCutFreqSlider,
        &highCutFreqSlider,
        &lowCutSlopeSlider,
        &highCutSlopeSlider,
        &responseCurveComponent,
        &irfftComponent,
        &loadBtn,
        &irNameLabel,
        &xPosSlider,
        &yPosSlider,
        &comboTypeBox,
        &mikTypeBox,
        &lowCutBypassButton,
        &peakBypassButton,
        &highCutBypassButton,
        &irBypassButton,
        &outputGainSlider,
        &meterLeft,
        &meterRight
    };
}

