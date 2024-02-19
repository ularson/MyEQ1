/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================

//==============================================================================
SimpleEQAudioProcessorEditor::SimpleEQAudioProcessorEditor (SimpleEQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
peakFreqSlider(*audioProcessor.apvts.getParameter("Peak Freq"), "Hz"),
peakGainSlider(*audioProcessor.apvts.getParameter("Peak Gain"), "dB"),
peakQualitySlider(*audioProcessor.apvts.getParameter("Peak Quality"), ""),
lowCutFreqSlider(*audioProcessor.apvts.getParameter("LowCut Freq"), "Hz"),
highCutFreqSlider(*audioProcessor.apvts.getParameter("HighCut Freq"), "Hz"),
lowCutSlopeSlider(*audioProcessor.apvts.getParameter("LowCut Slope"), "dB/oct"),
highCutSlopeSlider(*audioProcessor.apvts.getParameter("HighCut Slope"), "dB/oct"),

responseCurveComponent(audioProcessor),
peakFreqSliderAttachement(audioProcessor.apvts, "Peak Freq", peakFreqSlider),
peakGainSliderAttachement(audioProcessor.apvts, "Peak Gain", peakGainSlider),
peakQualitySliderAttachement(audioProcessor.apvts, "Peak Quality", peakQualitySlider),
lowCutFreqSliderAttachement(audioProcessor.apvts, "LowCut Freq", lowCutFreqSlider),
highCutFreqSliderAttachement(audioProcessor.apvts, "HighCut Freq", highCutFreqSlider),
lowCutSlopeSliderAttachement(audioProcessor.apvts, "LowCut Slope", lowCutSlopeSlider),
highCutSlopeSliderAttachement(audioProcessor.apvts, "HighCut Slope", highCutSlopeSlider),

lowcutBypassButtonAttachment(audioProcessor.apvts, "LowCut Bypassed", lowcutBypassButton),
peakBypassButtonAttachment(audioProcessor.apvts, "Peak Bypassed", peakBypassButton),
highcutBypassButtonAttachment(audioProcessor.apvts, "HighCut Bypassed", highcutBypassButton),
analyzerEnabledButtonAttachment(audioProcessor.apvts, "Analyzer Enabled", analyzerEnabledButton)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    peakFreqSlider.labels.add({0.f, "20Hz"});
    peakFreqSlider.labels.add({1.f, "20kHz"});
    
    peakGainSlider.labels.add({0.f, "-24dB"});
    peakGainSlider.labels.add({1.f, "+24dB"});
    
    peakQualitySlider.labels.add({0.f, "0.1"});
    peakQualitySlider.labels.add({1.f, "10.0"});
    
    lowCutFreqSlider.labels.add({0.f, "20Hz"});
    lowCutFreqSlider.labels.add({1.f, "20kHz"});
    
    highCutFreqSlider.labels.add({0.f, "20Hz"});
    highCutFreqSlider.labels.add({1.f, "20kHz"});
    
    lowCutSlopeSlider.labels.add({0.f, "12"});
    lowCutSlopeSlider.labels.add({1.f, "48"});
    
    highCutSlopeSlider.labels.add({0.f, "12"});
    highCutSlopeSlider.labels.add({1.f, "48"});
    
    for(auto* comp : getComps())
    {
        addAndMakeVisible(comp);
    }
    
    lowcutBypassButton.setLookAndFeel(&lnf);
    peakBypassButton.setLookAndFeel(&lnf);
    highcutBypassButton.setLookAndFeel(&lnf);
    analyzerEnabledButton.setLookAndFeel(&lnf);
    
    auto safePtr = juce::Component::SafePointer<SimpleEQAudioProcessorEditor>(this);
       peakBypassButton.onClick = [safePtr]()
       {
           if( auto* comp = safePtr.getComponent() )
           {
               auto bypassed = comp->peakBypassButton.getToggleState();
               
               comp->peakFreqSlider.setEnabled( !bypassed );
               comp->peakGainSlider.setEnabled( !bypassed );
               comp->peakQualitySlider.setEnabled( !bypassed );
           }
       };
       

       lowcutBypassButton.onClick = [safePtr]()
       {
           if( auto* comp = safePtr.getComponent() )
           {
               auto bypassed = comp->lowcutBypassButton.getToggleState();
               
               comp->lowCutFreqSlider.setEnabled( !bypassed );
               comp->lowCutSlopeSlider.setEnabled( !bypassed );
           }
       };
       
       highcutBypassButton.onClick = [safePtr]()
       {
           if( auto* comp = safePtr.getComponent() )
           {
               auto bypassed = comp->highcutBypassButton.getToggleState();
               
               comp->highCutFreqSlider.setEnabled( !bypassed );
               comp->highCutSlopeSlider.setEnabled( !bypassed );
           }
       };
    
        analyzerEnabledButton.onClick = [safePtr]()
        {
            if( auto* comp = safePtr.getComponent() )
            {
                auto enabled = comp->analyzerEnabledButton.getToggleState();
                comp->responseCurveComponent.toggleAnalysisEnablement(enabled);
            }
        };
    
    setSize (600, 500);
}
    

