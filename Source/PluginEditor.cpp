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
//osBypassButtonAttachment(audioProcessor.apvts, "Oversampling Bypassed", osBypassButton),
irBypassButtonAttachment(audioProcessor.apvts, "IR Bypassed", irBypassButton)/*,
polettiAmpTypeButtonAttachment(audioProcessor.apvts, "IR Bypassed", polettiAmpTypeButton),
yamahaAmpTypeButtonAttachment(audioProcessor.apvts, "IR Bypassed", yamahaAmpTypeButton),
wavefoldAmpTypeButtonAttachment(audioProcessor.apvts, "IR Bypassed", wavefoldAmpTypeButton)*/
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.


    /*peakFreqSlider.labels.add({ 0.f, "10Hz" });
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
    highCutSlopeSlider.labels.add({ 1.f, "48" });*/
    //xPosSlider.labels.add({ 0.f, "0cm" });
    //xPosSlider.labels.add({ 1.f, "10cm" });
    xPosSlider.name = "X Position";
    //yPosSlider.labels.add({ 0.f, "0cm" });
    //yPosSlider.labels.add({ 1.f, "40cm" });
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
    //comboTypeBox.setSelectedItemIndex(1);
    comboTypeBox.onChange = [this]() { 
        //DBG("changed combo");
        //juce::AudioBuffer<float> irBuffer;
        //int sampleRate = 0;
        ////audioProcessor.updateLoadedIR(irBuffer, sampleRate, comboTypeBox.getSelectedId() - 1, mikTypeBox.getSelectedId() - 1, yPosSlider.getValue(), xPosSlider.getValue());
        //userIRLoaded = false; 
        //irfftComponent.loadedIRChanged(irBuffer, sampleRate);
        needIRUpdate.set(true);
        };

    aCabButton.setRadioGroupId(RadioButtonIDs::CabTypeButtons);
    aCabButton.setClickingTogglesState(true);
    aCabButton.onClick = [this]() { comboTypeBox.setSelectedItemIndex(0); };

    bCabButton.setRadioGroupId(RadioButtonIDs::CabTypeButtons);
    bCabButton.setClickingTogglesState(true);
    bCabButton.onClick = [this]() { comboTypeBox.setSelectedItemIndex(1); };

    cCabButton.setRadioGroupId(RadioButtonIDs::CabTypeButtons);
    cCabButton.setClickingTogglesState(true);
    cCabButton.onClick = [this]() { comboTypeBox.setSelectedItemIndex(2); };

    //auto x = comboTypeBox.getSelectedId();
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
    //mikTypeBox.setSelectedItemIndex(1);
    mikTypeBox.onChange = [this]() {
        //DBG("changed mic");
        //juce::AudioBuffer<float> irBuffer;
        //int sampleRate = 0;
        ////audioProcessor.updateLoadedIR(irBuffer, sampleRate, comboTypeBox.getSelectedId() - 1, mikTypeBox.getSelectedId() - 1, yPosSlider.getValue(), xPosSlider.getValue());
        //userIRLoaded = false;
        //irfftComponent.loadedIRChanged(irBuffer, sampleRate);
        needIRUpdate.set(true);
        };

    aMicButton.setRadioGroupId(RadioButtonIDs::MicTypeButtons);
    aMicButton.setClickingTogglesState(true);
    aMicButton.onClick = [this]() { mikTypeBox.setSelectedItemIndex(0); };

    bMicButton.setRadioGroupId(RadioButtonIDs::MicTypeButtons);
    bMicButton.setClickingTogglesState(true);
    bMicButton.onClick = [this]() { mikTypeBox.setSelectedItemIndex(1); };

    cMicButton.setRadioGroupId(RadioButtonIDs::MicTypeButtons);
    cMicButton.setClickingTogglesState(true);
    cMicButton.onClick = [this]() { mikTypeBox.setSelectedItemIndex(2); };

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

    ampTypeBox.addItem("Poletti", 1);
    ampTypeBox.addItem("Yamaha", 2);
    ampTypeBox.addItem("WaveFolder", 3);
    //ampTypeBox.setSelectedItemIndex(0);
    ampTypeBox.onChange = [this]() {
        //audioProcessor.ampDrive.simTypeChanged(ampTypeBox.getSelectedId());
        audioProcessor.ampSim.ampType = static_cast<AmpTypeEnum>(ampTypeBox.getSelectedId() - 1);
        DBG("ComboBox: ");
        DBG(audioProcessor.ampSim.ampType);

        };


    polettiAmpTypeButton.setRadioGroupId(RadioButtonIDs::AmpTypeButtons);
    polettiAmpTypeButton.setClickingTogglesState(true);
    polettiAmpTypeButton.onClick = [this]() { /*audioProcessor.ampSim.ampType = AmpTypeEnum::Poletti;*/ 
        if (!polettiAmpTypeButton.getToggleState()) return;
        ampTypeBox.setSelectedItemIndex(0);
        //getLookAndFeel().setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::red);
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
        DBG("Button 1 pressed: ");
        DBG(ampTypeBox.getSelectedId());
        };

    yamahaAmpTypeButton.setRadioGroupId(RadioButtonIDs::AmpTypeButtons);
    yamahaAmpTypeButton.setClickingTogglesState(true);
    yamahaAmpTypeButton.onClick = [this]() { /*audioProcessor.ampSim.ampType = AmpTypeEnum::Yamaha;*/ 
        if (!yamahaAmpTypeButton.getToggleState()) return;
        ampTypeBox.setSelectedItemIndex(1);
        driveSlider.setSliderColor(juce::Colours::blue);
        driveSlider.repaint();
        lowShelfSlider.setSliderColor(juce::Colours::blue);
        lowShelfSlider.repaint();
        midPeakSlider.setSliderColor(juce::Colours::blue);
        midPeakSlider.repaint();
        highShelfSlider.setSliderColor(juce::Colours::blue);
        highShelfSlider.repaint();
        inputGainSlider.setSliderColor(juce::Colours::blue);
        inputGainSlider.repaint();
        outputGainSlider.setSliderColor(juce::Colours::blue);
        outputGainSlider.repaint();
        xyPad.thumb.setThumbColor(juce::Colours::blue);
        xyPad.thumb.repaint();
        //getLookAndFeel().setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::red);
        DBG("Button 2 pressed: ");
        DBG(ampTypeBox.getSelectedId());
        };
    
    wavefoldAmpTypeButton.setRadioGroupId(RadioButtonIDs::AmpTypeButtons);
    wavefoldAmpTypeButton.setClickingTogglesState(true);
    wavefoldAmpTypeButton.onClick = [this]() { /*audioProcessor.ampSim.ampType = AmpTypeEnum::WaveFolder;*/ 
        if (!wavefoldAmpTypeButton.getToggleState()) return;
        ampTypeBox.setSelectedItemIndex(2);
        driveSlider.setSliderColor(juce::Colours::green);
        driveSlider.repaint();
        lowShelfSlider.setSliderColor(juce::Colours::green);
        lowShelfSlider.repaint();
        midPeakSlider.setSliderColor(juce::Colours::green);
        midPeakSlider.repaint();
        highShelfSlider.setSliderColor(juce::Colours::green);
        highShelfSlider.repaint();
        inputGainSlider.setSliderColor(juce::Colours::green);
        inputGainSlider.repaint();
        outputGainSlider.setSliderColor(juce::Colours::green);
        outputGainSlider.repaint();
        xyPad.thumb.setThumbColor(juce::Colours::green);
        xyPad.thumb.repaint();
        //getLookAndFeel().setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::red);
        DBG("Button 3 pressed: ");
        DBG(ampTypeBox.getSelectedId());
        };

    switch (audioProcessor.ampSim.ampType)
    {
    case AmpTypeEnum::Poletti:
        polettiAmpTypeButton.triggerClick();
        break;
    case AmpTypeEnum::Yamaha:
        yamahaAmpTypeButton.triggerClick();
        break;
    case AmpTypeEnum::WaveFolder:
        wavefoldAmpTypeButton.triggerClick();
        break;
    }

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
    //driveSlider.setLookAndFeel(&lnf);
    //lowShelfSlider.setLookAndFeel(&lnf);
    //midPeakSlider.setLookAndFeel(&lnf);
    //highShelfSlider.setLookAndFeel(&lnf);

    comboTypeBoxAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(p.apvts, "Combo Type", comboTypeBox);
    mikTypeBoxAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(p.apvts, "Mic Type", mikTypeBox);
    ampTypeBoxAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(p.apvts, "Amp Type", ampTypeBox);

    yPosSlider.onValueChange = [this]() {
        ////DBG("changed yPos to " << yPosSlider.getValue());
        //juce::AudioBuffer<float> irBuffer;
        //int sampleRate = 0;
        //audioProcessor.updateLoadedIR(irBuffer, sampleRate, comboTypeBox.getSelectedId() - 1, mikTypeBox.getSelectedId() - 1, yPosSlider.getValue(), xPosSlider.getValue());
        //userIRLoaded = false;
        ////if (tempFile.getFile().getSize() == 0) return;
        //irfftComponent.loadedIRChanged(irBuffer, sampleRate);
        needIRUpdate.set(true);
    };
    xPosSlider.onValueChange = [this]() { 
        ////DBG("changed xPos to " << xPosSlider.getValue());
        //juce::AudioBuffer<float> irBuffer;
        //int sampleRate = 0;
        //audioProcessor.updateLoadedIR(irBuffer, sampleRate, comboTypeBox.getSelectedId() - 1, mikTypeBox.getSelectedId() - 1, yPosSlider.getValue(), xPosSlider.getValue());
        //userIRLoaded = false;
        ////if (tempFile.getFile().getSize() == 0) return;
        //irfftComponent.loadedIRChanged(irBuffer, sampleRate);
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

    setSize (1674, 752);
    setResizable(true, true);
    setResizeLimits( 418.5, 188, 1674, 752);
    getConstrainer()->setFixedAspectRatio(1674.f / 752.f);

    startTimer(0, 33);
    startTimer(1, 50);
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
    //driveSlider.setLookAndFeel(nullptr);
    //lowShelfSlider.setLookAndFeel(nullptr);
    //midPeakSlider.setLookAndFeel(nullptr);
    //highShelfSlider.setLookAndFeel(nullptr);
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
        if (needIRUpdate.get()) {
            juce::AudioBuffer<float> irBuffer;
            int sampleRate = 0;
            float yPos = yPosSlider.getValue(), 
                  xPos = xPosSlider.getValue();
            int comboTypeID = comboTypeBox.getSelectedId() - 1,
                mikTypeID = mikTypeBox.getSelectedId() - 1;
            // should compute interpolation here, then pass interpolated IR buffer to audioprocessor, where it is checked each buffer, if new IR is ready to be loaded -> need another atomic bool
            audioProcessor.newIRReady.set(false);

            // interpolate here
            int yPosRoundDown = 0, yPosRoundUp = 0, xPosRoundDown = 0, xPosRoundUp = 0;
            float XmaxDistance = 0, Xdistance = 0, XtransposedDistance = 0, YmaxDistance = 0, Ydistance = 0, YtransposedDistance = 0;
            int yPosArr[3] = { 0, 10, 40 };
            int xPosArr[6] = { 0, 2, 4, 6, 8, 10 };
            // formatManager takes a file (wav in our case), returns AudioBuffer (could return float array tho)


            // if Y = {0,10,40} and X = {0,2,4,..,10}, no need to interpolate
            if (std::any_of(std::begin(yPosArr), std::end(yPosArr), [&](int i) { return i == yPos; }) && std::any_of(std::begin(xPosArr), std::end(xPosArr), [&](int j) {return j == xPos; })) {
                // load IR, stereo, trimmed, normalized, size 0 = original IR size
                //irLoader.loadImpulseResponse(impulseResponseArray[comboTypeID][mikTypeID][std::ceil((double)yPos / 20)][xPos], juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::yes, 0, juce::dsp::Convolution::Normalise::yes);
                // TODO: file -> audiobuffer, get samplerate from file
                std::unique_ptr<juce::AudioFormatReader> reader;
                reader.reset(audioProcessor.formatManager.createReaderFor(audioProcessor.impulseResponseArray[comboTypeID][mikTypeID][std::ceil(yPos / 20)][xPos]));
                irBuffer.setSize(reader->numChannels, reader->lengthInSamples);
                if (!reader->read(&irBuffer, 0, reader->lengthInSamples, 0, true, true)) { DBG("Reader for non-interpolated file failed to write to buffer"); }

                sampleRate = reader->sampleRate;

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


            // impulseResponseArray[typ komba][typ mikrofonu][pozice Y - 0=0, 1=10, 2=40]  [pozice X] 
            std::unique_ptr<juce::AudioFormatReader> readerBL, readerBR, readerTL, readerTR; // bottom-left, bottom-right etc

            readerBL.reset(audioProcessor.formatManager.createReaderFor(audioProcessor.impulseResponseArray[comboTypeID][mikTypeID][std::ceil((double)yPosRoundDown / 20)][xPosRoundDown]));
            readerTL.reset(audioProcessor.formatManager.createReaderFor(audioProcessor.impulseResponseArray[comboTypeID][mikTypeID][std::ceil((double)yPosRoundUp / 20)][xPosRoundDown]));
            readerTR.reset(audioProcessor.formatManager.createReaderFor(audioProcessor.impulseResponseArray[comboTypeID][mikTypeID][std::ceil((double)yPosRoundUp / 20)][xPosRoundUp]));
            readerBR.reset(audioProcessor.formatManager.createReaderFor(audioProcessor.impulseResponseArray[comboTypeID][mikTypeID][std::ceil((double)yPosRoundDown / 20)][xPosRoundUp]));

            /*juce::AudioBuffer<float> audioBufferBL, audioBufferBR, audioBufferTL, audioBufferTR, audioBufferInterpBottom, audioBufferInterpTop, audioBufferInterp;

            audioBufferBL.setSize(readerBL->numChannels, readerBL->lengthInSamples);
            audioBufferBR.setSize(readerBR->numChannels, readerBR->lengthInSamples);
            audioBufferTL.setSize(readerTL->numChannels, readerTL->lengthInSamples);
            audioBufferTR.setSize(readerTR->numChannels, readerTR->lengthInSamples);*/

            sampleRate = readerBL->sampleRate;
            audioProcessor.interpIRSampleRate = sampleRate;

            readerBL->read(&audioProcessor.audioBufferInterpBL, 0, readerBL->lengthInSamples, 0, true, true);
            readerBR->read(&audioProcessor.audioBufferInterpBR, 0, readerBR->lengthInSamples, 0, true, true);
            readerTL->read(&audioProcessor.audioBufferInterpTL, 0, readerTL->lengthInSamples, 0, true, true);
            readerTR->read(&audioProcessor.audioBufferInterpTR, 0, readerTR->lengthInSamples, 0, true, true);

            // check if both audioBuffers are equal length
            if (audioProcessor.audioBufferInterpBL.getNumChannels() != audioProcessor.audioBufferInterpBR.getNumChannels() || audioProcessor.audioBufferInterpBL.getNumSamples() != audioProcessor.audioBufferInterpBR.getNumSamples()) { DBG("Not the same no of channels or samples"); }

            /*audioBufferInterpBottom.setSize(audioBufferBL.getNumChannels(), audioBufferBL.getNumSamples());
            audioBufferInterpTop.setSize(audioBufferBL.getNumChannels(), audioBufferBL.getNumSamples());
            audioBufferInterp.setSize(audioBufferBL.getNumChannels(), audioBufferBL.getNumSamples());*/
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
            
            //audioProcessor.updateLoadedIR(irBuffer, sampleRate, comboTypeBox.getSelectedId() - 1, mikTypeBox.getSelectedId() - 1, yPosSlider.getValue(), xPosSlider.getValue());
            userIRLoaded = false;
            //if (tempFile.getFile().getSize() == 0) return;
            irfftComponent.loadedIRChanged(irBuffer, sampleRate);
            needIRUpdate.set(false);
        }
    }
}

