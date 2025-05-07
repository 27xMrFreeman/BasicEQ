
#include "PathProducer.h"

void PathProducer::process(juce::Rectangle<float> fftBounds, double sampleRate)
{
    juce::AudioBuffer<float> tempIncomingBuffer;
    // if there is a buffer available in the FIFO, send it to FFT data generator
    while (leftChannelFifo->getNumCompleteBuffersAvailable() > 0)
    {
        if (leftChannelFifo->getAudioBuffer(tempIncomingBuffer))    // if we can get the buffer
        {
            auto size = tempIncomingBuffer.getNumSamples();
            // this shifts stuff in the buffer by how big the sent out buffer is
            juce::FloatVectorOperations::copy(monoBuffer.getWritePointer(0, 0),   // where to copy - to the start of the buffer
                monoBuffer.getReadPointer(0, size), // source to copy - starts where the previous buffer block ended
                monoBuffer.getNumSamples() - size); // how many values to copy - all values except the ones in the previous buffer block

            juce::FloatVectorOperations::copy(monoBuffer.getWritePointer(0, monoBuffer.getNumSamples() - size),   // copy to the end of the buffer
                tempIncomingBuffer.getReadPointer(0, 0),                            // copy from start of incoming buffer
                size);                                                              // copy as many values as are in incoming buffer
            // block above effectively shifts audio in monoBuffer left by the block size, appending new data from tempIncomingBuffer at the end of monoBuffer //

            leftChannelFFTDataGenerator.produceFFTDataForRendering(monoBuffer, -92.f); // pass monoBuffer to the FFT, negativeInfinity set to -48dB (what level of audio will be the lowest)
        }
    }

    // if there are FFT data buffers to pull, try to pull it and generate path from it
    // fftBounds is where it should draw the path
    const auto fftSize = leftChannelFFTDataGenerator.getFFTSize();
    const auto binWidth = sampleRate / (double)fftSize; // e.g. 48000 / 2048 = 23 Hz - frequency width of one fft bin, casting fftSize to double because sampleRate is double

    while (leftChannelFFTDataGenerator.getNumAvailableFFTDataBlocks() > 0)
    {
        std::vector<float> fftData;
        if (leftChannelFFTDataGenerator.getFFTData(fftData))
        {
            pathProducer.generatePath(fftData, fftBounds, fftSize, binWidth, -92.f);
        }
    }

    // if there are paths that can be pulled, pull as many as possible, display the most recent one
    while (pathProducer.getNumPathsAvailable())
    {
        pathProducer.getPath(leftChannelFFTPath);
    }
}