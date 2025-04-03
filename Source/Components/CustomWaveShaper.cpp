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
    float asymPosGain, asymNegGain, symGain, asymPosLP, asymPosLN, asymNegLP, asymNegLN, symLPLN;

};
