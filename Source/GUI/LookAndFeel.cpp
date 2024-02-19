/*
  ==============================================================================

    LookAndFeel.cpp
    Created: 7 Feb 2024 11:09:19pm
    Author:  Ulf Larsson

  ==============================================================================
*/

#include "LookAndFeel.h"
#include "RotarySliderWithLabels.h"

void LookAndFeel::drawRotarySlider(juce::Graphics& g,
                                  int x,
                                  int y,
                                  int width,
                                  int height,
                                  float sliderPosProportional,
                                  float rotaryStartAnggle,
                                  float rotaryEndAngle,
                                  juce::Slider& slider)
{
    using namespace juce;
    
    auto bounds = Rectangle<float>(x, y, width, height);
    
    auto enabled = slider.isEnabled();
    
    g.setColour(enabled ? ColorScheme::getSliderFillColor() : Colours::darkgrey);
    g.fillEllipse(bounds);
    
    g.setColour(enabled ? ColorScheme::getSliderBorderColor() : Colours::grey);
    g.drawEllipse(bounds, 1.f);
    
    if(auto* rswl = dynamic_cast<RotarySliderWithLabels*>(&slider))
    {
        auto center = bounds.getCentre();
        Path p;
        
        Rectangle<float> r;
        r.setLeft(center.getX() - 2);
        r.setRight(center.getX() + 2);
        r.setTop(bounds.getY());
        r.setBottom(center.getY() - rswl->getTextHeight() * 1.5);
        
        p.addRoundedRectangle(r, 2.f);
        p.addRectangle(r);
        
        jassert(rotaryStartAnggle < rotaryEndAngle);
        
        auto sliderAngRad = jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAnggle, rotaryEndAngle);
        
        p.applyTransform(AffineTransform().rotated(sliderAngRad, center.getX(), center.getY()));
        
        g.fillPath(p);
        
        g.setFont(rswl->getTextHeight());
        auto text = rswl->getDisplayString();
        auto strWidth = g.getCurrentFont().getStringWidth(text);
        
        r.setSize(strWidth + 4, rswl->getTextHeight() + 2);
        r.setCentre(bounds.getCentre());
        
        g.setColour(enabled ? ColorScheme::getSliderTextBoxColor() : Colours::darkgrey);
        g.fillRect(r);
        
        g.setColour(enabled ? Colours::black : Colours::lightgrey);
        g.drawFittedText(text, r.toNearestInt(), juce::Justification::centred, 1);
    }
}

void LookAndFeel::drawToggleButton(juce::Graphics &g,
                                    juce::ToggleButton &toggleButton,
                                    bool shouldDrawButtonAsHighlighted,
                                    bool shouldDrawButtonAsDown)
{
    using namespace juce;
    
    if(auto* pb = dynamic_cast<PowerButton*>(&toggleButton))
    {
        Path powerButton;
        
        auto bounds = toggleButton.getLocalBounds();
        auto size = juce::jmin(bounds.getWidth(), bounds.getHeight()) - 10;
        auto r = bounds.withSizeKeepingCentre(size, size).toFloat();
        
        float ang = 30.f;
        
        size -= -6;
        
        powerButton.addCentredArc(r.getCentreX(),
                                  r.getCentreY(),
                                  size * 0.5,
                                  size * 0.5,
                                  0.f,
                                  degreesToRadians(ang),
                                  degreesToRadians(360 - ang),
                                  true);
        powerButton.startNewSubPath(r.getCentreX(), r.getY());
        powerButton.lineTo(r.getCentre());
        
        PathStrokeType pst(2.f, PathStrokeType::JointStyle::curved);
        
        g.strokePath(powerButton, pst);
        
        auto color = toggleButton.getToggleState() ? Colours::dimgrey : ColorScheme::getBypassButtonColor();
        
        g.setColour(color);
        g.strokePath(powerButton, pst);
        g.drawEllipse(r, 2);
    }
    else if( auto* analyzerButton = dynamic_cast<AnalyzerButton*>(&toggleButton) )
    {
        auto color = ! toggleButton.getToggleState() ? Colours::dimgrey : ColorScheme::getAnalyzerButtonColor();
        
        g.setColour(color);
        
        auto bounds = toggleButton.getLocalBounds();
        g.drawRect(bounds);
        
        g.strokePath(analyzerButton->randomPath, PathStrokeType(1.f));
    }
    
}
