# CRTMusicVisualizer
A cross-platform C/SDL music display system featuring a lightweight Linux runtime for embedded computers and a more feature-rich Windows previewer for customizing layouts, album art, metadata, animations, and real-time visualizers before deployment to an analog CRT through direct composite video injection.

The system is designed to bypass the television's RF input chain entirely, allowing a Raspberry Pi or similar computer to provide a cleaner baseband NTSC video signal directly to the CRT's video circuitry.

# Current Status
- Programming work is paused until the Daytron DT-505A is fully operational.
- The CRT currently suffers from minor vertical fold-over and requires a recap.
- A composite video injection point has been identified on the DT-505A for use with the Raspberry Pi's analog video output.

![Initial test of SDL display with test image and waveform](projectImages/birdTestOne.png)
Very quick initial test of SDL libraries to make sure everything works (Final version will contain substantially nicer visuals)

# Composite Injection Overview
The simplest way to display video from a Raspberry Pi on an older television is to pass its composite output through an RF modulator and feed the resulting signal into the TV's antenna input. While convenient, this method introduces several unnecessary stages into the signal path, including RF modulation, the television's tuner and IF circuitry, and subsequent video demodulation. Each additional stage provides another opportunity for interference, noise, and signal degradation.

This is especially noticeable on the Daytron DT-505A, where there is very little RF shielding between the mains transformer and portions of the television's signal-processing circuitry.

A cleaner approach is to inject the Raspberry Pi's composite video signal directly into the television's baseband video path.

After the television's tuner and IF stages recover the incoming broadcast signal, the downstream video circuitry ultimately operates on a baseband NTSC composite signal containing the luminance, synchronization, blanking, and, where applicable, chrominance information required to produce an image. Because the Raspberry Pi can generate a compatible composite NTSC signal directly, its output can be inserted after the RF decoding stages rather than being converted to RF and then demodulated again by the television.

On the Daytron DT-505A, the composite injection point was identified immediately after the video-output pin of the uPC1366C video processor. A through-hole AC-coupling capacitor sits in series with the video path at this location, providing a convenient point to isolate the original signal and connect the Raspberry Pi's composite output to the downstream video circuitry.

The original RF video path can either be permanently disconnected at this point or retained through the addition of a selector switch, allowing the television to alternate between its original RF input and the externally generated composite signal.