SimpleEQAudioProcessorEditor::~SimpleEQAudioProcessorEditor()
{
    lowcutBypassButton.setLookAndFeel(nullptr);
    peakBypassButton.setLookAndFeel(nullptr);
    highcutBypassButton.setLookAndFeel(nullptr);
    analyzerEnabledButton.setLookAndFeel(nullptr);
}

//==============================================================================
void SimpleEQAudioProcessorEditor::paint (juce::Graphics& g)
{
    using namespace juce;
    
    g.fillAll(ColorScheme::getModuleBackgroundColor());
    
    g.setFont(Font("Iosevka Term Slab", 30, 0)); //https://github.com/be5invis/Iosevka
    
    String title { "Grey Matters EQ" };
    g.setFont(20);
    
    auto bounds = getLocalBounds().withTrimmedTop(5);
    
    g.setColour(ColorScheme::getTitleColor());
    g.drawFittedText(title, bounds, juce::Justification::centredTop, 1);
}

void SimpleEQAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto bounds = getLocalBounds();
    auto analyzerEnabledArea = bounds.removeFromTop(25);
    analyzerEnabledArea.setWidth(100);
    analyzerEnabledArea.setX(5);
    analyzerEnabledArea.removeFromTop(2);
    
    analyzerEnabledButton.setBounds(analyzerEnabledArea);
    
    bounds.removeFromTop(5);
    auto responseArea = bounds.removeFromTop(180);
    
    responseCurveComponent.setBounds(responseArea);
    
    bounds.removeFromTop(5);
    
    auto lowCutArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);
    auto highCutArea = bounds.removeFromRight(bounds.getWidth() * 0.5);
    
    lowcutBypassButton.setBounds(lowCutArea.removeFromTop(25));
    
    lowCutFreqSlider.setBounds(lowCutArea.removeFromTop(lowCutArea.getHeight() * 0.5));
    lowCutSlopeSlider.setBounds(lowCutArea);
    
    highcutBypassButton.setBounds(highCutArea.removeFromTop(25));
    
    highCutFreqSlider.setBounds(highCutArea.removeFromTop(highCutArea.getHeight() * 0.5));
    highCutSlopeSlider.setBounds(highCutArea);
    
    peakBypassButton.setBounds(bounds.removeFromTop(25));
    
    peakFreqSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.33));
    peakGainSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.5));
    peakQualitySlider.setBounds(bounds);
}

std::vector<juce::Component*> SimpleEQAudioProcessorEditor::getComps()
{
    return
    {
        &peakFreqSlider,
        &peakGainSlider,
        &peakQualitySlider,
        &lowCutFreqSlider,
        &highCutFreqSlider,
        &lowCutSlopeSlider,
        &highCutSlopeSlider,
        &responseCurveComponent,
        &lowcutBypassButton,
        &peakBypassButton,
        &highcutBypassButton,
        &analyzerEnabledButton
    };
}
