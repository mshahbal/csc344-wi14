/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#define PI 3.14159265359

const float defaultGain = 1.0f;
const float defaultDelay = 0.5f;
const float defaultClip = 1.0f;
//const JuceDemoPluginAudioProcessor::Wave defaultWaveType = JuceDemoPluginAudioProcessor::sine;

//==============================================================================
NewFilterAudioProcessor::NewFilterAudioProcessor()
    : delayBuffer (2, 12000)
{
    // Set up some default values..
    gain = defaultGain;
    delay = defaultDelay;
    clip = defaultClip;
    //waveType = defaultWaveType;
    
    lastUIWidth = 400;
    lastUIHeight = 200;
    
    lastPosInfo.resetToDefault();
    delayPosition = 0;
}

NewFilterAudioProcessor::~NewFilterAudioProcessor()
{
}

//==============================================================================
const String NewFilterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

int NewFilterAudioProcessor::getNumParameters()
{
    return totalNumParams;
}

float NewFilterAudioProcessor::getParameter (int index)
{
    // This method will be called by the host, probably on the audio thread, so
    // it's absolutely time-critical. Don't use critical sections or anything
    // UI-related, or anything at all that may block in any way!
    switch (index)
    {
        case gainParam:     return gain;
        case delayParam:    return delay;
        case clipParam:     return clip;
        default:            return 0.0f;
    }
}

void NewFilterAudioProcessor::setParameter (int index, float newValue)
{
    // This method will be called by the host, probably on the audio thread, so
    // it's absolutely time-critical. Don't use critical sections or anything
    // UI-related, or anything at all that may block in any way!
    switch (index)
    {
        case gainParam:
            gain = newValue;
            break;
        case delayParam:
            delay = newValue;
            break;
        case clipParam:
            clip = newValue;
            break;
        default:
            break;
    }
}

const String NewFilterAudioProcessor::getParameterName (int index)
{
    switch (index)
    {
        case gainParam:     return "gain";
        case delayParam:    return "delay";
        case clipParam:     return "clip";
        default:            break;
    }
    
    return String::empty;
}

const String NewFilterAudioProcessor::getParameterText (int index)
{
    return String (getParameter (index), 2);
}

const String NewFilterAudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String NewFilterAudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool NewFilterAudioProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}

bool NewFilterAudioProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
}

bool NewFilterAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool NewFilterAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool NewFilterAudioProcessor::silenceInProducesSilenceOut() const
{
    return false;
}

double NewFilterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int NewFilterAudioProcessor::getNumPrograms()
{
    return 0;
}

int NewFilterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void NewFilterAudioProcessor::setCurrentProgram (int index)
{
}

const String NewFilterAudioProcessor::getProgramName (int index)
{
    return String::empty;
}

void NewFilterAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void NewFilterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    delayBuffer.clear();
}

void NewFilterAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void NewFilterAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    const int numSamples = buffer.getNumSamples();
    int channel, dp = 0;
    
    // Go through the incoming data, and apply our gain to it...
    for (channel = 0; channel < getNumInputChannels(); ++channel)
        buffer.applyGain (channel, 0, buffer.getNumSamples(), gain);
    
    // Now pass any incoming midi messages to our keyboard state object, and let it
    // add messages to the buffer if the user is clicking on the on-screen keys
    keyboardState.processNextMidiBuffer (midiMessages, 0, numSamples, true);
    
    // and now get the synth to process these midi events and generate its output.
    synth.renderNextBlock (buffer, midiMessages, 0, numSamples);
    
    
    // Apply our delay effect to the new output..
    for (channel = 0; channel < getNumInputChannels(); ++channel)
    {
        float* channelData = buffer.getSampleData (channel);
        float* delayData = delayBuffer.getSampleData (jmin (channel, delayBuffer.getNumChannels() - 1));
        dp = delayPosition;
        
        for (int i = 0; i < numSamples; ++i)
        {
            if (channelData[i] > clip)
                channelData[i] = clip;
            const float in = channelData[i];
            channelData[i] += delayData[dp];
            delayData[dp] = (delayData[dp] + in) * delay;
            
            
            if (++dp >= delayBuffer.getNumSamples())
                dp = 0;
        }
    }
    
    delayPosition = dp;
    

    // In case we have more outputs than inputs, we'll clear any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    for (int i = getNumInputChannels(); i < getNumOutputChannels(); ++i)
    {
        buffer.clear (i, 0, buffer.getNumSamples());
    }
    
    // ask the host for the current time so we can display it...
    AudioPlayHead::CurrentPositionInfo newTime;
    
    if (getPlayHead() != nullptr && getPlayHead()->getCurrentPosition (newTime))
    {
        // Successfully got the current time from the host..
        lastPosInfo = newTime;
    }
    else
    {
        // If the host fails to fill-in the current time, we'll just clear it to a default..
        lastPosInfo.resetToDefault();
    }
}

//==============================================================================
bool NewFilterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* NewFilterAudioProcessor::createEditor()
{
    return new NewFilterAudioProcessorEditor (this);
}

//==============================================================================
void NewFilterAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // Here's an example of how you can use XML to make it easy and more robust:
    
    // Create an outer XML element..
    XmlElement xml ("MYPLUGINSETTINGS");
    
    // add some attributes to it..
    xml.setAttribute ("uiWidth", lastUIWidth);
    xml.setAttribute ("uiHeight", lastUIHeight);
    xml.setAttribute ("gain", gain);
    xml.setAttribute ("delay", delay);
    xml.setAttribute("clip", clip);
    
    // then use this helper function to stuff it into the binary blob and return it..
    copyXmlToBinary (xml, destData);
}

void NewFilterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    // This getXmlFromBinary() helper function retrieves our XML from the binary blob..
    ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    
    if (xmlState != nullptr)
    {
        // make sure that it's actually our type of XML object..
        if (xmlState->hasTagName ("MYPLUGINSETTINGS"))
        {
            // ok, now pull out our parameters..
            lastUIWidth  = xmlState->getIntAttribute ("uiWidth", lastUIWidth);
            lastUIHeight = xmlState->getIntAttribute ("uiHeight", lastUIHeight);
            
            gain  = (float) xmlState->getDoubleAttribute ("gain", gain);
            delay = (float) xmlState->getDoubleAttribute ("delay", delay);
            clip = (float) xmlState->getDoubleAttribute ("clip", clip);
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NewFilterAudioProcessor();
}
