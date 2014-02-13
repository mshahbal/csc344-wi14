/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


//==============================================================================
/**
*/
class NewFilterAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    NewFilterAudioProcessorEditor (NewFilterAudioProcessor* ownerFilter);
    ~NewFilterAudioProcessorEditor();

    //==============================================================================
    // This is just a standard Juce paint method...
    void paint (Graphics& g);
};


#endif  // PLUGINEDITOR_H_INCLUDED
