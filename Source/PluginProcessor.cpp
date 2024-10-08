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

#include "PluginProcessor.h"
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
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;
    
    outputGain.reset();
    outputGain.prepare(spec);
    outputGain.setGainDecibels(0);

    rmsLevelLeft.reset(sampleRate, 0.2);
    rmsLevelRight.reset(sampleRate, 0.1);
    rmsLevelLeft.setCurrentAndTargetValue(-100.f);
    rmsLevelRight.setCurrentAndTargetValue(-100.f);

    leftChain.prepare(spec);
    rightChain.prepare(spec);

    updateFilters();

    leftChannelFifo.prepare(samplesPerBlock);
    rightChannelFifo.prepare(samplesPerBlock);

    spec.numChannels = getTotalNumOutputChannels();

    loadShippedImpulseResponses();

    irLoader.reset();
    irLoader.prepare(spec);

    /*osc.initialise([](float x) { return std::sin(x); });
    osc.prepare(spec);
    osc.setFrequency(50);*/ // oscillator for testing FFT

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
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    updateFilters();

    juce::dsp::AudioBlock<float> block(buffer);

    //buffer.clear(); // for testing FFT with oscillator
    //juce::dsp::ProcessContextReplacing<float> stereoContext(block);
    //osc.process(stereoContext);

    // input block divided to mono L/R for EQ processing
    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);
    
    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

    leftChain.process(leftContext);
    rightChain.process(rightContext);
    
    //input stereo block sent to irLoader
    //DBG((int)!settings.irBypassed);
    if (!settings.irBypassed)
    {
        if (irLoader.getCurrentIRSize() > 0)
        {
            irLoader.process(juce::dsp::ProcessContextReplacing<float>(block));
        }
    }

    // APPLY GAIN KNOB
    outputGain.process(juce::dsp::ProcessContextReplacing<float>(block));

    // CALC and SET RMS LEVEL OF L&R CHANNELS
    rmsLevelLeft.skip(buffer.getNumSamples());
    rmsLevelRight.skip(buffer.getNumSamples());
    const auto valueLeft = juce::Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));
    if (valueLeft < rmsLevelLeft.getCurrentValue()) { rmsLevelLeft.setTargetValue(valueLeft); } // if the new value is lower than the current one, apply smoothing
    else { rmsLevelLeft.setCurrentAndTargetValue(valueLeft); }  // if the new value is greater than the current one, do not apply smoothing - so that transients are shown well

    const auto valueRight = juce::Decibels::gainToDecibels(buffer.getRMSLevel(1, 0, buffer.getNumSamples()));
    if (valueRight < rmsLevelRight.getCurrentValue()) { rmsLevelRight.setTargetValue(valueRight); } // if the new value is lower than the current one, apply smoothing
    else { rmsLevelRight.setCurrentAndTargetValue(valueRight); }  // if the new value is greater than the current one, do not apply smoothing - so that transients are shown well
    

    leftChannelFifo.update(buffer);
    rightChannelFifo.update(buffer);

    //DBG("IR size is " << irLoader.getCurrentIRSize());
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    //for (int channel = 0; channel < totalNumInputChannels; ++channel)
    //{
    //    auto* channelData = buffer.getWritePointer (channel);

    //    // ..do something to the data...
    //}
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
    settings.yPos = static_cast<Distance>(apvts.getRawParameterValue("Y Position")->load());
    settings.lowCutBypassed = apvts.getRawParameterValue("LowCut Bypassed")->load() > 0.5f;
    settings.highCutBypassed = apvts.getRawParameterValue("HighCut Bypassed")->load() > 0.5f;
    settings.peakBypassed = apvts.getRawParameterValue("Peak Bypassed")->load() > 0.5f;
    settings.irBypassed = apvts.getRawParameterValue("IR Bypassed")->load() > 0.5f;
    settings.outputGainInDecibels = apvts.getRawParameterValue("Output Gain")->load();
    return settings;
}

Coefficients makePeakFilter(const ChainSettings& chainSettings, double sampleRate)
{
    return juce::dsp::IIR::Coefficients<float>::makePeakFilter( sampleRate,
                                                                chainSettings.peakFreq,
                                                                chainSettings.peakQuality,
                                                                juce::Decibels::decibelsToGain(chainSettings.peakGainInDecibels));
}

