/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

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
inputGainSlider(*audioProcessor.apvts.getParameter("Input Gain"), "dB"),
outputGainSlider(*audioProcessor.apvts.getParameter("Output Gain"), "dB"),
asymPosGainSlider(*audioProcessor.apvts.getParameter("AsymPosGain"), ""),
asymNegGainSlider(*audioProcessor.apvts.getParameter("AsymNegGain"), ""),
symGainSlider(*audioProcessor.apvts.getParameter("SymGain"), ""),
symLPLNSlider(*audioProcessor.apvts.getParameter("SymLPLN"), ""),
asymPosLPSlider(*audioProcessor.apvts.getParameter("AsymPosLP"), ""),
asymPosLNSlider(*audioProcessor.apvts.getParameter("AsymPosLN"), ""),
asymNegLPSlider(*audioProcessor.apvts.getParameter("AsymNegLP"), ""),
asymNegLNSlider(*audioProcessor.apvts.getParameter("AsymNegLN"), ""),
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
inputGainSliderAttachment(audioProcessor.apvts, "Input Gain", inputGainSlider),
outputGainSliderAttachment(audioProcessor.apvts, "Output Gain", outputGainSlider),
lowCutBypassButtonAttachment(audioProcessor.apvts, "LowCut Bypassed", lowCutBypassButton),
highCutBypassButtonAttachment(audioProcessor.apvts, "HighCut Bypassed", highCutBypassButton),
peakBypassButtonAttachment(audioProcessor.apvts, "Peak Bypassed", peakBypassButton),
ampBypassButtonAttachment(audioProcessor.apvts, "Amp Bypassed", ampBypassButton),
irBypassButtonAttachment(audioProcessor.apvts, "IR Bypassed", irBypassButton),
asymPosGainSliderAttachment(audioProcessor.apvts, "AsymPosGain", asymPosGainSlider),
asymNegGainSliderAttachment(audioProcessor.apvts, "AsymNegGain", asymNegGainSlider),
symGainSliderAttachment(audioProcessor.apvts, "SymGain", symGainSlider),
symLPLNSliderAttachment(audioProcessor.apvts, "SymLPLN", symLPLNSlider),
asymPosLPSliderAttachment(audioProcessor.apvts, "AsymPosLP", asymPosLPSlider),
asymPosLNSliderAttachment(audioProcessor.apvts, "AsymPosLN", asymPosLNSlider),
asymNegLPSliderAttachment(audioProcessor.apvts, "AsymNegLP", asymNegLPSlider),
asymNegLNSliderAttachment(audioProcessor.apvts, "AsymNegLN", asymNegLNSlider)

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
    xPosSlider.labels.add({ 1.f, "10cm" });
    yPosSlider.labels.add({ 0.f, "0cm" });
    yPosSlider.labels.add({ 1.f, "40cm" });
    symGainSlider.labels.add({ 0.f, "SG" });
    asymNegGainSlider.labels.add({ 0.f, "ANG" });
    asymPosGainSlider.labels.add({ 0.f, "APG" });
    asymNegLNSlider.labels.add({ 0.f, "ANLN" });
    asymNegLPSlider.labels.add({ 0.f, "ANLP" });
    asymPosLNSlider.labels.add({ 0.f, "APLN" });
    asymPosLPSlider.labels.add({ 0.f, "APLP" });
    symLPLNSlider.labels.add({ 0.f, "SLPLN" });

    

    comboTypeBox.addItem("Mar", 1);
    comboTypeBox.addItem("MM", 2);
    comboTypeBox.addItem("SV", 3);
    comboTypeBox.setSelectedId(1);
    comboTypeBox.onChange = [this]() { 
        DBG("changed combo");
        juce::AudioBuffer<float> irBuffer;
        int sampleRate = 0;
        audioProcessor.updateLoadedIR(irBuffer, sampleRate, comboTypeBox.getSelectedId() - 1, mikTypeBox.getSelectedId() - 1, yPosSlider.getValue(), xPosSlider.getValue());
        userIRLoaded = false; 
        irfftComponent.loadedIRChanged(irBuffer, sampleRate);
        };

    mikTypeBox.addItem("57A", 1);
    mikTypeBox.addItem("kalib", 2);
    mikTypeBox.addItem("sm57", 3);
    mikTypeBox.setSelectedId(1);
    mikTypeBox.onChange = [this]() {
        DBG("changed mic");
        juce::AudioBuffer<float> irBuffer;
        int sampleRate = 0;
        audioProcessor.updateLoadedIR(irBuffer, sampleRate, comboTypeBox.getSelectedId() - 1, mikTypeBox.getSelectedId() - 1, yPosSlider.getValue(), xPosSlider.getValue());
        userIRLoaded = false;
        irfftComponent.loadedIRChanged(irBuffer, sampleRate);
        };

    ampTypeBox.addItem("Poletti", 1);
    ampTypeBox.addItem("Placeholder", 2);
    ampTypeBox.setSelectedId(1);
    ampTypeBox.onChange = [this]() {
        audioProcessor.ampDrive.simTypeChanged(ampTypeBox.getSelectedId());
        };

    for (auto* comp : getComps())
    {
        addAndMakeVisible(comp);
    }

    lowCutBypassButton.setLookAndFeel(&lnf);
    highCutBypassButton.setLookAndFeel(&lnf);
    peakBypassButton.setLookAndFeel(&lnf);
    irBypassButton.setLookAndFeel(&lnf);
    ampBypassButton.setLookAndFeel(&lnf);
    //lowCutFreqSlider.setLookAndFeel(&lnf);
    //lowCutSlopeSlider.setLookAndFeel(&lnf);
    //highCutFreqSlider.setLookAndFeel(&lnf);
    //highCutSlopeSlider.setLookAndFeel(&lnf);
    //peakFreqSlider.setLookAndFeel(&lnf);
    //peakGainSlider.setLookAndFeel(&lnf);
    //peakQualitySlider.setLookAndFeel(&lnf);
    //xPosSlider.setLookAndFeel(&lnf);
    //yPosSlider.setLookAndFeel(&lnf);
    //outputGainSlider.setLookAndFeel(&lnf);

    comboTypeBoxAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(p.apvts, "Combo Type", comboTypeBox);
    mikTypeBoxAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(p.apvts, "Mic Type", mikTypeBox);
    ampTypeBoxAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(p.apvts, "Amp Type", ampTypeBox);

    // TODO: Interpolate loaded IR, X and Y pos now floats
    yPosSlider.onValueChange = [this]() { 
        ////DBG("changed yPos to " << yPosSlider.getValue());
        //juce::AudioBuffer<float> irBuffer;
        //int sampleRate = 0;
        //audioProcessor.updateLoadedIR(irBuffer, sampleRate, comboTypeBox.getSelectedId() - 1, mikTypeBox.getSelectedId() - 1, yPosSlider.getValue(), xPosSlider.getValue());
        //userIRLoaded = false;
        ////if (tempFile.getFile().getSize() == 0) return;
        //irfftComponent.loadedIRChanged(irBuffer, sampleRate);
        needIRUpdate = true;
    };
    xPosSlider.onValueChange = [this]() { 
        ////DBG("changed xPos to " << xPosSlider.getValue());
        //juce::AudioBuffer<float> irBuffer;
        //int sampleRate = 0;
        //audioProcessor.updateLoadedIR(irBuffer, sampleRate, comboTypeBox.getSelectedId() - 1, mikTypeBox.getSelectedId() - 1, yPosSlider.getValue(), xPosSlider.getValue());
        //userIRLoaded = false;
        ////if (tempFile.getFile().getSize() == 0) return;
        //irfftComponent.loadedIRChanged(irBuffer, sampleRate);
        needIRUpdate = true;
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
                    //irNameLabel.setText( result.getFileNameWithoutExtension(), juce::dontSendNotification );
                    audioProcessor.irLoader.reset();
                    // load IR, stereo, trimmed, normalized, size 0 = original IR size
                    audioProcessor.irLoader.loadImpulseResponse(result, juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::yes, 0, juce::dsp::Convolution::Normalise::yes);
                    irfftComponent.loadedIRChanged(result);
                });
            userIRLoaded = true;
            //DBG("loaded ir " << (int)userIRLoaded.compareAndSetBool(true, true) << "with length " << audioProcessor.irLoader.getCurrentIRSize());
    };

    inputGainSlider.onValueChange = [this] { audioProcessor.inputGain.setGainDecibels(inputGainSlider.getValue()); };
    outputGainSlider.onValueChange = [this] { audioProcessor.outputGain.setGainDecibels(outputGainSlider.getValue()); /*DBG("Output gain set to " << outputGainSlider.getValue());*/ };

    setSize (1000, 600);

    startTimer(0, 33);
    startTimer(1, 200);
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
    inputGainSlider.setLookAndFeel(nullptr);
    outputGainSlider.setLookAndFeel(nullptr);
    asymPosGainSlider.setLookAndFeel(nullptr);
    asymNegGainSlider.setLookAndFeel(nullptr);
    symGainSlider.setLookAndFeel(nullptr);
    symLPLNSlider.setLookAndFeel(nullptr);
    asymPosLPSlider.setLookAndFeel(nullptr);
    asymPosLNSlider.setLookAndFeel(nullptr);
    asymNegLPSlider.setLookAndFeel(nullptr);
    asymNegLNSlider.setLookAndFeel(nullptr);
    ampBypassButton.setLookAndFeel(nullptr);
}

