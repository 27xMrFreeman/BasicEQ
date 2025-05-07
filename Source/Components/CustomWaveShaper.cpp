
#pragma once
#include <JuceHeader.h>
template <typename Type, typename Function>
class CustomWaveShaper : public juce::dsp::WaveShaper<Type, Function>
{
public:
    float asymPosGain{ 0 }, asymNegGain{ 0 }, symGain{ 0 }, asymPosLP{ 23.6f }, asymPosLN{ 0.5f }, asymNegLP{ 0.5f }, asymNegLN{ 23.6f }, symLPLN{ 1.1f };

};
