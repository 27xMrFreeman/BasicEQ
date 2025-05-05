#include "XYPad.h"

XYPad::Thumb::Thumb()
{
	constrainer.setMinimumOnscreenAmounts(thumbSize, thumbSize, thumbSize, thumbSize);
}

void XYPad::Thumb::paint(juce::Graphics& g)
{
	g.setColour(thumbColour);
	g.drawEllipse(getLocalBounds().reduced(6).toFloat(), 6.f);
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
	/*g.setColour(juce::Colours::black);
	g.fillRoundedRectangle(getLocalBounds().toFloat(), 10.f);*/
	g.drawImage(background, getLocalBounds().reduced(thumbSize/2).toFloat());
	//g.setColour(juce::Colours::red);
	//g.drawRect(getLocalBounds());
}

void XYPad::resized()
{
	auto bounds = getLocalBounds();
	thumbSize = bounds.getWidth() * 0.12;
	thumb.thumbSize = thumbSize;
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

	auto backgroundBounds = bounds.reduced(thumbSize);

	background = juce::Image(juce::Image::PixelFormat::ARGB, backgroundBounds.getWidth(), backgroundBounds.getHeight(), true);

	juce::Graphics g(background);

	g.setColour(juce::Colours::transparentWhite);
	g.setOpacity(0.0);
	g.fillAll();

	juce::Array<float> verticalLines
	{
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9
	};
	juce::Array<float> horizontalLines
	{
		10, 40
	};

	g.setColour(juce::Colour::fromString("FF3860B5"));
	
	for (auto l : verticalLines)
	{
		auto normX = juce::jmap(l, 0.f, 10.f, 0.f, (float)backgroundBounds.getWidth());
		//g.drawVerticalLine(normX, 0.f, backgroundBounds.getHeight());
		g.fillRect(normX, 0.f, 2.f, (float)backgroundBounds.getHeight());
	}
	g.fillRect(backgroundBounds.getWidth() - 2.f, 0.f, 2.f, (float)backgroundBounds.getHeight());
	//g.drawVerticalLine(backgroundBounds.getWidth()-1, 0.f, backgroundBounds.getHeight());

	//g.setColour(juce::Colour::fromRGB(120, 120, 120));
	for (auto l : horizontalLines)
	{
		auto normY = juce::jmap(l, 40.f, 0.f, 0.f, (float)backgroundBounds.getHeight());
		g.fillRect(0.f, normY, (float)backgroundBounds.getWidth(), 2.f);
		//auto normX = juce::mapFromLog10(f, 20.f, 20000.f);
		//g.drawVerticalLine(getWidth() * normX, 0.f, getHeight());
	}
	g.fillRect(0.f, (float)backgroundBounds.getHeight()-2, (float)backgroundBounds.getWidth(), 2.f);
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
