/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

ResponseCurveComponent::ResponseCurveComponent(BasicEQAudioProcessor& p) : audioProcessor(p)
{
    const auto& params = audioProcessor.getParameters();
    for (auto param : params)
    {
        param->addListener(this);
    }

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
    DBG("param changed" << parameterIndex);
    parametersChanged.set(true);
}

void ResponseCurveComponent::timerCallback()
{
    if (parametersChanged.compareAndSetBool(false, true))
    {
        //update mono chain
        auto chainSettings = getChainSettings(audioProcessor.apvts);
        auto peakCoefficients = makePeakFilter(chainSettings, audioProcessor.getSampleRate());
        updateCoefficients(monoChain.get<ChainPositions::Peak>().coefficients, peakCoefficients);

        auto lowCutCoefficients = makeLowCutFilter(chainSettings, audioProcessor.getSampleRate());
        auto highCutCoefficients = makeHighCutFilter(chainSettings, audioProcessor.getSampleRate());

        updateCutFilter(monoChain.get<ChainPositions::LowCut>(), lowCutCoefficients, chainSettings.lowCutSlope);
        updateCutFilter(monoChain.get<ChainPositions::HighCut>(), highCutCoefficients, chainSettings.highCutSlope);
        //signal a repaint
        repaint();
    }
}

void ResponseCurveComponent::parameterGestureChanged(int parameterIndex, bool gestureIsStarting) {  };

void ResponseCurveComponent::paint(juce::Graphics& g)
{
    using namespace juce;
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(Colours::black);

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
        if (!lowcut.isBypassed<0>())
            mag *= lowcut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!lowcut.isBypassed<1>())
            mag *= lowcut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!lowcut.isBypassed<2>())
            mag *= lowcut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!lowcut.isBypassed<3>())
            mag *= lowcut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);

        if (!highcut.isBypassed<0>())
            mag *= highcut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!highcut.isBypassed<1>())
            mag *= highcut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!highcut.isBypassed<2>())
            mag *= highcut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!highcut.isBypassed<3>())
            mag *= highcut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);

        mags[i] = Decibels::gainToDecibels(mag);
    }

    Path filterResponseCurve;

    const double outputMin = responseArea.getBottom();
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

    g.setColour(Colours::springgreen);
    g.drawRoundedRectangle(responseArea.toFloat(), 4.f, 1.5f);

    g.setColour(Colours::white);
    g.strokePath(filterResponseCurve, PathStrokeType(2.f));
}

//==============================================================================
BasicEQAudioProcessorEditor::BasicEQAudioProcessorEditor(BasicEQAudioProcessor& p) : AudioProcessorEditor(&p), audioProcessor(p),
responseCurveComponent(audioProcessor),
peakFreqSliderAttachment(audioProcessor.apvts, "Peak Freq", peakFreqSlider),
peakGainSliderAttachment(audioProcessor.apvts, "Peak Gain", peakGainSlider),
peakQualitySliderAttachment(audioProcessor.apvts, "Peak Q", peakQualitySlider),
lowCutFreqSliderAttachment(audioProcessor.apvts, "LowCut Freq", lowCutFreqSlider),
lowCutSlopeSliderAttachment(audioProcessor.apvts, "LowCut Slope", lowCutSlopeSlider),
highCutFreqSliderAttachment(audioProcessor.apvts, "HighCut Freq", highCutFreqSlider),
highCutSlopeSliderAttachment(audioProcessor.apvts, "HighCut Slope", highCutSlopeSlider),
xPosSliderAttachment(audioProcessor.apvts, "X Position", xPosSlider),
yPosSliderAttachment(audioProcessor.apvts, "Y Position", yPosSlider)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    for (auto* comp : getComps())
    {
        addAndMakeVisible(comp);
    }

    comboTypeBox.addItem("Mar", 1);
    comboTypeBox.addItem("MM", 2);
    comboTypeBox.addItem("SV", 3);
    comboTypeBox.setSelectedId(1);
    comboTypeBox.onChange = [this]() { DBG("changed combo"); audioProcessor.updateLoadedIR(comboTypeBox.getSelectedId() - 1, mikTypeBox.getSelectedId() - 1, yPosSlider.getValue(), xPosSlider.getValue()); userIRLoaded = false; };

    mikTypeBox.addItem("57A", 1);
    mikTypeBox.addItem("kalib", 2);
    mikTypeBox.addItem("sm57", 3);
    mikTypeBox.setSelectedId(1);
    mikTypeBox.onChange = [this]() { DBG("changed mic"); audioProcessor.updateLoadedIR(comboTypeBox.getSelectedId()-1, mikTypeBox.getSelectedId()-1, yPosSlider.getValue(), xPosSlider.getValue()); userIRLoaded = false; };

    yPosSlider.onValueChange = [this]() { DBG("changed yPos to " << yPosSlider.getValue()); audioProcessor.updateLoadedIR(comboTypeBox.getSelectedId() - 1, mikTypeBox.getSelectedId() - 1, yPosSlider.getValue(), xPosSlider.getValue()); userIRLoaded = false; };
    xPosSlider.onValueChange = [this]() { DBG("changed xPos to " << xPosSlider.getValue()); audioProcessor.updateLoadedIR(comboTypeBox.getSelectedId() - 1, mikTypeBox.getSelectedId() - 1, yPosSlider.getValue(), xPosSlider.getValue()); userIRLoaded = false; };

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
                    audioProcessor.savedFile = result;
                    audioProcessor.root = result.getParentDirectory().getFullPathName();    // set root directory to where the file was selected from
                    irNameLabel.setText( result.getFileNameWithoutExtension(), juce::dontSendNotification );
                    audioProcessor.irLoader.reset();
                    // load IR, stereo, trimmed, normalized, size 0 = original IR size
                    audioProcessor.irLoader.loadImpulseResponse(result, juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::yes, 0, juce::dsp::Convolution::Normalise::yes);
                });
            userIRLoaded = true;
            DBG("loaded ir " << (int)userIRLoaded.compareAndSetBool(true, true) << "with length " << audioProcessor.irLoader.getCurrentIRSize());
    };

    setSize (800, 600);
}

