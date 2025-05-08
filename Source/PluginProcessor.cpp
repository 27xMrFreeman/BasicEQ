/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

    TODO:
    setup valuetree to save previously loaded file, load that file to IRLoader in setStateInformation()

    load files based on position of xPositionSlider and yPositionSlider

    add drop down menu to choose type of cab, load files based on that

    add FFT analysis of loaded IR

    try to interpolate between mik positions and maybe even cab types

  ==============================================================================
*/

//#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BasicEQAudioProcessor::BasicEQAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    formatManager.registerBasicFormats();
}

BasicEQAudioProcessor::~BasicEQAudioProcessor()
{
}

//==============================================================================
const juce::String BasicEQAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BasicEQAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BasicEQAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool BasicEQAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double BasicEQAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BasicEQAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int BasicEQAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BasicEQAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String BasicEQAudioProcessor::getProgramName (int index)
{
    return {};
}

void BasicEQAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void BasicEQAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    juce::dsp::ProcessSpec spec;

    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 2;
    spec.sampleRate = sampleRate;
    
    bufferBPContour.setSize(2, samplesPerBlock);
    bufferHPContour.setSize(2, samplesPerBlock);
    // filter design according to Will Pirkle Addendum chapter A19.26.3
    //==============================================================================================================================================
    // AMP + GAIN STAGING
    preampGain.reset();
    preampGain.prepare(spec);
    preampGain.setGainDecibels(0);
    ampSim.polettiDistortion.posProcessorChain.get<posAsymWaveShaper>().asymPosLP = 23.6f;
    ampSim.polettiDistortion.posProcessorChain.get<posAsymWaveShaper>().asymPosLN = 0.5f;
    ampSim.polettiDistortion.negProcessorChain.get<posAsymWaveShaper>().asymNegLP = 0.5f;
    ampSim.polettiDistortion.negProcessorChain.get<posAsymWaveShaper>().asymNegLN = 23.6f;
    ampSim.polettiDistortion.posProcessorChain.get<posSymWaveShaper>().symLPLN = 1.1f;
    ampSim.polettiDistortion.negProcessorChain.get<posSymWaveShaper>().symLPLN = 1.1f;
    ampSim.prepare(spec);
    postampGain.reset();
    postampGain.prepare(spec);
    postampGain.setGainDecibels(0);
    //==============================================================================================================================================
    // TONE STACK
    toneStack.prepare(spec);
    //==============================================================================================================================================
    // INPUT OUTPUT GAIN
    inputGain.reset();
    inputGain.prepare(spec);
    inputGain.setGainDecibels(0);
    outputGain.reset();
    outputGain.prepare(spec);
    outputGain.setGainDecibels(0);
    //==============================================================================================================================================
    spec.numChannels = 1;

    rmsLevelInputLeft.reset(sampleRate, 0.2);
    rmsLevelInputRight.reset(sampleRate, 0.1);
    rmsLevelInputLeft.setCurrentAndTargetValue(-100.f);
    rmsLevelInputRight.setCurrentAndTargetValue(-100.f);
    rmsLevelOutputLeft.reset(sampleRate, 0.2);
    rmsLevelOutputRight.reset(sampleRate, 0.1);
    rmsLevelOutputLeft.setCurrentAndTargetValue(-100.f);
    rmsLevelOutputRight.setCurrentAndTargetValue(-100.f);

    leftChain.prepare(spec);
    rightChain.prepare(spec);

    updateFilters();

    leftChannelFifo.prepare(samplesPerBlock);
    rightChannelFifo.prepare(samplesPerBlock);

    spec.numChannels = getTotalNumOutputChannels();

    loadShippedImpulseResponses();

    irLoader.reset();
    irLoader.prepare(spec);

    if (shippedIRsMissing.get()) { return; }

    std::unique_ptr<juce::AudioFormatReader> audioFormatReader;
    audioFormatReader.reset(formatManager.createReaderFor(impulseResponseArray[0][0][0][0]));
    int numChannels = audioFormatReader->numChannels;
    int numSamples = audioFormatReader->lengthInSamples;
    // preallocating audioBuffers here instead of in processing block
    audioBufferInterpBL.setSize(numChannels, numSamples);
    audioBufferInterpBR.setSize(numChannels, numSamples);
    audioBufferInterpTL.setSize(numChannels, numSamples);
    audioBufferInterpTR.setSize(numChannels, numSamples);
    audioBufferInterpFin.setSize(numChannels, numSamples);
    newIRAudioBuffer.setSize(numChannels, numSamples);

}

void BasicEQAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BasicEQAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void BasicEQAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto settings = getChainSettings(apvts);
    juce::dsp::AudioBlock<float> block(buffer);


    //==============================================================================================================================================
    // APPLY GAIN KNOB
    inputGain.process(juce::dsp::ProcessContextReplacing<float>(block));
    //==============================================================================================================================================
    // CALC and SET RMS LEVEL OF L&R CHANNELS
    rmsLevelInputLeft.skip(buffer.getNumSamples());
    rmsLevelInputRight.skip(buffer.getNumSamples());
    const auto valueInLeft = juce::Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));
    if (valueInLeft < rmsLevelInputLeft.getCurrentValue()) { rmsLevelInputLeft.setTargetValue(valueInLeft); } // if the new value is lower than the current one, apply smoothing
    else { rmsLevelInputLeft.setCurrentAndTargetValue(valueInLeft); }  // if the new value is greater than the current one, do not apply smoothing - so that transients are shown well

    const auto valueInRight = juce::Decibels::gainToDecibels(buffer.getRMSLevel(1, 0, buffer.getNumSamples()));
    if (valueInRight < rmsLevelInputRight.getCurrentValue()) { rmsLevelInputRight.setTargetValue(valueInRight); } // if the new value is lower than the current one, apply smoothing
    else { rmsLevelInputRight.setCurrentAndTargetValue(valueInRight); }  // if the new value is greater than the current one, do not apply smoothing - so that transients are shown well
    //==============================================================================================================================================
    // AMP PROCESS
    ampSim.drive = settings.drive;
    ampSim.osBypassed = settings.osBypassed;
    ampSim.ampType = settings.ampType;
    if (!settings.ampBypassed) {
        ampSim.process(block);
    }
    //==============================================================================================================================================
    // TONESTACK PROCESS
    toneStack.setLowGain(settings.stackLowGain);
    toneStack.setMidGain(settings.stackMidGain);
    toneStack.setHighGain(settings.stackHighGain);

    toneStack.process(block);
    //==============================================================================================================================================
    // IR PROCESS
    if (!settings.irBypassed)
    {
        if (irLoader.getCurrentIRSize() > 0)
        {
            // check if new IR is ready
            if (newIRReady.get())
            {
                // set new IR in ir loader
                irLoader.loadImpulseResponse((juce::AudioBuffer<float>)newIRAudioBuffer, interpIRSampleRate, juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::yes, juce::dsp::Convolution::Normalise::yes);
            }
            irLoader.process(juce::dsp::ProcessContextReplacing<float>(block));
        }
    }
    //==============================================================================================================================================
    // APPLY OUTPUT GAIN KNOB
    outputGain.process(juce::dsp::ProcessContextReplacing<float>(block));
    //==============================================================================================================================================
    // CALC and SET RMS LEVEL OF L&R CHANNELS
    rmsLevelOutputLeft.skip(buffer.getNumSamples());
    rmsLevelOutputRight.skip(buffer.getNumSamples());
    const auto valueOutLeft = juce::Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));
    if (valueOutLeft < rmsLevelOutputLeft.getCurrentValue()) { rmsLevelOutputLeft.setTargetValue(valueOutLeft); } // if the new value is lower than the current one, apply smoothing
    else { rmsLevelOutputLeft.setCurrentAndTargetValue(valueOutLeft); }  // if the new value is greater than the current one, do not apply smoothing - so that transients are shown well

    const auto valueOutRight = juce::Decibels::gainToDecibels(buffer.getRMSLevel(1, 0, buffer.getNumSamples()));
    if (valueOutRight < rmsLevelOutputRight.getCurrentValue()) { rmsLevelOutputRight.setTargetValue(valueOutRight); } // if the new value is lower than the current one, apply smoothing
    else { rmsLevelOutputRight.setCurrentAndTargetValue(valueOutRight); }  // if the new value is greater than the current one, do not apply smoothing - so that transients are shown well
    //==============================================================================================================================================
    // UPDATE FIFO FOR FFT VISUAL
    leftChannelFifo.update(buffer);
    rightChannelFifo.update(buffer);
    //==============================================================================================================================================
}

