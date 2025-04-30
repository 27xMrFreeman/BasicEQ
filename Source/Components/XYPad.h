#pragma once
#include <JuceHeader.h>

class XYPad : public juce::Component
{
public:
    enum Axis { X, Y };

    class Thumb : public juce::Component
    {
    public:
        Thumb();
        void paint(juce::Graphics& g) override;
        void mouseDown(const juce::MouseEvent& event) override;
        void mouseDrag(const juce::MouseEvent& event) override;

        std::function<void(juce::Point<double>)> moveCallback;

        juce::ComponentDragger dragger;
        juce::ComponentBoundsConstrainer constrainer;


        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Thumb)
    };

    XYPad();

    void paint(juce::Graphics& g) override;
    void resized() override;
    void registerSlider(juce::Slider* slider, Axis axis);
    void deregisterSlider(juce::Slider* slider);

    std::vector<juce::Slider*> xSliders, ySliders;
    Thumb thumb;
    double thumbSize{ 28 };
    std::mutex vectorMutex;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (XYPad)
};