BasicEQAudioProcessorEditor::~BasicEQAudioProcessorEditor()
{
    
}

//==============================================================================
void BasicEQAudioProcessorEditor::paint (juce::Graphics& g)
{
    using namespace juce;
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (Colours::black);
}

void BasicEQAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    auto bounds = getLocalBounds();
    auto responseArea = bounds.removeFromTop(bounds.getHeight() * 0.33);
    responseCurveComponent.setBounds(responseArea.removeFromRight(bounds.getWidth()*0.5));

    auto IRArea = bounds.removeFromLeft(bounds.getWidth() * 0.5);
    auto IRSlidersArea = IRArea.removeFromBottom(IRArea.getHeight() * 0.5);
    xPosSlider.setBounds(IRSlidersArea.removeFromRight(IRSlidersArea.getWidth() * 0.5));
    yPosSlider.setBounds(IRSlidersArea);

    auto comboBoxArea = IRArea;
    auto mikBoxArea = IRArea;
    comboBoxArea = comboBoxArea.removeFromLeft(comboBoxArea.getWidth() * 0.5);
    comboBoxArea = comboBoxArea.removeFromLeft(comboBoxArea.getWidth() * 0.75);
    comboBoxArea = comboBoxArea.removeFromRight(comboBoxArea.getWidth() * 0.67);
    mikBoxArea = mikBoxArea.removeFromRight(mikBoxArea.getWidth() * 0.5);
    mikBoxArea = mikBoxArea.removeFromRight(mikBoxArea.getWidth() * 0.75);
    mikBoxArea = mikBoxArea.removeFromLeft(mikBoxArea.getWidth() * 0.67);
    comboTypeBox.setBounds(comboBoxArea.removeFromTop(comboBoxArea.getHeight() * 0.66).removeFromBottom(comboBoxArea.getHeight()*0.5));
    mikTypeBox.setBounds(mikBoxArea.removeFromTop(mikBoxArea.getHeight() * 0.66).removeFromBottom(mikBoxArea.getHeight() * 0.5));

    auto EQArea = bounds.removeFromRight(bounds.getWidth());
    auto lowCutArea = EQArea.removeFromLeft(EQArea.getWidth() * 0.33);
    auto highCutArea = EQArea.removeFromRight(EQArea.getWidth() * 0.5);

    lowCutFreqSlider.setBounds(lowCutArea.removeFromTop(lowCutArea.getHeight() * 0.66));
    lowCutSlopeSlider.setBounds(lowCutArea);

    highCutFreqSlider.setBounds(highCutArea.removeFromTop(highCutArea.getHeight() * 0.66 ));
    highCutSlopeSlider.setBounds(highCutArea);

    peakFreqSlider.setBounds(EQArea.removeFromTop(EQArea.getHeight() * 0.33));
    peakGainSlider.setBounds(EQArea.removeFromTop(EQArea.getHeight() * 0.5));
    peakQualitySlider.setBounds(EQArea);

    auto loadBtnArea = IRArea;
    loadBtn.setBounds(loadBtnArea.removeFromLeft(loadBtnArea.getWidth()*0.6).removeFromRight(loadBtnArea.getWidth()*0.5).removeFromBottom(loadBtnArea.getHeight()*0.8).removeFromTop(loadBtnArea.getHeight()*0.2));
    irNameLabel.setBounds(loadBtnArea);

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
        &loadBtn,
        &irNameLabel,
        &xPosSlider,
        &yPosSlider,
        &comboTypeBox,
        &mikTypeBox
    };
}