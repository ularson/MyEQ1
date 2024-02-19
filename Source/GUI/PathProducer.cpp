/*
  ==============================================================================

    PathProducer.cpp
    Created: 7 Feb 2024 11:45:37pm
    Author:  Ulf Larsson

  ==============================================================================
*/

#include "PathProducer.h"

void PathProducer::process(juce::Rectangle<float> fftBounds, double sampleRate)
{
    juce::AudioBuffer<float> tempIncomningBuffer;
    
    while(leftChannelFifo->getNumCompleteBuffersAvailable() > 0 )
    {
        if(leftChannelFifo->getAudioBuffer(tempIncomningBuffer))
        {
            auto size = tempIncomningBuffer.getNumSamples();
            
            juce::FloatVectorOperations::copy(monoBuffer.getWritePointer(0, 0),
                                              monoBuffer.getReadPointer(0, size),
                                              monoBuffer.getNumSamples() - size);
            
            juce::FloatVectorOperations::copy(monoBuffer.getWritePointer(0, monoBuffer.getNumSamples() - size),
                                              tempIncomningBuffer.getReadPointer(0, 0),
                                              size);
            
            leftChannelFFTDataGenerator.produceFFTDataForRendering(monoBuffer, -48.f);
             

            const auto fftSize = leftChannelFFTDataGenerator.getFFTSize();
            
            const auto binWidth = sampleRate / (double) fftSize;

            while( leftChannelFFTDataGenerator.getNumAvailableFFTDataBlocks() > 0)
            {
                std::vector<float> ftdData;
                if(leftChannelFFTDataGenerator.getFFTData(ftdData))
                {
                    pathProducer.generatePath(ftdData, fftBounds, fftSize, binWidth, -48.f);
                    
                }
            }
        }
    }
    
    const auto fftSize = leftChannelFFTDataGenerator.getFFTSize();
    
    const auto binWidth = sampleRate / (double) fftSize;
    
    while (leftChannelFFTDataGenerator.getNumAvailableFFTDataBlocks() > 0)
    {
        std::vector<float> fftData;
        if(leftChannelFFTDataGenerator.getFFTData(fftData))
        {
            pathProducer.generatePath(fftData, fftBounds, fftSize, binWidth, -48.f);
        }
    }
    
    while (pathProducer.getNumPathsAvailable() > 0)
    {
        pathProducer.getPath(leftChannelFFTPath);
    }
}
