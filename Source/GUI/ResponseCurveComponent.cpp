/*
  ==============================================================================

    ResponseCurveComponent.cpp
    Created: 8 Feb 2024 12:07:51am
    Author:  Ulf Larsson

  ==============================================================================
*/

#include "ResponseCurveComponent.h"
#include "LookAndFeel.h"
#include "Utilities.h"

ResponseCurveComponent::ResponseCurveComponent(SimpleEQAudioProcessor& p) :
audioProcessor(p),
leftPathProducer(audioProcessor.leftChannelFifo),
rightPathProducer(audioProcessor.rightChannelFifo)
{
    const auto& params = audioProcessor.getParameters();
    for (auto param : params) {
        param->addListener(this);
    }
    
    updateChain();
    
    startTimerHz(60);
}

ResponseCurveComponent::~ResponseCurveComponent()
{
    const auto& params = audioProcessor.getParameters();
    for (auto param : params) {
        param->removeListener(this);
    }
}

void ResponseCurveComponent::parameterValueChanged(int parameterIndex, float newValue)
{
    parametersChanged = true;
}



void ResponseCurveComponent::timerCallback()
{
    if(shouldShowFFTAnalysis)
    {
        auto fftBounds = getAnalysisArea().toFloat();
        auto sampleRate = audioProcessor.getSampleRate();
    
        leftPathProducer.process(fftBounds, sampleRate);
        rightPathProducer.process(fftBounds, sampleRate);
    }
    
    if(parametersChanged.compareAndSetBool(false, true))
    {
        updateChain();
    }
    repaint();
}

void ResponseCurveComponent::updateChain()
{
    auto chainSettings = getChainSettings(audioProcessor.apvts);
    
    monoChain.setBypassed<ChainPositions::LowCut>(chainSettings.lowCutBypassed);
    monoChain.setBypassed<ChainPositions::Peak>(chainSettings.peakBypassed);
    monoChain.setBypassed<ChainPositions::HighCut>(chainSettings.highCutBypassed);
    
    auto peakCoefficients = makePeakFilter(chainSettings, audioProcessor.getSampleRate());
    updateCoefficients(monoChain.get<ChainPositions::Peak>().coefficients, peakCoefficients);
    
    auto lowCutCoefficients = makeLowCutFilter(chainSettings, audioProcessor.getSampleRate());
    auto highCutCoefficients = makeHighCutFilter(chainSettings, audioProcessor.getSampleRate());
    
    updateCutFilter(monoChain.get<ChainPositions::LowCut>(), lowCutCoefficients, chainSettings.lowCutSlope);
    updateCutFilter(monoChain.get<ChainPositions::HighCut>(), highCutCoefficients, chainSettings.highCutSlope);
}