void BasicEQAudioProcessorEditor::paint (juce::Graphics& g)
{
    using namespace juce;
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll (Colours::black);
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
    // Input/Output meters and sliders
    auto meterWidth = bounds.getWidth() * 0.062;
    auto meterHeight = bounds.getHeight() * 0.2;
    auto inputMeterArea = bounds.removeFromLeft(meterWidth);
    inputMeterArea.removeFromTop(inputMeterArea.getHeight() * 0.02);
    auto inputMeterLabelArea = inputMeterArea.removeFromBottom(inputMeterArea.getHeight() * 0.12);
    auto outputMeterArea = bounds.removeFromRight(meterWidth);
    outputMeterArea.removeFromTop(outputMeterArea.getHeight() * 0.02);
    auto outputMeterLabelArea = outputMeterArea.removeFromBottom(outputMeterArea.getHeight() * 0.12);
    //auto inputGainArea = inputMeterArea.removeFromBottom(meterHeight);
    //auto outputGainArea = outputMeterArea.removeFromBottom(meterHeight);
    //inputGainSlider.setBounds(inputGainArea.reduced(0, inputGainArea.getHeight()*0.05));
    //outputGainSlider.setBounds(outputGainArea.reduced(0, outputGainArea.getHeight() * 0.05));
    meterInLeft.setBounds(inputMeterArea.removeFromLeft(inputMeterArea.getWidth()*0.38));
    inputMeterArea.removeFromLeft(inputMeterArea.getWidth() * 0.3);
    meterInRight.setBounds(inputMeterArea.removeFromLeft(inputMeterArea.getWidth()*0.81));
    outputMeterArea.removeFromLeft(outputMeterArea.getWidth() * 0.1);
    meterOutLeft.setBounds(outputMeterArea.removeFromLeft(outputMeterArea.getWidth() * 0.38));
    outputMeterArea.removeFromLeft(outputMeterArea.getWidth() * 0.36);
    meterOutRight.setBounds(outputMeterArea.removeFromLeft(outputMeterArea.getWidth()));

    // Space for buffer if it will be implemented
    //auto bufferArea = bounds.removeFromTop(bounds.getHeight() * 0.1);
    //auto buttonWidth = bufferArea.getWidth() * 0.33;
    
    //osBypassButton.setBounds(bufferArea.removeFromLeft(buttonWidth));
    

    // Main knobs - gain sliders in with knobs, maybe smaller size
    auto mainKnobsArea = bounds.removeFromBottom(bounds.getHeight() * 0.4).reduced(bounds.getWidth() * 0.02, bounds.getHeight() * 0.1);
    driveSlider.setBounds(mainKnobsArea);
    //ampBypassButton.setBounds(mainKnobsArea.removeFromLeft(mainKnobsArea.getWidth()/20));
    //auto knobWidth = mainKnobsArea.getWidth() / 5;
    //driveSlider.setBounds(mainKnobsArea.removeFromLeft(knobWidth));
    //auto ampTypeArea = mainKnobsArea.removeFromLeft(knobWidth);
    //polettiAmpTypeButton.setBounds(ampTypeArea.removeFromTop(ampTypeArea.getHeight() * 0.33));
    //yamahaAmpTypeButton.setBounds(ampTypeArea.removeFromTop(ampTypeArea.getHeight() * 0.5));
    //wavefoldAmpTypeButton.setBounds(ampTypeArea);
    ////ampTypeBox.setBounds(mainKnobsArea.removeFromLeft(knobWidth));
    //lowShelfSlider.setBounds(mainKnobsArea.removeFromLeft(knobWidth));
    //midPeakSlider.setBounds(mainKnobsArea.removeFromLeft(knobWidth));
    //highShelfSlider.setBounds(mainKnobsArea.removeFromLeft(knobWidth));

    // bounds are now only top "half"
    //auto xyPadArea = bounds.removeFromLeft(bounds.getWidth() * 0.25).reduced(bounds.getWidth() * 0.02, bounds.getHeight() * 0.02);
    //auto irChoicesArea = bounds.removeFromLeft(bounds.getWidth() * 0.33).reduced(bounds.getWidth() * 0.02, bounds.getHeight() * 0.02);
    //auto responseArea = bounds.reduced(bounds.getWidth() * 0.02, bounds.getHeight() * 0.02);

    //xPosSlider.setBounds(xyPadArea.removeFromTop(xyPadArea.getHeight()*0.5).reduced(xyPadArea.getWidth()*0.05));
    //yPosSlider.setBounds(xyPadArea.reduced(xyPadArea.getWidth() * 0.05));
    auto xyPadArea = bounds.removeFromLeft(bounds.getWidth() * 0.299);
    xyPadArea.removeFromTop(xyPadArea.getHeight() * 0.21);
    xyPadArea.removeFromBottom(xyPadArea.getHeight() * 0.138);
    xyPadArea.removeFromLeft(xyPadArea.getWidth() * 0.295);
    xyPad.setBounds(xyPadArea);

    auto responseArea = bounds.removeFromRight(bounds.getWidth() * 0.628);
    responseArea.removeFromTop(bounds.getHeight() * 0.195);
    responseArea.removeFromBottom(bounds.getHeight() * 0.095);
    responseArea.removeFromRight(responseArea.getWidth() * 0.195);
    irfftComponent.setBounds(responseArea);





    //irBypassButton.setBounds(irChoicesArea.removeFromTop(irChoicesArea.getHeight() * 0.25).reduced(irChoicesArea.getWidth() * 0.09));
    //loadBtn.setBounds(irChoicesArea.removeFromTop(irChoicesArea.getHeight() * 0.33).reduced(irChoicesArea.getWidth() * 0.02, irChoicesArea.getHeight() * 0.02));
    //mikTypeBox.setBounds(irChoicesArea.removeFromTop(irChoicesArea.getHeight() * 0.5).reduced(irChoicesArea.getWidth() * 0.02, irChoicesArea.getHeight() * 0.02));
    //comboTypeBox.setBounds(irChoicesArea.reduced(irChoicesArea.getWidth() * 0.02, irChoicesArea.getHeight() * 0.02));

    /*auto cabBtnsArea = irChoicesArea.removeFromTop(irChoicesArea.getHeight() * 0.5).reduced(irChoicesArea.getWidth() * 0.02, irChoicesArea.getHeight() * 0.02);
    auto cabBtnWidth = cabBtnsArea.getWidth() * 0.33;
    aCabButton.setBounds(cabBtnsArea.removeFromLeft(cabBtnWidth));
    bCabButton.setBounds(cabBtnsArea.removeFromLeft(cabBtnWidth));
    cCabButton.setBounds(cabBtnsArea);

    auto micBtnsArea = irChoicesArea.reduced(irChoicesArea.getWidth() * 0.02, irChoicesArea.getHeight() * 0.02);
    aMicButton.setBounds(micBtnsArea.removeFromLeft(cabBtnWidth));
    bMicButton.setBounds(micBtnsArea.removeFromLeft(cabBtnWidth));
    cMicButton.setBounds(micBtnsArea);*/

    //irfftComponent.setBounds(responseArea);

    //auto responseArea = bounds.removeFromTop(bounds.getHeight() * 0.4);

    //auto responseCurveComponentBounds = responseArea.removeFromRight(responseArea.getWidth() * 0.5);
    //responseCurveComponent.setBounds(responseCurveComponentBounds.reduced(responseCurveComponentBounds.getWidth()*0.05, 0).removeFromBottom(responseCurveComponentBounds.getHeight()*0.95));

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


    /*auto GainArea = bounds;
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
    irfftComponent.setBounds(IrFFTComponentBounds.reduced(IrFFTComponentBounds.getWidth() * 0.05, 0).removeFromBottom(IrFFTComponentBounds.getHeight() * 0.95));*/

    //bounds.removeFromTop(5);
    //================================================================================================================
    //EQ just commented out for testing
    /*auto EQArea = bounds.removeFromRight(bounds.getWidth());
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
    peakQualitySlider.setBounds(EQArea);*/
    //================================================================================================================
    //ToneStack
    /*auto ToneStackArea = bounds.removeFromRight(bounds.getWidth());
    toneStackLowSlider.setBounds(ToneStackArea.removeFromLeft(ToneStackArea.getWidth() * 0.33));
    toneStackMidSlider.setBounds(ToneStackArea.removeFromLeft(ToneStackArea.getWidth() * 0.5));
    toneStackHighSlider.setBounds(ToneStackArea);*/


    //================================================================================================================


    //auto loadBtnArea = IRArea;
    //loadBtn.setBounds(loadBtnArea.removeFromLeft(loadBtnArea.getWidth()*0.6).removeFromRight(loadBtnArea.getWidth()*0.5).removeFromBottom(loadBtnArea.getHeight()*0.9).removeFromTop(loadBtnArea.getHeight()*0.3));
    //irNameLabel.setBounds(loadBtnArea);

    //GainArea.removeFromTop(GainArea.getHeight() * 0.5);
    //GainArea.removeFromTop(GainArea.getHeight() * 0.6);
    //GainArea.reduce(GainArea.getWidth() * 0.02, 0);
    
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
        &meterOutRight,
        &driveSlider,
        &lowShelfSlider,
        &midPeakSlider,
        &highShelfSlider,
        //&osBypassButton,
        &ampBypassButton,
        //&symLPLNSlider,
        //&asymPosLPSlider,
        //&asymPosLNSlider,
        //&asymNegLPSlider,
        //&asymNegLNSlider,
        //&ampBypassButton,
        &ampTypeBox,
        &xyPad,
        &polettiAmpTypeButton,
        &yamahaAmpTypeButton,
        &wavefoldAmpTypeButton,
        &aCabButton,
        &bCabButton,
        &cCabButton,
        &aMicButton,
        &bMicButton,
        &cMicButton
    };
}

