#include "XYPad.h"

XYPad::Thumb::Thumb()
{
	constrainer.setMinimumOnscreenAmounts(28,28,28,28);
}

void XYPad::Thumb::paint(juce::Graphics& g)
{
	g.setColour(thumbColour);
	g.drawEllipse(getLocalBounds().reduced(4).toFloat(), 4.f);
}

void XYPad::Thumb::mouseDown(const juce::MouseEvent& event)
{
	dragger.startDraggingComponent(this, event);
}

void XYPad::Thumb::mouseDrag(const juce::MouseEvent& event)
{
	dragger.dragComponent(this, event, &constrainer);
	if (moveCallback) { moveCallback(getPosition().toDouble()); }
}

void XYPad::Thumb::setThumbColor(juce::Colour newColour)
{
	thumbColour = newColour;
}

//=====================================================================================

XYPad::XYPad()
{
	addAndMakeVisible(thumb);
	thumb.moveCallback = [&](juce::Point<double> position) {
		const std::lock_guard<std::mutex> lock(vectorMutex);
		auto bounds = getLocalBounds().toDouble();
		for (auto* slider : xSliders) {
			slider->setValue(juce::jmap(position.getX(), 0.0, bounds.getWidth() - thumbSize, slider->getMinimum(), slider->getMaximum()));
		}
		for (auto* slider : ySliders) {
			slider->setValue(juce::jmap(position.getY(), bounds.getHeight() - thumbSize, 0.0, slider->getMinimum(), slider->getMaximum()));
		}
	};
}

void XYPad::paint(juce::Graphics& g)
{
	g.setColour(juce::Colours::black);
	g.fillRoundedRectangle(getLocalBounds().toFloat(), 10.f);
}

void XYPad::resized()
{
	auto bounds = getLocalBounds();
	auto thumbBounds = bounds.withSizeKeepingCentre(thumbSize, thumbSize);
	thumb.setBounds(thumbBounds);
	if (!xSliders.empty()) {
		thumb.setTopLeftPosition(	juce::jmap(xSliders[0]->getValue(), xSliders[0]->getMinimum(), xSliders[0]->getMaximum(), 0.0, bounds.getWidth() - thumbSize),
									thumb.getY());
	}
	if (!ySliders.empty()) {
		thumb.setTopLeftPosition(	thumb.getX(),
									juce::jmap(ySliders[0]->getValue(), ySliders[0]->getMinimum(), ySliders[0]->getMaximum(), bounds.getHeight()-thumbSize, 0.0));
	}
}

void XYPad::registerSlider(juce::Slider* slider, Axis axis)
{
	// scoped lock to prevent manipulating vector from multiple sources
	const std::lock_guard<std::mutex> lock(vectorMutex);
	if (axis == Axis::X) {
		xSliders.push_back(slider);
	}
	else if (axis == Axis::Y) {
		ySliders.push_back(slider);
	}
}

void XYPad::deregisterSlider(juce::Slider* slider)
{
	const std::lock_guard<std::mutex> lock(vectorMutex);
	// remove moves every matching slider to end of vector and returns iterator to that position
	// erase deletes everything from this iterator to the end of the vector
	xSliders.erase(std::remove(xSliders.begin(), xSliders.end(), slider), xSliders.end());
}
