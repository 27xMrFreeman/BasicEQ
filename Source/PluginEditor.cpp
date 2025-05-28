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
driveSlider(*audioProcessor.apvts.getParameter("Drive"), ""),
lowShelfSlider(*audioProcessor.apvts.getParameter("LowShelfGain"), ""),
midPeakSlider(*audioProcessor.apvts.getParameter("MidPeakGain"), ""),
highShelfSlider(*audioProcessor.apvts.getParameter("HighShelfGain"), ""),
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
driveSliderAttachment(audioProcessor.apvts, "Drive", driveSlider),
lowShelfSliderAttachment(audioProcessor.apvts, "LowShelfGain", lowShelfSlider),
midPeakSliderAttachment(audioProcessor.apvts, "MidPeakGain", midPeakSlider),
highShelfSliderAttachment(audioProcessor.apvts, "HighShelfGain", highShelfSlider),
lowCutBypassButtonAttachment(audioProcessor.apvts, "LowCut Bypassed", lowCutBypassButton),
highCutBypassButtonAttachment(audioProcessor.apvts, "HighCut Bypassed", highCutBypassButton),
peakBypassButtonAttachment(audioProcessor.apvts, "Peak Bypassed", peakBypassButton),
ampBypassButtonAttachment(audioProcessor.apvts, "Amp Bypassed", ampBypassButton),
irBypassButtonAttachment(audioProcessor.apvts, "IR Bypassed", irBypassButton)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
   
    tooltipWindow.setMillisecondsBeforeTipAppears(150);

    xPosSlider.name = "X Position";
    yPosSlider.name = "Y Position";
    driveSlider.name = "Drive";
    lowShelfSlider.name = "Low";
    midPeakSlider.name = "Mid";
    highShelfSlider.name = "High";
    inputGainSlider.name = "Input";
    outputGainSlider.name = "Output";

    comboTypeBox.addItem("Mar", 1);
    comboTypeBox.addItem("MM", 2);
    comboTypeBox.addItem("SV", 3);
    comboTypeBox.onChange = [this]() { 
        needIRUpdate.set(true);
        };

    aCabButton.setRadioGroupId(RadioButtonIDs::CabTypeButtons);
    aCabButton.setClickingTogglesState(true);
    aCabButton.onClick = [this]() { comboTypeBox.setSelectedItemIndex(0); };
    aCabButton.setName("A");

    bCabButton.setRadioGroupId(RadioButtonIDs::CabTypeButtons);
    bCabButton.setClickingTogglesState(true);
    bCabButton.onClick = [this]() { comboTypeBox.setSelectedItemIndex(1); };
    bCabButton.setName("B");

    cCabButton.setRadioGroupId(RadioButtonIDs::CabTypeButtons);
    cCabButton.setClickingTogglesState(true);
    cCabButton.onClick = [this]() { comboTypeBox.setSelectedItemIndex(2); };
    cCabButton.setName("C");

    int cabTypeSelected = audioProcessor.apvts.getRawParameterValue("Combo Type")->load();

    switch (cabTypeSelected) {
    case 0:
        aCabButton.triggerClick();
        break;
    case 1:
        bCabButton.triggerClick();
        break;
    case 2:
        cCabButton.triggerClick();
        break;
    }

    mikTypeBox.addItem("57A", 1);
    mikTypeBox.addItem("kalib", 2);
    mikTypeBox.addItem("sm57", 3);
    mikTypeBox.onChange = [this]() {
        needIRUpdate.set(true);
        };

    aMicButton.setRadioGroupId(RadioButtonIDs::MicTypeButtons);
    aMicButton.setClickingTogglesState(true);
    aMicButton.onClick = [this]() { mikTypeBox.setSelectedItemIndex(0); };
    aMicButton.setName("A");
    bMicButton.setRadioGroupId(RadioButtonIDs::MicTypeButtons);
    bMicButton.setClickingTogglesState(true);
    bMicButton.onClick = [this]() { mikTypeBox.setSelectedItemIndex(1); };
    bMicButton.setName("B");
    cMicButton.setRadioGroupId(RadioButtonIDs::MicTypeButtons);
    cMicButton.setClickingTogglesState(true);
    cMicButton.onClick = [this]() { mikTypeBox.setSelectedItemIndex(2); };
    cMicButton.setName("C");
    int micTypeSelected = audioProcessor.apvts.getRawParameterValue("Mic Type")->load();

    switch (micTypeSelected) {
    case 0:
        aMicButton.triggerClick();
        break;
    case 1:
        bMicButton.triggerClick();
        break;
    case 2:
        cMicButton.triggerClick();
        break;
    }

    ampTypeSwitch.ampTypeBox.onChange = [this]() {
        audioProcessor.ampSim.ampType = static_cast<AmpTypeEnum>(ampTypeSwitch.ampTypeBox.getSelectedId() - 1);
        };

    
    ampTypeSwitch.polettiAmpTypeButton.onClick = [this]() {
        // on click lambda gets called on every button change
        if (!ampTypeSwitch.polettiAmpTypeButton.getToggleState()) return;
        ampTypeSwitch.ampTypeBox.setSelectedItemIndex(0);
        driveSlider.setSliderColor(juce::Colours::red);
        driveSlider.repaint();
        lowShelfSlider.setSliderColor(juce::Colours::red);
        lowShelfSlider.repaint();
        midPeakSlider.setSliderColor(juce::Colours::red);
        midPeakSlider.repaint();
        highShelfSlider.setSliderColor(juce::Colours::red);
        highShelfSlider.repaint();
        inputGainSlider.setSliderColor(juce::Colours::red);
        inputGainSlider.repaint();
        outputGainSlider.setSliderColor(juce::Colours::red);
        outputGainSlider.repaint();
        xyPad.thumb.setThumbColor(juce::Colours::red);
        xyPad.thumb.repaint();
        this->lnf.bypassButtonFillColor = juce::Colours::red;
        ampBypassButton.repaint();
        };

    ampTypeSwitch.yamahaAmpTypeButton.onClick = [this]() { 
        if (!ampTypeSwitch.yamahaAmpTypeButton.getToggleState()) return;
        ampTypeSwitch.ampTypeBox.setSelectedItemIndex(1);
        auto colour = juce::Colour::fromString("FF00E7D3");
        driveSlider.setSliderColor(colour);
        driveSlider.repaint();
        lowShelfSlider.setSliderColor(colour);
        lowShelfSlider.repaint();
        midPeakSlider.setSliderColor(colour);
        midPeakSlider.repaint();
        highShelfSlider.setSliderColor(colour);
        highShelfSlider.repaint();
        inputGainSlider.setSliderColor(colour);
        inputGainSlider.repaint();
        outputGainSlider.setSliderColor(colour);
        outputGainSlider.repaint();
        xyPad.thumb.setThumbColor(colour);
        xyPad.thumb.repaint();
        this->lnf.bypassButtonFillColor = colour;
        ampBypassButton.repaint();
        };
    
    ampTypeSwitch.wavefoldAmpTypeButton.onClick = [this]() { 
        if (!ampTypeSwitch.wavefoldAmpTypeButton.getToggleState()) return;
        ampTypeSwitch.ampTypeBox.setSelectedItemIndex(2);
        auto colour = juce::Colour::fromString("FFBF00FF");
        driveSlider.setSliderColor(colour);
        driveSlider.repaint();
        lowShelfSlider.setSliderColor(colour);
        lowShelfSlider.repaint();
        midPeakSlider.setSliderColor(colour);
        midPeakSlider.repaint();
        highShelfSlider.setSliderColor(colour);
        highShelfSlider.repaint();
        inputGainSlider.setSliderColor(colour);
        inputGainSlider.repaint();
        outputGainSlider.setSliderColor(colour);
        outputGainSlider.repaint();
        xyPad.thumb.setThumbColor(colour);
        xyPad.thumb.repaint();
        this->lnf.bypassButtonFillColor = colour;
        ampBypassButton.repaint();
        };

    switch (audioProcessor.ampSim.ampType)
    {
    case AmpTypeEnum::Poletti:
        ampTypeSwitch.polettiAmpTypeButton.triggerClick();
        break;
    case AmpTypeEnum::Yamaha:
        ampTypeSwitch.yamahaAmpTypeButton.triggerClick();
        break;
    case AmpTypeEnum::WaveFolder:
        ampTypeSwitch.wavefoldAmpTypeButton.triggerClick();
        break;
    }

    for (auto* comp : getComps())
    {
        addAndMakeVisible(comp);
    }

    for (auto* label : getLabels())
    {
        label->setJustificationType(juce::Justification::centred);
        label->setColour(juce::Label::ColourIds::textColourId, juce::Colour::fromString("FFE5E5E5"));
    }
    helpBtn.setTooltip("Mics:\nA = Beta 57A, B = Measurement mic\n C = Shure SM57\nCabs:\nA = Marshall 4x12, B = Marshall JCM2000-DSL401, C = Line6 Spider Valve 112\n\n"
        "Mic position grid represents measured points\nX axis is in 1cm steps from center of speaker\nY axis is 0, 10 and 40cm away\n\n"
    "Amp:\nBlue = Yamaha Class B emulation modified patent\nRed = Mark Poletti Class B modified patent\nPurple = Experimental wavefolding/shaping combination");
    irBypassLabel.setJustificationType(juce::Justification::right);
    irBypassLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colour::fromString("FF4F83F3"));
    lowCutBypassButton.setLookAndFeel(&lnf);
    highCutBypassButton.setLookAndFeel(&lnf);
    peakBypassButton.setLookAndFeel(&lnf);
    irBypassButton.setLookAndFeel(&lnfIRBypass);
    ampBypassButton.setLookAndFeel(&lnf);
    aMicButton.setLookAndFeel(&lnfChoices);
    bMicButton.setLookAndFeel(&lnfChoices);
    cMicButton.setLookAndFeel(&lnfChoices);
    aCabButton.setLookAndFeel(&lnfChoices);
    bCabButton.setLookAndFeel(&lnfChoices);
    cCabButton.setLookAndFeel(&lnfChoices);
    loadBtn.setLookAndFeel(&lnf);

    comboTypeBoxAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(p.apvts, "Combo Type", comboTypeBox);
    mikTypeBoxAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(p.apvts, "Mic Type", mikTypeBox);
    ampTypeSwitch.ampTypeBoxAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(p.apvts, "Amp Type", ampTypeSwitch.ampTypeBox);

    yPosSlider.onValueChange = [this]() {
        needIRUpdate.set(true);
    };
    xPosSlider.onValueChange = [this]() { 
        needIRUpdate.set(true);
        };

    xyPad.registerSlider(&xPosSlider, XYPad::Axis::X);
    xyPad.registerSlider(&yPosSlider, XYPad::Axis::Y);

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
                    if (!result.existsAsFile()) { /*pressed cancel*/ return; }
                    audioProcessor.savedFile = result;
                    audioProcessor.root = result.getParentDirectory().getFullPathName();    // set root directory to where the file was selected from
                    audioProcessor.irLoader.reset();
                    // load IR, stereo, trimmed, normalized, size 0 = original IR size
                    audioProcessor.irLoader.loadImpulseResponse(result, juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::yes, 0, juce::dsp::Convolution::Normalise::yes);
                    irfftComponent.loadedIRChanged(result);
                });
            userIRLoaded = true;
    };

    inputGainSlider.onValueChange = [this] { audioProcessor.inputGain.setGainDecibels(inputGainSlider.getValue()); };
    outputGainSlider.onValueChange = [this] { audioProcessor.outputGain.setGainDecibels(outputGainSlider.getValue()); };

    setSize (1674, 752);
    setResizable(true, true);
    setResizeLimits( 837, 376, 1674, 752);
    getConstrainer()->setFixedAspectRatio(1674.f / 752.f);

    startTimer(0, 33);
    startTimer(1, 50);

    if (audioProcessor.shippedIRsMissing.get()) {
        alertWindow = std::make_unique<juce::AlertWindow>("Alert", "Prepackaged files are missing or incomplete", juce::MessageBoxIconType::WarningIcon, nullptr);
        alertWindow->showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, "Alert", "Prepackaged files are missing or incomplete", "OK");
    }
}

