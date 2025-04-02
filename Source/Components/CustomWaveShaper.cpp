/*
  ==============================================================================

    CustomWaveShaper.cpp
    Created: 2 Apr 2025 8:39:57pm
    Author:  knize

  ==============================================================================
*/
#include <JuceHeader.h>
template <typename Type, typename Function>
class CustomWaveShaper : public juce::dsp::WaveShaper<Type, Function>
{
public:

private:
    float asymPosGain, asymNegGain, symGain, asymPosLP, asymPosLN, asymNegLP, asymNegLN, symLPLN;

};
