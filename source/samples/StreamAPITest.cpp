#include "MicroBit.h"
#include "CodalAssert.h"
#include "LevelDetector.h"
#include "LevelDetectorSPL.h"
#include "StreamRecording.h"
#include "Tests.h"

/**
 * Note - These tests use CodalAssert.h - so require the following codal.json options
 * be enabled:
 * CODAL_ENABLE_ASSERT: 1
 * 
 * To cause the first assertion failure to halt the processor, also include:
 * CODAL_ASSERT_PANIC: 1
 */

extern MicroBit uBit;

/**
 * Tests if the mic auto activation (and ADC activation) works correctly
 */
void stream_test_mic_activate() {
    assert( uBit.audio.isMicrophoneEnabled() == false, "Microphone was enabled on startup?" );
    assert( uBit.audio.mic->output.isFlowing() == false, "isFlowing() should be false on startup." );
    int level = uBit.audio.levelSPL->getValue();
    assert( uBit.audio.isMicrophoneEnabled(), "getValue() failed to turn the MIC on!" );
    assert( uBit.audio.mic->output.isFlowing(), "isFlowing() was not true after mic activation" );
    assert( level > 0, "Detected level appears to be zero? Defective hardware?" );
    assert_pass( NULL );
}

void stream_test_getValue_interval() {
    uBit.audio.levelSPL->getValue(); // Wake the stream :)
    for( int i=0; i<10; i++ ) {
        assert( uBit.audio.isMicrophoneEnabled(), "Microphone should still be active" );
        assert( uBit.audio.mic->output.isFlowing(), "isFlowing() should still be true" );
        int level = uBit.audio.levelSPL->getValue();
        assert( level > 0, "Detected level appears to be zero? Defective hardware?" );
        uBit.sleep( CODAL_STREAM_IDLE_TIMEOUT_MS / 2 );
    }
    uBit.sleep( CODAL_STREAM_IDLE_TIMEOUT_MS * 2 ); // Ensure we go quiscient
    assert( uBit.audio.isMicrophoneEnabled() == false, "Microphone should be shut down after 2 * CODAL_STREAM_IDLE_TIMEOUT_MS" );
    assert( uBit.audio.mic->output.isFlowing() == false, "isFlowing() should be false after 2 * CODAL_STREAM_IDLE_TIMEOUT_MS" );
    assert_pass( NULL );
}

void stream_test_record() {
    uBit.audio.requestActivation();
    static SplitterChannel * input = uBit.audio.splitter->createChannel();
    static StreamRecording * recording = new StreamRecording( *input );
    static MixerChannel * output = uBit.audio.mixer.addChannel( *recording );

    input->requestSampleRate( 11000 );
    output->setSampleRate( 11000 );
    output->setVolume( CONFIG_MIXER_INTERNAL_RANGE * 0.8 ); // 80% volume

    uBit.display.printChar( '3', 1000 );
    uBit.display.printChar( '2', 1000 );
    uBit.display.printChar( '1', 1000 );

    uBit.display.printChar( 'R' );
    input->requestSampleRate( 11000 );
    recording->recordAsync();
    while( recording->isRecording() ) {
        uBit.display.printChar( '~' );
        uBit.sleep( 100 );
        uBit.display.printChar( '-' );
        uBit.sleep( 100 );
    }
    uBit.display.printChar( 'X' );

    uBit.sleep( 1000 );

    uBit.display.printChar( 'P' );
    output->setSampleRate( 18000 );
    recording->playAsync();
    while( recording->isPlaying() ) {
        uBit.display.printChar( '>' );
        uBit.sleep( 100 );
        uBit.display.printChar( ' ' );
        uBit.sleep( 100 );
    }
    uBit.display.printChar( 'X' );

    uBit.sleep( 1000 );

    recording->erase();
}

void stream_test_all() {
    stream_test_mic_activate();
    stream_test_getValue_interval();
    assert_pass( NULL );
}