BasicEQAudioProcessorEditor::~BasicEQAudioProcessorEditor()
{
    xyPad.deregisterSlider(&xPosSlider);
    xyPad.deregisterSlider(&yPosSlider);
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
    ampBypassButton.setLookAndFeel(nullptr);
    aMicButton.setLookAndFeel(nullptr);
    bMicButton.setLookAndFeel(nullptr);
    cMicButton.setLookAndFeel(nullptr);
    aCabButton.setLookAndFeel(nullptr);
    bCabButton.setLookAndFeel(nullptr);
    cCabButton.setLookAndFeel(nullptr);
    loadBtn.setLookAndFeel(nullptr);
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
        if (needIRUpdate.get() && !audioProcessor.shippedIRsMissing.get()) {
            juce::AudioBuffer<float> irBuffer;
            int sampleRate = 0;
            float yPos = yPosSlider.getValue(), 
                  xPos = xPosSlider.getValue();
            int comboTypeID = comboTypeBox.getSelectedId() - 1,
                mikTypeID = mikTypeBox.getSelectedId() - 1;
            // compute interpolation here, then pass interpolated IR buffer to audioprocessor, where it is checked each buffer, if new IR is ready to be loaded
            audioProcessor.newIRReady.set(false);

            // interpolate here
            int yPosRoundDown = 0, yPosRoundUp = 0, xPosRoundDown = 0, xPosRoundUp = 0;
            float XmaxDistance = 0, Xdistance = 0, XtransposedDistance = 0, YmaxDistance = 0, Ydistance = 0, YtransposedDistance = 0;
            int yPosArr[3] = { 0, 10, 40 };
            int xPosArr[6] = { 0, 2, 4, 6, 8, 10 };
            // formatManager takes a file (wav in our case), returns AudioBuffer


            // if Y = {0,10,40} and X = {0,2,4,..,10}, no need to interpolate
            if (std::any_of(std::begin(yPosArr), std::end(yPosArr), [&](int i) { return i == yPos; }) && std::any_of(std::begin(xPosArr), std::end(xPosArr), [&](int j) {return j == xPos; })) {
                std::unique_ptr<juce::AudioFormatReader> reader;
                reader.reset(audioProcessor.formatManager.createReaderFor(audioProcessor.impulseResponseArray[comboTypeID][mikTypeID][std::ceil(yPos / 20)][xPos]));
                irBuffer.setSize(reader->numChannels, reader->lengthInSamples);
                if (!reader->read(&irBuffer, 0, reader->lengthInSamples, 0, true, true)) { return; }

                sampleRate = reader->sampleRate;
                audioProcessor.interpIRSampleRate = sampleRate;

                audioProcessor.newIRAudioBuffer = irBuffer;
                audioProcessor.newIRReady.set(true);

                irfftComponent.loadedIRChanged(irBuffer, sampleRate);
                userIRLoaded = false;
                needIRUpdate.set(false);
                return;
            }

            // otherwise interpolate first on X axis for both Y values, then on Y axis from interpolated points on X axis, Y has to be rounded to 0 10 or 40 (recorded distances) and X to even values
            // 1. round Y to set values
            if (0 < yPos && yPos < 10) { yPosRoundDown = 0; yPosRoundUp = 10; }
            else if (10 < yPos && yPos < 40) { yPosRoundDown = 10; yPosRoundUp = 40; }
            else if (std::trunc(yPos) == yPos) { yPosRoundDown = yPos; yPosRoundUp = yPos; }
            // round X to even values
            xPosRoundUp = std::ceil(xPos);
            xPosRoundDown = std::floor(xPos);
            if (xPosRoundUp % 2 == 0 && xPosRoundDown != xPosRoundUp) { xPosRoundDown -= 1; } // when x = (1,2), (3,4), ...
            else if (xPosRoundUp % 2 != 0 && xPosRoundUp == xPosRoundDown) { xPosRoundDown -= 1; xPosRoundUp += 1; } // when x = 1, 3, 5...
            else if (xPosRoundDown != xPosRoundUp) { xPosRoundUp += 1; } // when x = (0,1), (2,3), ...

            // 2. find maximum distance: on X axis always 2, on Y axis 10 or 30
            XmaxDistance = 2;
            YmaxDistance = yPosRoundUp - yPosRoundDown;
            // 3. find the distance between X rounded down and current X, same for Y
            Xdistance = xPos - xPosRoundDown;
            Ydistance = yPos - yPosRoundDown;
            // 4. map said distance to <0,1> where 0 is xPosRoundDown and 1 is maxDistance
            XtransposedDistance = Xdistance / XmaxDistance;
            Ydistance == 0 ? YtransposedDistance = 0 : YtransposedDistance = Ydistance / YmaxDistance;

            // 5. interpolate X axis for both Y values (RoundUp & RoundDown)
            // (a * (1.0 - f)) + (b * f) where f = transposedDistance


            // impulseResponseArray[combo type][mic type][Y position == 0=0, 1=10, 2=40][X position] 
            std::unique_ptr<juce::AudioFormatReader> readerBL, readerBR, readerTL, readerTR; // bottom-left, bottom-right etc

            readerBL.reset(audioProcessor.formatManager.createReaderFor(audioProcessor.impulseResponseArray[comboTypeID][mikTypeID][std::ceil((double)yPosRoundDown / 20)][xPosRoundDown]));
            readerTL.reset(audioProcessor.formatManager.createReaderFor(audioProcessor.impulseResponseArray[comboTypeID][mikTypeID][std::ceil((double)yPosRoundUp / 20)][xPosRoundDown]));
            readerTR.reset(audioProcessor.formatManager.createReaderFor(audioProcessor.impulseResponseArray[comboTypeID][mikTypeID][std::ceil((double)yPosRoundUp / 20)][xPosRoundUp]));
            readerBR.reset(audioProcessor.formatManager.createReaderFor(audioProcessor.impulseResponseArray[comboTypeID][mikTypeID][std::ceil((double)yPosRoundDown / 20)][xPosRoundUp]));

            sampleRate = readerBL->sampleRate;
            audioProcessor.interpIRSampleRate = sampleRate;

            readerBL->read(&audioProcessor.audioBufferInterpBL, 0, readerBL->lengthInSamples, 0, true, true);
            readerBR->read(&audioProcessor.audioBufferInterpBR, 0, readerBR->lengthInSamples, 0, true, true);
            readerTL->read(&audioProcessor.audioBufferInterpTL, 0, readerTL->lengthInSamples, 0, true, true);
            readerTR->read(&audioProcessor.audioBufferInterpTR, 0, readerTR->lengthInSamples, 0, true, true);

            // check if both audioBuffers are equal length
            if (audioProcessor.audioBufferInterpBL.getNumChannels() != audioProcessor.audioBufferInterpBR.getNumChannels() || audioProcessor.audioBufferInterpBL.getNumSamples() != audioProcessor.audioBufferInterpBR.getNumSamples()) { return; }

            irBuffer.setSize(audioProcessor.audioBufferInterpBL.getNumChannels(), audioProcessor.audioBufferInterpBL.getNumSamples());

            float interpBottom = 0, interpTop = 0, interpValue = 0;

            for (int ch = 0; ch < audioProcessor.audioBufferInterpBL.getNumChannels(); ++ch) {
                for (int s = 0; s < audioProcessor.audioBufferInterpBL.getNumSamples(); ++s) {
                    interpBottom = audioProcessor.audioBufferInterpBL.getSample(ch, s) * (1.0 - XtransposedDistance) + (audioProcessor.audioBufferInterpBR.getSample(ch, s) * XtransposedDistance);
                    interpTop = audioProcessor.audioBufferInterpTL.getSample(ch, s) * (1.0 - XtransposedDistance) + (audioProcessor.audioBufferInterpTR.getSample(ch, s) * XtransposedDistance);
                    interpValue = (interpBottom * (1.0 - YtransposedDistance)) + (interpTop * YtransposedDistance);
                    audioProcessor.audioBufferInterpFin.setSample(ch, s, interpValue);
                }
            }
            irBuffer = audioProcessor.audioBufferInterpFin;
            
            // tells processor it can load new IR in next block
            audioProcessor.newIRAudioBuffer = irBuffer;
            audioProcessor.newIRReady.set(true);
            
            userIRLoaded = false;
            irfftComponent.loadedIRChanged(irBuffer, sampleRate);
            needIRUpdate.set(false);
        }
    }
}

