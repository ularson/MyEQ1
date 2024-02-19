/*
  ==============================================================================

    ResponseCurveComponent.h
    Created: 8 Feb 2024 12:07:51am
    Author:  Ulf Larsson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PathProducer.h"
#include "../PluginProcessor.h"

struct ResponseCurveComponent: juce::Component,
juce::AudioProcessorParameter::Listener,
juce::Timer
{
    ResponseCurveComponent(SimpleEQAudioProcessor&);
    ~ResponseCurveComponent();
    
    void parameterValueChanged (int parameterIndex, float newValue) override;

    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override { }
    
    void timerCallback() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void toggleAnalysisEnablement(bool enabled)
    {
        shouldShowFFTAnalysis = enabled;
    }

    
    
private:
    SimpleEQAudioProcessor& audioProcessor;
    juce::Atomic<bool> parametersChanged { false };
    
    MonoChain monoChain;
    
    void updateChain();
    
    juce::Image background;
    
    juce::Rectangle<int> getRenderArea();
    
    juce::Rectangle<int> getAnalysisArea();
    
    std::vector<float> getFrequencies();
    
    std::vector<float> getGains();
    
    std::vector<float> getXs(const std::vector<float> &freqs, float left, float width);
    
    void drawTextLabels(juce::Graphics &g, juce::Rectangle<int> bounds);
    
    void drawBackgroundGrid(juce::Graphics &g, juce::Rectangle<int> bounds);
    
    void drawFFTAnalysis(juce::Graphics& g, juce::Rectangle<int> bounds);
    
    PathProducer leftPathProducer, rightPathProducer;
    
    bool shouldShowFFTAnalysis = true;
};

