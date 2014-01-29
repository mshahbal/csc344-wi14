/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic startup code for a Juce application.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
/** A synth sound for project 2 */
class P2Sound : public SynthesiserSound
{
public:
    P2Sound() {}
    
    bool appliesToNote (const int /*midiNoteNumber*/)           { return true; }
    bool appliesToChannel (const int /*midiChannel*/)           { return true; }
};

//==============================================================================
/** A Wavetable **/

// NB: ANGLES SPECIFIED IN ROTATIONS, NOT RADIANS.

#define WAVETABLE_SIZE 1000
// this is one larger than needed, to prevent
// having to special-case the wraparound.
#define WAVETABLE_ARRAY_SIZE (WAVETABLE_SIZE + 1)
class WaveTable
{
public:
    float table[WAVETABLE_ARRAY_SIZE];
    
    // given a number in 0<angle<1.0, return
    // a (linearly interpolated) number from the wavetable.
    float lookup(double angle){
        // yes, we could avoid all of the multiplication if we
        // pushed the wavetable size into the angle....
        const double scaled_angle = angle * WAVETABLE_SIZE;
        const double lower = floor(scaled_angle);
        const double fraction = scaled_angle - lower;
        return (table[(int)lower] * (1.0 - fraction))
        + (table[(int)lower+1] * fraction);
    }
    
    void squareInit(){
        for (int i = 0; i < WAVETABLE_SIZE; i++){
            table[i] =  ((i < (WAVETABLE_SIZE/2)) ? 1.0 : -1.0);
        }
        table[WAVETABLE_SIZE] = 1.0;
    }
    
    void sineInit(){
        for (int i = 0; i < WAVETABLE_SIZE; i++){
            table[i] = sin(2.0 * double_Pi * ((double)i / WAVETABLE_SIZE));
        }
        table[WAVETABLE_SIZE] = 0.0;
    }
    
    /*    void triangleInit(){
     
     }*/
};


//==============================================================================
/** A Voice for Project 2 */

#define HARMONICS 4

class P2Voice  : public SynthesiserVoice
{
public:
    P2Voice()
    : playing(notPlaying),
    angleDelta (0.0),
    tailOff (0.0),
    wavetable()
    {
        wavetable.squareInit();
    }
    
    bool canPlaySound (SynthesiserSound* sound)
    {
        return dynamic_cast <P2Sound*> (sound) != 0;
    }
    
    void startNote (int midiNoteNumber, float velocity,
                    SynthesiserSound* /*sound*/, int /*currentPitchWheelPosition*/)
    {
        playing = keyHeld;
        currentAngle = 0.0;
        
        for (int i = 0; i < HARMONICS; i++) {
            levels[i] = 0.25;
        }
        level = velocity * 0.15;
        
        const double cyclesPerSecond = MidiMessage::getMidiNoteInHertz (midiNoteNumber);
        const double cyclesPerSample = cyclesPerSecond / getSampleRate();
        
        // the angleDelta for the base frequency.
        angleDelta = cyclesPerSample;
        
        // reset o1
        o1_angle = 0.0;
        o1_angleDelta = o1_freq / getSampleRate();
    }
    
    void stopNote (bool allowTailOff)
    {
        if (allowTailOff)
        {
            // start a tail-off by setting this flag. The render callback will pick up on
            // this and do a fade out, calling clearCurrentNote() when it's finished.
            if (playing == keyHeld) {
                // we only need to begin a tail-off if it's not already doing so - the
                // stopNote method could be called more than once.
                playing = keyReleased;
                tailOff = 1.0;
            }
        }
        else
        {
            // we're being told to stop playing immediately, so reset everything..
            clearCurrentNote();
            playing = notPlaying;
            angleDelta = 0.0;
        }
    }
    
    void pitchWheelMoved (int /*newValue*/)
    {
        // can't be bothered implementing this for the demo!
    }
    
    void controllerMoved (int /*controllerNumber*/, int /*newValue*/)
    {
        // not interested in controllers in this case.
    }
    