void BasicEQAudioProcessorEditor::paint (juce::Graphics& g)
{
    using namespace juce;
    backgroundImage = ImageCache::getFromMemory(BinaryData::BackgroundImage_png, BinaryData::BackgroundImage_pngSize);
    g.drawImage(backgroundImage, getLocalBounds().toFloat(), RectanglePlacement::stretchToFit);
}

void BasicEQAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    // Padding from sides
    auto bounds = getLocalBounds();
    bounds.reduce(bounds.getWidth() * 0.01, bounds.getHeight() * 0.01);
    // Input/Output meters
    auto meterWidth = bounds.getWidth() * 0.062;
    //auto meterHeight = bounds.getHeight() * 0.2;
    auto inputMeterArea = bounds.removeFromLeft(meterWidth);
    inputMeterArea.removeFromRight(-meterWidth/12);
    inputMeterArea.removeFromTop(inputMeterArea.getHeight() * 0.02);
    auto inputMeterLabelArea = inputMeterArea.removeFromBottom(inputMeterArea.getHeight() * 0.122);
    inputMeterLabelArea.removeFromTop(inputMeterLabelArea.getHeight() * 0.73);
    helpBtn.setBounds(inputMeterLabelArea.removeFromLeft(inputMeterLabelArea.getWidth()*0.55));

    auto outputMeterArea = bounds.removeFromRight(meterWidth);
    outputMeterArea.removeFromLeft(-meterWidth / 11);
    outputMeterArea.removeFromTop(outputMeterArea.getHeight() * 0.02);
    auto outputMeterLabelArea = outputMeterArea.removeFromBottom(outputMeterArea.getHeight() * 0.122);
    
    inputMeterArea.removeFromLeft(inputMeterArea.getWidth() * 0.03);
    meterInLeft.setBounds(inputMeterArea.removeFromLeft(inputMeterArea.getWidth()*0.31));
    auto meterInGridArea = inputMeterArea.removeFromLeft(inputMeterArea.getWidth() * 0.58);
    inputDecibelGrid.setBounds(meterInGridArea);
    meterInRight.setBounds(inputMeterArea);

    //outputMeterArea.removeFromLeft(outputMeterArea.getWidth() * 0.11);
    meterOutLeft.setBounds(outputMeterArea.removeFromLeft(outputMeterArea.getWidth() * 0.295));
    auto meterOutGridArea = outputMeterArea.removeFromLeft(outputMeterArea.getWidth() * 0.55);
    outputDecibelGrid.setBounds(meterOutGridArea);
    meterOutRight.setBounds(outputMeterArea.removeFromLeft(outputMeterArea.getWidth() * 0.9));

    // Main knobs
    auto oldHeight = bounds.getHeight();
    auto mainKnobsArea = bounds.removeFromBottom(oldHeight * 0.36).reduced(bounds.getWidth() * 0.02, 0.0);
    auto knobLabelArea = mainKnobsArea.removeFromBottom(mainKnobsArea.getHeight() * 0.28);
    knobLabelArea.removeFromBottom(knobLabelArea.getHeight() * 0.3);
    fontSize = knobLabelArea.getHeight()*0.6;

    for (auto* label : getLabels())
    {
        label->setFont(lnf.getCustomFont().withHeight(fontSize).withExtraKerningFactor(0.02));
    }
    irBypassLabel.setFont(lnf.getCustomFont().withHeight(fontSize).withExtraKerningFactor(0.02));
    
    //yellow part width
    auto yellowBackgroundWidth = mainKnobsArea.getWidth();
    mainKnobsArea.removeFromLeft(yellowBackgroundWidth * 0.05);
    mainKnobsArea.removeFromRight(yellowBackgroundWidth * 0.05); // this should center all the knobs relative to main plugin body
    mainKnobsArea.removeFromTop(mainKnobsArea.getHeight() * 0.07);

    float spaceBetweenKnobs = (yellowBackgroundWidth / 1384.f) * 13; // 7 spaces, should be 37px when max size
    float knobSize = (yellowBackgroundWidth / 1384.f) * 169; // knob size is 169px
    float switchSize = (yellowBackgroundWidth / 1384.f) * 87;// switch size is 87 x 169 px
    float bypassSize = (yellowBackgroundWidth / 1384.f) * 52;// bypass size is 52 px
    
    ampBypassButton.setBounds(mainKnobsArea.removeFromLeft(bypassSize).reduced(0,mainKnobsArea.getHeight()*0.355));
    mainKnobsArea.removeFromLeft(spaceBetweenKnobs);
    inputGainSlider.setBounds(mainKnobsArea.removeFromLeft(knobSize));
    mainKnobsArea.removeFromLeft(spaceBetweenKnobs);
    driveSlider.setBounds(mainKnobsArea.removeFromLeft(knobSize));
    mainKnobsArea.removeFromLeft(spaceBetweenKnobs);
    auto ampTypeArea = mainKnobsArea.removeFromLeft(switchSize);
    ampTypeSwitch.setBounds(ampTypeArea);
    mainKnobsArea.removeFromLeft(spaceBetweenKnobs);
    lowShelfSlider.setBounds(mainKnobsArea.removeFromLeft(knobSize));
    mainKnobsArea.removeFromLeft(spaceBetweenKnobs);
    midPeakSlider.setBounds(mainKnobsArea.removeFromLeft(knobSize));
    mainKnobsArea.removeFromLeft(spaceBetweenKnobs);
    highShelfSlider.setBounds(mainKnobsArea.removeFromLeft(knobSize));
    mainKnobsArea.removeFromLeft(spaceBetweenKnobs);
    outputGainSlider.setBounds(mainKnobsArea.removeFromLeft(knobSize));

    knobLabelArea.removeFromLeft(spaceBetweenKnobs * 3);
    ampBypassLabel.setBounds(knobLabelArea.removeFromLeft(bypassSize * 1.84));
    inputGainLabel.setBounds(knobLabelArea.removeFromLeft(knobSize));
    knobLabelArea.removeFromLeft(spaceBetweenKnobs);
    driveLabel.setBounds(knobLabelArea.removeFromLeft(knobSize));
    knobLabelArea.removeFromLeft(spaceBetweenKnobs);
    ampTypeLabel.setBounds(knobLabelArea.removeFromLeft(switchSize));
    knobLabelArea.removeFromLeft(spaceBetweenKnobs);
    lowLabel.setBounds(knobLabelArea.removeFromLeft(knobSize));
    knobLabelArea.removeFromLeft(spaceBetweenKnobs);
    midLabel.setBounds(knobLabelArea.removeFromLeft(knobSize));
    knobLabelArea.removeFromLeft(spaceBetweenKnobs);
    highLabel.setBounds(knobLabelArea.removeFromLeft(knobSize));
    knobLabelArea.removeFromLeft(spaceBetweenKnobs);
    outputGainLabel.setBounds(knobLabelArea.removeFromLeft(knobSize));
    
    // XY Pad
    auto xyPadArea = bounds.removeFromLeft(bounds.getWidth() * 0.299);
    xyPadArea.removeFromTop(xyPadArea.getHeight() * 0.21);
    auto labelHeight = xyPadArea.getHeight() * 0.138;
    auto labelPadding = xyPadArea.getHeight() * 0.085;
    auto xyPadNameArea = xyPadArea.removeFromBottom(labelHeight);
    xyPadArea.removeFromBottom(labelPadding);
    xyPadArea.removeFromLeft(xyPadArea.getWidth() * 0.295);
    xyPad.setBounds(xyPadArea);
    xyPadNameArea.removeFromLeft(xyPadNameArea.getWidth() - xyPadArea.getWidth());
    xyPadLabel.setBounds(xyPadNameArea);

    // IR response
    auto responseArea = bounds.removeFromRight(bounds.getWidth() * 0.635);
    responseArea.removeFromTop(bounds.getHeight() * 0.17);
    labelPadding = bounds.getHeight() * 0.03;
    auto irCompNameArea = responseArea.removeFromBottom(labelHeight);
    responseArea.removeFromBottom(labelPadding);
    responseArea.removeFromRight(responseArea.getWidth() * 0.184);
    irfftComponent.setBounds(responseArea);
    irCompNameArea.removeFromRight( irCompNameArea.getWidth() - responseArea.getWidth() );
    irfftLabel.setBounds(irCompNameArea);
    responseArea.removeFromBottom(responseArea.getHeight() * 0.8);
    responseArea.removeFromLeft(responseArea.getWidth() * 0.5);
    irBypassLabel.setBounds(responseArea.removeFromLeft(responseArea.getWidth() * 0.8));
    responseArea.removeFromRight(responseArea.getWidth() * 0.2);
    auto bypassBtnArea = responseArea.withSizeKeepingCentre(responseArea.getWidth(), responseArea.getWidth());
    irBypassButton.setBounds(bypassBtnArea);

    // IR choice buttons, load button
    bounds.removeFromLeft(bounds.getWidth() * 0.037);
    bounds.reduce(bounds.getWidth()*0.1, bounds.getHeight()*0.1);
    micButtonsLabel.setBounds(bounds.removeFromBottom(labelHeight));
    // button has 15px of "empty" space until main body of button from all sides (95x95 but button 65x65)
    // ir load button has the same padding (174x95 but 144x65 main body)
    // thus label padding -= 15px
    labelPadding -= (yellowBackgroundWidth / 1384.f) * 15;
    bounds.removeFromBottom(labelPadding);
    auto btnHeight = (yellowBackgroundWidth / 1384.f) * 95;
    auto micBtnBounds = bounds.removeFromBottom(btnHeight);
    aMicButton.setBounds(micBtnBounds.removeFromLeft(micBtnBounds.getWidth()*0.33));
    bMicButton.setBounds(micBtnBounds.removeFromLeft(micBtnBounds.getWidth() * 0.5));
    cMicButton.setBounds(micBtnBounds);

    cabButtonsLabel.setBounds(bounds.removeFromBottom(labelHeight));
    auto cabBtnBounds = bounds.removeFromBottom(btnHeight);
    aCabButton.setBounds(cabBtnBounds.removeFromLeft(cabBtnBounds.getWidth() * 0.33));
    bCabButton.setBounds(cabBtnBounds.removeFromLeft(cabBtnBounds.getWidth() * 0.5));
    cCabButton.setBounds(cabBtnBounds);

    //bounds.removeFromBottom(labelHeight*0.4);
    //loadBtn.setBounds(bounds.removeFromBottom(btnHeight));

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
        &meterOutRight,
        &driveSlider,
        &lowShelfSlider,
        &midPeakSlider,
        &highShelfSlider,
        &ampBypassButton,
        &xyPad,
        &aCabButton,
        &bCabButton,
        &cCabButton,
        &aMicButton,
        &bMicButton,
        &cMicButton,
        &ampBypassLabel,
        &inputGainLabel,
        &driveLabel,
        &ampTypeLabel,
        &lowLabel,
        &midLabel,
        &highLabel,
        &outputGainLabel,
        &ampTypeSwitch,
        &xyPadLabel,
        &irfftLabel,
        &micButtonsLabel,
        &cabButtonsLabel,
        &irBypassLabel,
        &inputDecibelGrid,
        &outputDecibelGrid,
        &helpBtn
    };
}

std::vector<juce::Label*> BasicEQAudioProcessorEditor::getLabels()
{
    
    return {
        &ampBypassLabel,
        &inputGainLabel,
        &driveLabel,
        &ampTypeLabel,
        &lowLabel,
        &midLabel,
        &highLabel,
        &outputGainLabel,
        &xyPadLabel,
        &irfftLabel,
        &micButtonsLabel,
        &cabButtonsLabel,
    };
}