//==============================================================================
bool BasicEQAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* BasicEQAudioProcessor::createEditor()
{
    return new BasicEQAudioProcessorEditor (*this);
    //return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void BasicEQAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void BasicEQAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid())
    {
        apvts.replaceState(tree);
        updateFilters();
        auto settings = getChainSettings(apvts);
        loadShippedImpulseResponses();
        juce::AudioBuffer<float> irBuffer;
        int sampleRate = 0;
        if (shippedIRsMissing.get()) return;
        updateLoadedIR(irBuffer, sampleRate, settings.comboType, settings.micType, settings.yPos, settings.xPos);
    }
}

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts)
{
    ChainSettings settings;

    settings.lowCutFreq = apvts.getRawParameterValue("LowCut Freq")->load();
    settings.highCutFreq = apvts.getRawParameterValue("HighCut Freq")->load();
    settings.peakFreq = apvts.getRawParameterValue("Peak Freq")->load();
    settings.peakGainInDecibels = apvts.getRawParameterValue("Peak Gain")->load();
    settings.peakQuality = apvts.getRawParameterValue("Peak Q")->load();
    settings.lowCutSlope = static_cast<Slope>(apvts.getRawParameterValue("LowCut Slope")->load());
    settings.highCutSlope = static_cast<Slope>(apvts.getRawParameterValue("HighCut Slope")->load());
    settings.xPos = apvts.getRawParameterValue("X Position")->load();
    settings.yPos = apvts.getRawParameterValue("Y Position")->load();
    settings.lowCutBypassed = apvts.getRawParameterValue("LowCut Bypassed")->load() > 0.5f;
    settings.highCutBypassed = apvts.getRawParameterValue("HighCut Bypassed")->load() > 0.5f;
    settings.peakBypassed = apvts.getRawParameterValue("Peak Bypassed")->load() > 0.5f;
    settings.irBypassed = apvts.getRawParameterValue("IR Bypassed")->load() > 0.5f;
    settings.inputGainInDecibels = apvts.getRawParameterValue("Input Gain")->load();
    settings.outputGainInDecibels = apvts.getRawParameterValue("Output Gain")->load();
    settings.micType = static_cast<micTypeEnum>(apvts.getRawParameterValue("Mic Type")->load());
    settings.comboType = static_cast<comboTypeEnum>(apvts.getRawParameterValue("Combo Type")->load());
    settings.ampBypassed = apvts.getRawParameterValue("Amp Bypassed")->load() > 0.5f;
    settings.ampType = static_cast<AmpTypeEnum>(apvts.getRawParameterValue("Amp Type")->load());
    settings.drive = apvts.getRawParameterValue("Drive")->load();
    settings.stackLowGain = apvts.getRawParameterValue("LowShelfGain")->load();
    settings.stackMidGain = apvts.getRawParameterValue("MidPeakGain")->load();
    settings.stackHighGain = apvts.getRawParameterValue("HighShelfGain")->load();
    return settings;
}

Coefficients makePeakFilter(const ChainSettings& chainSettings, double sampleRate)
{
    return juce::dsp::IIR::Coefficients<float>::makePeakFilter( sampleRate,
                                                                chainSettings.peakFreq,
                                                                chainSettings.peakQuality,
                                                                juce::Decibels::decibelsToGain(chainSettings.peakGainInDecibels));
}