void ResponseCurveComponent::paint (juce::Graphics& g)
{
    using namespace juce;
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (ColorScheme::getAnalyzerBackgroundColor());
    
    g.drawImage(background, getLocalBounds().toFloat());
    
    auto responseArea = getAnalysisArea();
    auto w = responseArea.getWidth();
    
    auto& lowcut = monoChain.get<ChainPositions::LowCut>();
    auto& peak = monoChain.get<ChainPositions::Peak>();
    auto& highcut = monoChain.get<ChainPositions::HighCut>();
    
    auto samplerate = audioProcessor.getSampleRate();
    
    std::vector<double> mags;
    mags.resize(w);
    
    for (int i = 0; i < w; ++i) {
        double mag = 1.f;
        auto freq = mapToLog10(double(i) / double(w), double(MIN_FREQUENCY), double(MAX_FREQUENCY));
        
        if(!monoChain.isBypassed<ChainPositions::Peak>())
            mag *= peak.coefficients->getMagnitudeForFrequency(freq, samplerate);
        
        
        if(!monoChain.isBypassed<ChainPositions::LowCut>())
        {
            if(!lowcut.isBypassed<0>())
                mag *= lowcut.get<0>().coefficients->getMagnitudeForFrequency(freq, samplerate);
            if(!lowcut.isBypassed<1>())
                mag *= lowcut.get<1>().coefficients->getMagnitudeForFrequency(freq, samplerate);
            if(!lowcut.isBypassed<2>())
                mag *= lowcut.get<2>().coefficients->getMagnitudeForFrequency(freq, samplerate);
            if(!lowcut.isBypassed<3>())
                mag *= lowcut.get<3>().coefficients->getMagnitudeForFrequency(freq, samplerate);
        }
        if(!monoChain.isBypassed<ChainPositions::HighCut>())
        {
            if(!highcut.isBypassed<0>())
                mag *= highcut.get<0>().coefficients->getMagnitudeForFrequency(freq, samplerate);
            if(!highcut.isBypassed<1>())
                mag *= highcut.get<1>().coefficients->getMagnitudeForFrequency(freq, samplerate);
            if(!highcut.isBypassed<2>())
                mag *= highcut.get<2>().coefficients->getMagnitudeForFrequency(freq, samplerate);
            if(!highcut.isBypassed<3>())
                mag *= highcut.get<3>().coefficients->getMagnitudeForFrequency(freq, samplerate);
        }
        
        mags[i] = Decibels::gainToDecibels(mag);
    }
    
    Path responseCurve;

    const double outputMin = responseArea.getBottom();
    const double outputMax = responseArea.getY();
    auto map = [outputMin, outputMax](double input)
    {
        return jmap(input, double(MIN_GAIN), double(MAX_GAIN), outputMin, outputMax);
        
    };
    
    responseCurve.startNewSubPath(responseArea.getX(), map(mags.front()));
    
    for (size_t i = 1; i < mags.size(); ++i) {
        responseCurve.lineTo(responseArea.getX() + i, map(mags[i]));
    }
    
    if(shouldShowFFTAnalysis)
    {
        drawFFTAnalysis(g, responseArea);
    }
    
    g.setColour(ColorScheme::getModuleBorderColor());
    g.drawRoundedRectangle(getRenderArea().toFloat(), 4.f, 1.f);
    g.setColour(ColorScheme::getFFTLineColor());
    g.strokePath(responseCurve, PathStrokeType(2.f));
}

void ResponseCurveComponent::drawFFTAnalysis(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    using namespace juce;
    auto responseArea = getAnalysisArea();
    
    auto leftChannelFFTPath = leftPathProducer.getPath();
    
    leftChannelFFTPath.applyTransform(AffineTransform().translation(responseArea.getX(), responseArea.getY()));
    
    g.setColour(ColorScheme::getLeftOutputSignalColor());
    g.strokePath(leftChannelFFTPath, PathStrokeType(1));
    
    auto rightChannelFFTPath = rightPathProducer.getPath();
    
    rightChannelFFTPath.applyTransform(AffineTransform().translation(responseArea.getX(), responseArea.getY()));
    
    g.setColour(ColorScheme::getRightOutputSignalColor());
    g.strokePath(rightChannelFFTPath, PathStrokeType(1));
    
}

void ResponseCurveComponent::resized()
{
    using namespace juce;
    background = Image(Image::PixelFormat::RGB, getWidth(), getHeight(), true);

    Graphics g(background);

    auto bounds = getLocalBounds();
    
    drawBackgroundGrid(g, bounds);
    
    drawTextLabels(g, bounds);
}

void ResponseCurveComponent::drawBackgroundGrid(juce::Graphics &g,
                                          juce::Rectangle<int> bounds)
{
    using namespace juce;
    
    auto renderArea = getAnalysisArea();
    auto freqs = getFrequencies();
    auto left = renderArea.getX();
    auto right = renderArea.getRight();
    auto top = renderArea.getY();
    auto bottom = renderArea.getBottom();
    auto width = renderArea.getWidth();
    
    auto xs = getXs(freqs, left, width);
    
    g.setColour(ColorScheme::getAnalyzerGridColor());
    for (auto x : xs)
    {
        g.drawVerticalLine(x, top, bottom);
    }
    
    auto gain = getGains();
       
    for(auto gDb : gain)
    {
        auto y = jmap(gDb, MIN_GAIN, MAX_GAIN, float(bottom), float(top));
        
        g.setColour(gDb == 0.f ? ColorScheme::getZeroDbColor() : Colours::darkgrey);
        g.drawHorizontalLine(y, left, right);
    }
}