//==============================================================================
void BasicEQAudioProcessorEditor::timerCallback(int timerID)
{
    if (timerID == 0) {
        meterInLeft.setLevel(audioProcessor.getInputRMSValue(0));
        meterInRight.setLevel(audioProcessor.getInputRMSValue(1));
        meterInLeft.repaint();
        meterInRight.repaint();
        meterOutLeft.setLevel(audioProcessor.getOutputRMSValue(0));
        meterOutRight.setLevel(audioProcessor.getOutputRMSValue(1));
        meterOutLeft.repaint();
        meterOutRight.repaint();
    }
    else if (timerID == 1) {
        if (needIRUpdate) {
            juce::AudioBuffer<float> irBuffer;
            int sampleRate = 0;
            audioProcessor.updateLoadedIR(irBuffer, sampleRate, comboTypeBox.getSelectedId() - 1, mikTypeBox.getSelectedId() - 1, yPosSlider.getValue(), xPosSlider.getValue());
            userIRLoaded = false;
            //if (tempFile.getFile().getSize() == 0) return;
            irfftComponent.loadedIRChanged(irBuffer, sampleRate);
            needIRUpdate = false;
        }
    }
}

void BasicEQAudioProcessorEditor::paint (juce::Graphics& g)
{
    using namespace juce;
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll (Colours::black);
    //backgroundImage = ImageCache::getFromMemory(BinaryData::darkbrushedmetaltexturesteelblackstockphotoscratchwallpaper_png, BinaryData::darkbrushedmetaltexturesteelblackstockphotoscratchwallpaper_pngSize);
    //g.drawImage(backgroundImage, getLocalBounds().toFloat(), RectanglePlacement::stretchToFit);
}

void BasicEQAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    auto bounds = getLocalBounds();
    bounds.reduce(bounds.getWidth() * 0.01, bounds.getHeight() * 0.01);
    auto meterWidth = bounds.getWidth() * 0.1;
    auto meterHeight = bounds.getHeight() * 0.2;
    auto inputMeterArea = bounds.removeFromLeft(meterWidth);
    auto outputMeterArea = bounds.removeFromRight(meterWidth);
    auto inputGainArea = inputMeterArea.removeFromBottom(meterHeight);
    auto outputGainArea = outputMeterArea.removeFromBottom(meterHeight);
    inputGainSlider.setBounds(inputGainArea);
    outputGainSlider.setBounds(outputGainArea);
    meterInLeft.setBounds(inputMeterArea.removeFromLeft(inputMeterArea.getWidth()*0.5));
    meterInRight.setBounds(inputMeterArea);
    meterOutLeft.setBounds(outputMeterArea.removeFromLeft(outputMeterArea.getWidth() * 0.5));
    meterOutRight.setBounds(outputMeterArea);


    auto responseArea = bounds.removeFromTop(bounds.getHeight() * 0.4);

    auto responseCurveComponentBounds = responseArea.removeFromRight(responseArea.getWidth() * 0.5);
    responseCurveComponent.setBounds(responseCurveComponentBounds.reduced(responseCurveComponentBounds.getWidth()*0.05, 0).removeFromBottom(responseCurveComponentBounds.getHeight()*0.95));

    /*auto AmpArea = bounds.removeFromBottom(bounds.getHeight() * 0.5);
    auto AmpAreaWidth = AmpArea.getWidth();
    auto WidthOfOneSlider = AmpAreaWidth / 10;
    ampBypassButton.setBounds(AmpArea.removeFromLeft(WidthOfOneSlider));
    asymPosGainSlider.setBounds(AmpArea.removeFromLeft(WidthOfOneSlider));
    asymNegGainSlider.setBounds(AmpArea.removeFromLeft(WidthOfOneSlider));
    symGainSlider.setBounds(AmpArea.removeFromLeft(WidthOfOneSlider));
    asymPosLPSlider.setBounds(AmpArea.removeFromLeft(WidthOfOneSlider));
    asymPosLNSlider.setBounds(AmpArea.removeFromLeft(WidthOfOneSlider));
    asymNegLPSlider.setBounds(AmpArea.removeFromLeft(WidthOfOneSlider));
    asymNegLNSlider.setBounds(AmpArea.removeFromLeft(WidthOfOneSlider));
    symLPLNSlider.setBounds(AmpArea.removeFromLeft(WidthOfOneSlider));
    ampTypeBox.setBounds(AmpArea);*/


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
    //irNameLabel.setBounds(loadBtnArea);

    GainArea.removeFromTop(GainArea.getHeight() * 0.5);
    GainArea.removeFromTop(GainArea.getHeight() * 0.6);
    GainArea.reduce(GainArea.getWidth() * 0.02, 0);
    
    //auto OutputGainArea = GainArea;
    //OutputGainArea.reduce(OutputGainArea.getWidth() * 0.44, 0);
    //inputGainSlider.setBounds(OutputGainArea.removeFromLeft(OutputGainArea.getWidth() * 0.5));
    //OutputGainArea.reduce(OutputGainArea.getWidth() * 0.44, 0);
    //outputGainSlider.setBounds(OutputGainArea);

    /*auto meterLeftArea = GainArea.removeFromLeft(GainArea.getWidth() * 0.5);
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
    meterRight.setBounds(meterRightArea);*/

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
        &inputGainSlider,
        &outputGainSlider,
        &meterInLeft,
        &meterInRight,
        &meterOutLeft,
        &meterOutRight/*,
        &asymPosGainSlider,
        &asymNegGainSlider,
        &symGainSlider,
        &symLPLNSlider,
        &asymPosLPSlider,
        &asymPosLNSlider,
        &asymNegLPSlider,
        &asymNegLNSlider,
        &ampBypassButton,
        &ampTypeBox*/
    };
}

