/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GUI/LookAndFeel.h"
#include "GUI/CustomButtons.h"
#include "GUI/RotarySliderWithLabels.h"
#include "GUI/ResponseCurveComponent.h"
#include "PluginProcessor.h"

// TODO:
// Add utilities methods
// Refactor ResponseCurveComponent
// Add more bands


//==============================================================================
/**
*/
class SimpleEQAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SimpleEQAudioProcessorEditor (SimpleEQAudioProcessor&);
    ~SimpleEQAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SimpleEQAudioProcessor& audioProcessor;
    
    RotarySliderWithLabels peakFreqSlider,
        peakGainSlider,
        peakQualitySlider,
        lowCutFreqSlider,
        highCutFreqSlider,
        lowCutSlopeSlider,
        highCutSlopeSlider;
    
    ResponseCurveComponent responseCurveComponent;
    
    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::SliderAttachment;
    
    Attachment peakFreqSliderAttachement,
        peakGainSliderAttachement,
        peakQualitySliderAttachement,
        lowCutFreqSliderAttachement,
        highCutFreqSliderAttachement,
        lowCutSlopeSliderAttachement,
        highCutSlopeSliderAttachement;
    
    PowerButton lowcutBypassButton, peakBypassButton, highcutBypassButton;
    AnalyzerButton analyzerEnabledButton;
    
    using ButtonAttachement = APVTS::ButtonAttachment;
    ButtonAttachement lowcutBypassButtonAttachment,
                      peakBypassButtonAttachment,
                      highcutBypassButtonAttachment,
                      analyzerEnabledButtonAttachment;
    
    std::vector<juce::Component*> getComps();
    
    LookAndFeel lnf;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleEQAudioProcessorEditor)
};