    void renderNextBlock (AudioSampleBuffer& outputBuffer, int startSample, int numSamples)
    {
        if (playing != notPlaying) {
            const double levelMult = level * (playing ==  keyReleased ? tailOff : 1.0);
            for (int sample = startSample; sample < startSample + numSamples; sample++){
                const double o1 = (sin (o1_angle * 2.0 * double_Pi));
                const double amplitude = 1.0 + (0.5 * o1);
                const float currentSampleVal =
                (float) (wavetable.lookup(currentAngle) * levelMult * amplitude);
                
                for (int i = outputBuffer.getNumChannels(); --i >= 0;) {
                    *outputBuffer.getSampleData (i, sample) += currentSampleVal;
                }
                
                currentAngle = angleWrap(currentAngle + angleDelta);
                o1_angle = angleWrap(o1_angle + o1_angleDelta);
                
                if (playing == keyReleased) {
                    tailOff *= 0.99;
                    
                    if (tailOff <= 0.005)
                    {
                        clearCurrentNote();
                        playing = notPlaying;
                        angleDelta = 0.0;
                        break;
                    }
                }
            }
        }
    }
    
private:
    // wrap an angle around. ASSUMES IT'S NOT GREATER THAN 2.
    double angleWrap(double angle) const {
        return (angle > 1.0 ? angle - 1.0 : angle);
    }
    
    enum PlayState
    {
        notPlaying = 0,
        keyHeld,
        keyReleased
    };
    
    
    PlayState playing;
    double angles[HARMONICS];
    double levels[HARMONICS];
    // all angles are measured in *rotations*, not radians.
    double currentAngle, angleDelta;
    double level, tailOff;
    // osc1
    double o1_freq = 2;
    double o1_angle, o1_angleDelta;
    // a wavetable
    WaveTable wavetable;
};



//==============================================================================
Assignment2AudioProcessor::Assignment2AudioProcessor()
{
    // Set up some default values..
    lastUIWidth = 400;
    lastUIHeight = 200;
    
    lastPosInfo.resetToDefault();
    
    // Initialise the synth...
    for (int i = 4; --i >= 0;)
        synth.addVoice (new P2Voice());   // These voices will play our custom sine-wave sounds..
    
    synth.addSound (new P2Sound());
}

Assignment2AudioProcessor::~Assignment2AudioProcessor()
{
}

//==============================================================================
const String Assignment2AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

int Assignment2AudioProcessor::getNumParameters()
{
    return 0;
}

float Assignment2AudioProcessor::getParameter (int index)
{
    return 0.0f;
}

void Assignment2AudioProcessor::setParameter (int index, float newValue)
{
}

const String Assignment2AudioProcessor::getParameterName (int index)
{
    return String::empty;
}

const String Assignment2AudioProcessor::getParameterText (int index)
{
    return String::empty;
}

const String Assignment2AudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String Assignment2AudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool Assignment2AudioProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}

bool Assignment2AudioProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
}

bool Assignment2AudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool Assignment2AudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool Assignment2AudioProcessor::silenceInProducesSilenceOut() const
{
    return false;
}

double Assignment2AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Assignment2AudioProcessor::getNumPrograms()
{
    return 0;
}

int Assignment2AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Assignment2AudioProcessor::setCurrentProgram (int index)
{
}

const String Assignment2AudioProcessor::getProgramName (int index)
{
    return String::empty;
}

void Assignment2AudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void Assignment2AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    synth.setCurrentPlaybackSampleRate (sampleRate);
    keyboardState.reset();
    //delayBuffer.clear();
}

void Assignment2AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    
    // not sure why this is necessary? -- JBC 2014-01-23
    keyboardState.reset();
}

void Assignment2AudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    const int numSamples = buffer.getNumSamples();
    
    // output buffers will initially be garbage, must be cleared:
    for (int i = 0; i < getNumOutputChannels(); ++i) {
        buffer.clear (i, 0, numSamples);
    }
    
    // Now pass any incoming midi messages to our keyboard state object, and let it
    // add messages to the buffer if the user is clicking on the on-screen keys
    keyboardState.processNextMidiBuffer (midiMessages, 0, numSamples, true);
    
    // and now get the synth to process these midi events and generate its output.
    synth.renderNextBlock (buffer, midiMessages, 0, numSamples);
    
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
bool Assignment2AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* Assignment2AudioProcessor::createEditor()
{
    return new Assignment2AudioProcessorEditor (this);
}

//==============================================================================
void Assignment2AudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Assignment2AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Assignment2AudioProcessor();
}