void BasicEQAudioProcessor::updateLoadedIR(juce::AudioBuffer<float>& bufferInterp, int& sampleRate, int comboTypeID, int mikTypeID, float yPos, float xPos)
{
    // this might take a while, need to turn off processing
    suspendProcessing(true);
    irLoader.reset();
    // interpolate
    int yPosRoundDown = 0, yPosRoundUp = 0, xPosRoundDown = 0, xPosRoundUp = 0;
    float XmaxDistance = 0, Xdistance = 0, XtransposedDistance = 0, YmaxDistance = 0, Ydistance = 0, YtransposedDistance = 0;
    int yPosArr[3] = { 0, 10, 40 };
    int xPosArr[6] = { 0, 2, 4, 6, 8, 10 };
    // formatManager takes a file (wav in our case), returns AudioBuffer
    
    
    // if Y = {0,10,40} and X = {0,2,4,..,10}, no need to interpolate
    if (std::any_of(std::begin(yPosArr), std::end(yPosArr), [&](int i) { return i == yPos; }) && std::any_of(std::begin(xPosArr), std::end(xPosArr), [&](int j) {return j == xPos; })) {
    // load IR, stereo, trimmed, normalized, size 0 = original IR size
        irLoader.loadImpulseResponse(impulseResponseArray[comboTypeID][mikTypeID][std::ceil((double)yPos/20)][xPos], juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::yes, 0, juce::dsp::Convolution::Normalise::yes);
        std::unique_ptr<juce::AudioFormatReader> reader;
        reader.reset(formatManager.createReaderFor(impulseResponseArray[comboTypeID][mikTypeID][std::ceil(yPos / 20)][xPos]));
        bufferInterp.setSize(reader->numChannels, reader->lengthInSamples);
        if (!reader->read(&bufferInterp, 0, reader->lengthInSamples, 0, true, true)) { return; }
        
        sampleRate = reader->sampleRate;
        
        suspendProcessing(false);
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
    
    
    // impulseResponseArray[combo type][mic type][Y pos == 0=0, 1=10, 2=40]  [X pos] 
    std::unique_ptr<juce::AudioFormatReader> readerBL, readerBR, readerTL, readerTR; // bottom-left, bottom-right etc

    readerBL.reset(formatManager.createReaderFor(impulseResponseArray[comboTypeID][mikTypeID][std::ceil((double)yPosRoundDown / 20)][xPosRoundDown]));
    readerBR.reset(formatManager.createReaderFor(impulseResponseArray[comboTypeID][mikTypeID][std::ceil((double)yPosRoundDown / 20)][xPosRoundUp]));
    readerTL.reset(formatManager.createReaderFor(impulseResponseArray[comboTypeID][mikTypeID][std::ceil((double)yPosRoundUp / 20)][xPosRoundDown]));
    readerTR.reset(formatManager.createReaderFor(impulseResponseArray[comboTypeID][mikTypeID][std::ceil((double)yPosRoundUp / 20)][xPosRoundUp]));
    
    sampleRate = readerBL->sampleRate;
    audioBufferInterpBL.setSize(readerBL->numChannels, readerBL->lengthInSamples);
    audioBufferInterpBR.setSize(readerBR->numChannels, readerBR->lengthInSamples);
    audioBufferInterpTL.setSize(readerTL->numChannels, readerTL->lengthInSamples);
    audioBufferInterpTR.setSize(readerTR->numChannels, readerTR->lengthInSamples);
    readerBL->read(&audioBufferInterpBL, 0, readerBL->lengthInSamples, 0, true, true);
    readerBR->read(&audioBufferInterpBR, 0, readerBR->lengthInSamples, 0, true, true);
    readerTL->read(&audioBufferInterpTL, 0, readerTL->lengthInSamples, 0, true, true);
    readerTR->read(&audioBufferInterpTR, 0, readerTR->lengthInSamples, 0, true, true);
    
    // check if both audioBuffers are equal length
    if (audioBufferInterpBL.getNumChannels() != audioBufferInterpBR.getNumChannels() || audioBufferInterpBL.getNumSamples() != audioBufferInterpBR.getNumSamples()) { return; }
    
    bufferInterp.setSize(audioBufferInterpBL.getNumChannels(), audioBufferInterpBL.getNumSamples());
    audioBufferInterpFin.setSize(audioBufferInterpBL.getNumChannels(), audioBufferInterpBL.getNumSamples());
    float interpBottom = 0, interpTop = 0, interpValue = 0;
    
    for (int ch = 0; ch < audioBufferInterpBL.getNumChannels(); ++ch) {
        for (int s = 0; s < audioBufferInterpBL.getNumSamples(); ++s) {
            interpBottom = audioBufferInterpBL.getSample(ch, s) * (1.0 - XtransposedDistance) + (audioBufferInterpBR.getSample(ch, s) * XtransposedDistance);
            interpTop = audioBufferInterpTL.getSample(ch, s) * (1.0 - XtransposedDistance) + (audioBufferInterpTR.getSample(ch, s) * XtransposedDistance);
            interpValue = (interpBottom * (1.0 - YtransposedDistance)) + (interpTop * YtransposedDistance);
            audioBufferInterpFin.setSample(ch, s, interpValue);
        }
    }
    bufferInterp = audioBufferInterpFin;
    // load IR, stereo, trimmed, normalized, size 0 = original IR size
    irLoader.loadImpulseResponse((juce::AudioBuffer <float>)audioBufferInterpFin, (double)sampleRate, juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::yes, juce::dsp::Convolution::Normalise::yes);
    // turn on processing
    suspendProcessing(false);
    
}

void BasicEQAudioProcessor::updatePeakFilter(const ChainSettings& chainSettings)
{
    // calculate peak coefficients
    auto peakCoefficients = makePeakFilter(chainSettings, getSampleRate());

    leftChain.setBypassed<ChainPositions::Peak>(chainSettings.peakBypassed);
    rightChain.setBypassed<ChainPositions::Peak>(chainSettings.peakBypassed);

    // initializing LR chains, coefficients need to be dereferenced (get values they point at)
    updateCoefficients(leftChain.get<ChainPositions::Peak>().coefficients, peakCoefficients);
    updateCoefficients(rightChain.get<ChainPositions::Peak>().coefficients, peakCoefficients);
}

void updateCoefficients(Coefficients& old, const Coefficients& replacements)
{
    *old = *replacements;
}

void BasicEQAudioProcessor::updateLowCutFilter(const ChainSettings& chainSettings)
{
    // lowCutSlope = (0,1,2,3) -> need to get order = (2,4,6,8) -> hence 2*(slope+1)
    auto lowCutCoefficients = makeLowCutFilter(chainSettings, getSampleRate());
    // init left chain
    auto& leftLowCut = leftChain.get<ChainPositions::LowCut>();
    leftChain.setBypassed<ChainPositions::LowCut>(chainSettings.lowCutBypassed);
    updateCutFilter(leftLowCut, lowCutCoefficients, chainSettings.lowCutSlope);

    // init right chain
    auto& rightLowCut = rightChain.get<ChainPositions::LowCut>();
    rightChain.setBypassed<ChainPositions::LowCut>(chainSettings.lowCutBypassed);
    updateCutFilter(rightLowCut, lowCutCoefficients, chainSettings.lowCutSlope);
}

void BasicEQAudioProcessor::updateHighCutFilter(const ChainSettings& chainSettings)
{
    // compute highcut coefficients
    auto highCutCoefficients = makeHighCutFilter(chainSettings, getSampleRate());
    // init both chains, update filter
    auto& leftHighCut = leftChain.get<ChainPositions::HighCut>();
    auto& rightHighCut = rightChain.get<ChainPositions::HighCut>();

    leftChain.setBypassed<ChainPositions::HighCut>(chainSettings.highCutBypassed);
    rightChain.setBypassed<ChainPositions::HighCut>(chainSettings.highCutBypassed);

    updateCutFilter(leftHighCut, highCutCoefficients, chainSettings.highCutSlope);
    updateCutFilter(rightHighCut, highCutCoefficients, chainSettings.highCutSlope);
}

void BasicEQAudioProcessor::updateToneStackLow(const ChainSettings& chainSettings)
{
    auto lowShelfCoefficients = IIRFilterCoeff::makeLowShelf(getSampleRate(), 62, 1 / (std::sqrt(2)), juce::Decibels::decibelsToGain(chainSettings.stackLowGain));
    updateCoefficients(toneStackFilters.get<0>().state, lowShelfCoefficients);
}

void BasicEQAudioProcessor::updateToneStackMid(const ChainSettings& chainSettings)
{
    auto midPeakCoefficients = IIRFilterCoeff::makePeakFilter(getSampleRate(), 700, 1 / (std::sqrt(2)), juce::Decibels::decibelsToGain(chainSettings.stackMidGain));
    updateCoefficients(toneStackFilters.get<1>().state, midPeakCoefficients);
}

void BasicEQAudioProcessor::updateToneStackHigh(const ChainSettings& chainSettings)
{
    auto highShelfCoefficients = IIRFilterCoeff::makeHighShelf(getSampleRate(), 1400, 1 / (std::sqrt(2)), juce::Decibels::decibelsToGain(chainSettings.stackHighGain));
    updateCoefficients(toneStackFilters.get<2>().state, highShelfCoefficients);
}

void BasicEQAudioProcessor::updateFilters()
{
    auto chainSettings = getChainSettings(apvts);

    updateLowCutFilter(chainSettings);
    updateHighCutFilter(chainSettings);
    updatePeakFilter(chainSettings);
    updateToneStackLow(chainSettings);
    updateToneStackMid(chainSettings);
    updateToneStackHigh(chainSettings);
}

    // Here are parameters defined
juce::AudioProcessorValueTreeState::ParameterLayout
    BasicEQAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    //============================================================================================================================================
    // EQ
    layout.add(std::make_unique<juce::AudioParameterFloat>("LowCut Freq", "LowCut Freq",
        juce::NormalisableRange<float>(10.f, 20000.f, 1.f, 0.3f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("HighCut Freq", "HighCut Freq",
        juce::NormalisableRange<float>(10.f, 20000.f, 1.f, 1.f), 20000.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Peak Freq", "Peak Freq",
        juce::NormalisableRange<float>(10.f, 20000.f, 1.f, 0.5f), 1500.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Peak Gain", "Peak Gain",
        juce::NormalisableRange<float>(-24.f, 24.f, 0.1f, 1.f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Peak Q", "Peak Q",
        juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.f), 7.f));
    juce::StringArray stringArray; // String array containing 4 choices for slope setting
    for (int i = 0; i < 4; i++) {
        juce::String str;
        str << (12 + i * 12);
        str << " db/Oct";
        stringArray.add(str);
    }
    layout.add(std::make_unique<juce::AudioParameterChoice>("LowCut Slope", "LowCut Slope", stringArray, 0));
    layout.add(std::make_unique<juce::AudioParameterChoice>("HighCut Slope", "HighCut Slope", stringArray, 0));
    //============================================================================================================================================
    // ToneStack
    layout.add(std::make_unique<juce::AudioParameterFloat>("LowShelfGain", "Low",
        juce::NormalisableRange<float>(-10.f, 10.f, 0.1f, 1.f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("MidPeakGain", "Mid",
        juce::NormalisableRange<float>(-6.f, 6.f, 0.1f, 1.f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("HighShelfGain", "High",
        juce::NormalisableRange<float>(-8.f, 8.f, 0.1f, 1.f), 0.0f));    
    
    //============================================================================================================================================
    // IR
    layout.add(std::make_unique<juce::AudioParameterFloat>("X Position", "X Position", juce::NormalisableRange<float>(0.f, 10.f, 0.05f), 0));

    juce::StringArray yPosChoices("0 cm", "10 cm", "40 cm");
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("Y Position", "Y Position", juce::NormalisableRange<float>(0.0f, 40.f, 0.05f, 1.f), 0));
    
    juce::StringArray comboChoices("Mar", "MM", "SV");
    layout.add(std::make_unique<juce::AudioParameterChoice>("Combo Type", "Combo Type", comboChoices, 1));
    juce::StringArray micChoices("57A", "kalib", "sm57");
    layout.add(std::make_unique<juce::AudioParameterChoice>("Mic Type", "Mic Type", micChoices, 1));
    //============================================================================================================================================
    // GAIN
    layout.add(std::make_unique<juce::AudioParameterFloat>("Input Gain", "Input Gain",
        juce::NormalisableRange<float>(-24.f, 24.f, 0.1f, 1.f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Output Gain", "Output Gain",
        juce::NormalisableRange<float>(-24.f, 24.f, 0.1f, 1.f), 0.0f));
    //============================================================================================================================================
    // AMP
    layout.add(std::make_unique<juce::AudioParameterFloat>("Drive", "Drive",
        juce::NormalisableRange<float>(0.1f, 10.f, 0.1f, 1.f), 5.f));
    //layout.add(std::make_unique<juce::AudioParameterBool>("Oversampling Bypassed", "Oversampling Bypassed", false));
    layout.add(std::make_unique<juce::AudioParameterBool>("Amp Bypassed", "Amp Bypassed", false));
    juce::StringArray ampChoices("Poletti", "Yamaha", "WaveFolder");
    layout.add(std::make_unique<juce::AudioParameterChoice>("Amp Type", "Amp Type", ampChoices, 0));
    //============================================================================================================================================
    // BYPASS BUTTONS
    layout.add(std::make_unique<juce::AudioParameterBool>("LowCut Bypassed", "LowCut Bypassed", false));
    layout.add(std::make_unique<juce::AudioParameterBool>("HighCut Bypassed", "HighCut Bypassed", false));
    layout.add(std::make_unique<juce::AudioParameterBool>("Peak Bypassed", "Peak Bypassed", false));
    layout.add(std::make_unique<juce::AudioParameterBool>("IR Bypassed", "IR Bypassed", false));

    return layout;
}

void BasicEQAudioProcessor::loadShippedImpulseResponses()
{
    // impulseResponseArray[combo][mic][Y][X]
    // same structure in Data folder
    
    int comboType, mikType, yPosition, xPosition;
    impulseResponseArray.resize(3);
    for (auto& array_2 : impulseResponseArray) {
        array_2.resize(3);
        for (auto& array_1 : array_2) {
            array_1.resize(3);
            for (auto& array_0 : array_1) {
                array_0.resize(12);
            }
        }
    }
    juce::File dataFolder = juce::File::getSpecialLocation(juce::File::SpecialLocationType::commonApplicationDataDirectory).getChildFile("PechacekIRLoader").getChildFile("Data");
    
    // check if data folder exists
    if (!dataFolder.exists()) {
        //set flag to display warning
        shippedIRsMissing.set(true);
        return;
    }
    // check if data folder isnt empty or missing files - findChildFiles is recursive, returns array of matching files, full collection should be 162 files
    if (!dataFolder.containsSubDirectories() || dataFolder.findChildFiles(juce::File::TypesOfFileToFind::findFiles, true, "*.wav", juce::File::FollowSymlinks::yes).size() < 162) {
        shippedIRsMissing.set(true);
        return;
    }

    // parsing filename, filling file array
    for (juce::DirectoryEntry entry : juce::RangedDirectoryIterator(dataFolder, true, "*.wav", 2)) {
        juce::String filename = entry.getFile().getFileNameWithoutExtension();
        juce::StringArray filenameArray;
        filenameArray.addTokens(filename, "_", "\"");
        
        if (filenameArray[0] == "57A") { mikType = 0; }
        else if (filenameArray[0] == "kalib") { mikType = 1; }
        else { mikType = 2; }
        if (filenameArray[1] == "0cm") { yPosition = 0; }
        else if (filenameArray[1] == "10cm") { yPosition = 1; }
        else { yPosition = 2; }
        if (filenameArray[2] == "Mar") { comboType = 0; }
        else if (filenameArray[2] == "MM") { comboType = 1; }
        else { comboType = 2; }
        xPosition = filenameArray[3].getIntValue();

        juce::File file = entry.getFile();
        
        impulseResponseArray.getReference(comboType).getReference(mikType).getReference(yPosition).set(xPosition, file);
        
        filenameArray.clear();
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BasicEQAudioProcessor();
}

float BasicEQAudioProcessor::getInputRMSValue(const int channel) const
{
    jassert(channel == 0 || channel == 1);
    if (channel == 0) { return rmsLevelInputLeft.getCurrentValue(); }
    else if (channel == 1) { return rmsLevelInputRight.getCurrentValue(); }
    return 0.f;
}

float BasicEQAudioProcessor::getOutputRMSValue(const int channel) const
{
    jassert(channel == 0 || channel == 1);
    if (channel == 0) { return rmsLevelOutputLeft.getCurrentValue(); }
    else if (channel == 1) { return rmsLevelOutputRight.getCurrentValue(); }
    return 0.f;
}