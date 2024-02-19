/*
  ==============================================================================

    LookAndFeel.h
    Created: 7 Feb 2024 11:09:19pm
    Author:  Ulf Larsson

  ==============================================================================
*/
#include <JuceHeader.h>
#include "CustomButtons.h"

#pragma once

#define USE_LIVE_CONSTANT false

#if USE_LIVE_CONSTANT
#define colorHelper(c) JUCE_LIVE_CONSTANT(c);
#else
#define colorHelper(c) c;
#endif

namespace ColorScheme
{

inline juce::Colour getLeftOutputSignalColor() { return colorHelper(juce::Colour(0xffd0d0d0)); }
inline juce::Colour getRightOutputSignalColor() { return colorHelper(juce::Colour(0xffa0a0a0)); }
inline juce::Colour getSliderFillColor() { return colorHelper(juce::Colour(0xffa8a8a8)); }
inline juce::Colour getSliderRangeTextColor() { return juce::Colours::black; }
inline juce::Colour getSliderTextBoxColor() { return juce::Colours::lightgrey.withAlpha(0.f); }
inline juce::Colour getSliderBorderColor() { return colorHelper(juce::Colour(0xff404040)); }
inline juce::Colour getModuleBackgroundColor() { return juce::Colours::grey; }
inline juce::Colour getModuleBorderColor() { return juce::Colours::orange; }
inline juce::Colour getAnalyzerButtonColor() { return juce::Colours::black; }
inline juce::Colour getToggleButtonColor() { return juce::Colours::black; }
inline juce::Colour getTitleColor() { return juce::Colours::black; }
inline juce::Colour getAnalyzerBackgroundColor() { return juce::Colours::black; }
inline juce::Colour getAnalyzerGridColor() { return juce::Colours::lightgrey; }
inline juce::Colour getFFTLineColor() { return juce::Colours::white;  }
inline juce::Colour getZeroDbColor() { return juce::Colour(0u, 172u, 1u); }
inline juce::Colour getTickColor() { return colorHelper(juce::Colour(0xff404040)); }
inline juce::Colour getMeterLineColor() { return colorHelper(juce::Colour(0xff3c3c3c)); }
inline juce::Colour getScaleTextColor() { return juce::Colours::lightgrey; }
inline juce::Colour getBypassButtonColor() { return juce::Colours::black; }
}


struct LookAndFeel : juce::LookAndFeel_V4
{
    void drawRotarySlider (juce::Graphics&,
                           int x, int y, int width, int height,
                           float sliderPosProportional,
                           float rotaryStartAngle,
                           float rotaryEndAngle,
                           juce::Slider&) override;
    
    void drawToggleButton (juce::Graphics &g,
                           juce::ToggleButton & toggleButton,
                           bool shouldDrawButtonAsHighlighted,
                           bool shouldDrawButtonAsDown) override;
};
