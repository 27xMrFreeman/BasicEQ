/*
  ==============================================================================

    CustomWaveShaper.cpp
    Created: 2 Apr 2025 8:39:57pm
    Author:  knize

  ==============================================================================
*/
#pragma once
#include <JuceHeader.h>
template <typename Type, typename Function>
class CustomWaveShaper : public juce::dsp::WaveShaper<Type, Function>
{
public:
    /*float getAsymPosGain() { return asymPosGain; }
    float getAsymNegGain() { return asymNegGain; }
    float getSymGain() { return symGain; }
    float getAsymPosLP() { return asymPosLP; }
    float getAsymPosLN() { return asymPosLN; }
    float getAsymNegLP() { return asymNegLP; }
    float getAsymNegLN() { return asymNegLN; }
    float getSymLPLN() { return symLPLN; }

    void setAsymPosGain(float newVal)   { asymPosGain = newVal; }
    void setAsymNegGain(float newVal)   { asymNegGain = newVal; }
    void setSymGain(float newVal)       { symGain = newVal; }
    void setAsymPosLP(float newVal)     { asymPosLP = newVal; }
    void setAsymPosLN(float newVal)     { asymPosLN = newVal; }
    void setAsymNegLP(float newVal)     { asymNegLP = newVal; }
    void setAsymNegLN(float newVal)     { asymNegLN = newVal; }
    void setSymLPLN(float newVal)       { symLPLN = newVal; }
private:*/
    float asymPosGain{ 0 }, asymNegGain{ 0 }, symGain{ 0 }, asymPosLP{ 23.6f }, asymPosLN{ 0.5f }, asymNegLP{ 0.5f }, asymNegLN{ 23.6f }, symLPLN{ 1.1f };

};