std::vector<float> ResponseCurveComponent::getFrequencies()
{
    return std::vector<float>
    {
        20, 40, 60, 100,
        200, 300, 500, 1000,
        2000, 3000, 5000, 10000,
        20000
    };
}

std::vector<float> ResponseCurveComponent::getGains()
{
    std::vector<float> values;
    
    auto increment = GAIN_STEP_SIZE;
    for (auto db = MIN_GAIN; db <= MAX_GAIN; db += increment) {
        values.push_back(db);
    }
    
    return values;
}

std::vector<float> ResponseCurveComponent::getXs(const std::vector<float> &freqs, float left, float width)
{
    std::vector<float> xs;
    for( auto f : freqs )
    {
        auto normX = juce::mapFromLog10(f, MIN_FREQUENCY, MAX_FREQUENCY);
        xs.push_back( left + width * normX );
    }
    
    return xs;
}

void ResponseCurveComponent::drawTextLabels(juce::Graphics &g, juce::Rectangle<int> bounds)
{
    using namespace juce;
    g.setColour(ColorScheme::getScaleTextColor());
    const int fontHeight = 10;
    g.setFont(fontHeight);
    
    auto renderArea = getAnalysisArea();
    auto top = renderArea.getY();
    auto bottom = renderArea.getBottom();
    auto left = renderArea.getX();
    auto width = renderArea.getWidth();
    
    auto freqs = getFrequencies();
    auto xs = getXs(freqs, left, width);
    
    for( int i = 0; i < freqs.size(); ++i )
    {
        auto f = freqs[i];
        auto x = xs[i];
        
        bool addK = false;
        String str;
        if( f > 999.f )
        {
            addK = true;
            f /= 1000.f;
        }
        
        str << f;
        if( addK )
            str << "k";
        str << "Hz";
        
        auto textWidth = g.getCurrentFont().getStringWidth(str);
        
        Rectangle<int> r;
        
        r.setSize(textWidth, fontHeight);
        r.setCentre(x, 0);
        r.setY(bounds.getY());
        
        g.drawFittedText(str, r, juce::Justification::centred, 1);
    }
    
    auto gain = getGains();
    
    for(auto gDb : gain)
    {
        auto y = jmap(gDb, MIN_GAIN, MAX_GAIN, float(bottom), float(top));
        
        String str;
        if(gDb > 0)
            str << "+";
        str << gDb;
        
        auto textWidth = g.getCurrentFont().getStringWidth(str);
        
        Rectangle<int> r;
        r.setSize(textWidth, fontHeight);
        r.setX(getWidth() - textWidth);
        r.setCentre(r.getCentreX(), y);
        
        g.setColour(gDb == 0.f ? ColorScheme::getZeroDbColor() : Colours::lightgrey);
        
        g.drawFittedText(str, r, Justification::centred, 1);
        
        str.clear();
        str << (gDb - MAX_GAIN);
        
        r.setX(1);
        g.setColour(Colours::lightgrey);
        g.drawFittedText(str, r, Justification::centred, 1);
    }
}

juce::Rectangle<int> ResponseCurveComponent::getRenderArea()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop(12);
    bounds.removeFromBottom(2);
    bounds.removeFromLeft(20);
    bounds.removeFromRight(20);
    return bounds;
}

juce::Rectangle<int> ResponseCurveComponent::getAnalysisArea()
{
    auto bounds = getRenderArea();
    bounds.removeFromTop(4);
    bounds.removeFromBottom(4);
    return bounds;
}