juce::File BasicEQAudioProcessor::updateLoadedIR(int comboTypeID, int mikTypeID, int yPos, int xPos)
{
    irLoader.reset();
    // load IR, stereo, trimmed, normalized, size 0 = original IR size
    irLoader.loadImpulseResponse(impulseResponseArray[comboTypeID][mikTypeID][yPos][xPos], juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::yes, 0, juce::dsp::Convolution::Normalise::yes);
    /*DBG("Loaded IR from array " << comboTypeID << " " << mikTypeID << " " << yPos << " " << xPos);
    DBG("File name is " << impulseResponseArray[comboTypeID][mikTypeID][yPos][xPos].getFileName());
    DBG("IR Size is " << irLoader.getCurrentIRSize());*/
    return impulseResponseArray[comboTypeID][mikTypeID][yPos][xPos];
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

void BasicEQAudioProcessor::updateFilters()
{
    auto chainSettings = getChainSettings(apvts);

    updateLowCutFilter(chainSettings);
    updateHighCutFilter(chainSettings);
    updatePeakFilter(chainSettings);
}

    // Here are parameters defined
juce::AudioProcessorValueTreeState::ParameterLayout
    BasicEQAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
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

    layout.add(std::make_unique<juce::AudioParameterFloat>("X Position", "X Position", juce::NormalisableRange<float>(0, 8, 2), 0));

    juce::StringArray yPosChoices("0 cm", "10 cm", "40 cm");
    
    layout.add(std::make_unique<juce::AudioParameterChoice>("Y Position", "Y Position", yPosChoices, 0));

    juce::StringArray stringArray; // String array containing 4 choices for slope setting
    for (int i = 0; i < 4; i++) {
        juce::String str;
        str << (12 + i * 12);
        str << " db/Oct";
        stringArray.add(str);
    }

    layout.add(std::make_unique<juce::AudioParameterFloat>("Output Gain", "Output Gain",
        juce::NormalisableRange<float>(-24.f, 24.f, 0.1f, 1.f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterChoice>("LowCut Slope", "LowCut Slope", stringArray, 0));
    layout.add(std::make_unique<juce::AudioParameterChoice>("HighCut Slope", "HighCut Slope", stringArray, 0));

    layout.add(std::make_unique<juce::AudioParameterBool>("LowCut Bypassed", "LowCut Bypassed", false));
    layout.add(std::make_unique<juce::AudioParameterBool>("HighCut Bypassed", "HighCut Bypassed", false));
    layout.add(std::make_unique<juce::AudioParameterBool>("Peak Bypassed", "Peak Bypassed", false));
    layout.add(std::make_unique<juce::AudioParameterBool>("IR Bypassed", "IR Bypassed", false));

    return layout;
}

void BasicEQAudioProcessor::loadShippedImpulseResponses()
{
    // impulseResponseArray[typ komba][typ mikrofonu][pozice Y][pozice X]
    // toto nasleduje strukturu slozek v Data
    // 
    
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
    /*DBG(impulseResponseArray.size());
    DBG(impulseResponseArray[0].size());
    DBG(impulseResponseArray[0][0].size());
    DBG(impulseResponseArray[0][0][0].size());*/

    //DBG(juce::File::getSpecialLocation(juce::File::currentExecutableFile).getParentDirectory().getParentDirectory().getParentDirectory().getParentDirectory().getParentDirectory().getParentDirectory().getChildFile("Data").getFullPathName());
    //juce::File dataFolder = juce::File::getSpecialLocation(juce::File::currentExecutableFile).getParentDirectory().getParentDirectory().getParentDirectory().getParentDirectory().getParentDirectory().getParentDirectory().getChildFile("Data");
    juce::File dataFolder = juce::File::getSpecialLocation(juce::File::SpecialLocationType::commonApplicationDataDirectory).getChildFile("PechacekIRLoader").getChildFile("Data");
    //DBG(dataFolder.getFullPathName());
    for (juce::DirectoryEntry entry : juce::RangedDirectoryIterator(/*juce::File("C:/Users/knize/Documents/VST_CODE/BasicEQ/Data")*/dataFolder, true, "*.wav", 2)) {
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
        //DBG("added " << file.getFileName());

        //juce::String filepath = impulseResponseArray.getUnchecked(comboType).getUnchecked(mikType).getUnchecked(yPosition).getUnchecked(xPosition).getFullPathName();
        /*juce::String filepath = impulseResponseArray[comboType][mikType][yPosition][xPosition].getFullPathName();
        DBG("Loading " << filename << " into array " << comboType << " " << mikType << " " << yPosition << " " << xPosition);
        DBG("Loaded " << impulseResponseArray[comboType][mikType][yPosition][xPosition].getFullPathName() << " into array");
        DBG("File in 2 2 2 10: " << impulseResponseArray.getUnchecked(2).getUnchecked(2).getUnchecked(2).getUnchecked(10).getFullPathName());*/
        filenameArray.clear();
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BasicEQAudioProcessor();
}

float BasicEQAudioProcessor::getRMSValue(const int channel) const
{
    jassert(channel == 0 || channel == 1);
    if (channel == 0) { return rmsLevelLeft.getCurrentValue(); }
    else if (channel == 1) { return rmsLevelRight.getCurrentValue(); }
    return 0.f;
}