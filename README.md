# VisAmp
A cross-platform C/SDL audio-visual system combining custom display software, embedded video hardware, and a modular high-power audio platform. A lightweight Linux runtime runs on a Raspberry Pi or similar embedded computer, while a more feature-rich Windows application provides a development and preview environment for configuring layouts, album art, metadata, animations, and real-time audio visualizers.

Video output is designed for an analog CRT, bypassing the television's RF tuner and demodulation stages by injecting a baseband NTSC composite signal directly into the CRT's video circuitry for improved signal quality and tighter control over the displayed image.

The audio subsystem consists of a modular 500 W Class-D bass/guitar amplifier and a separate 150 W stereo HiFi amplifier. The 500 W amplifier incorporates a dedicated instrument preamplifier, high-power Class-D output stage, startup muting, protection circuitry, and supporting power-supply and control systems. The modular architecture is intended to support interchangeable preamplifiers, signal processing, wireless audio, and future integration between the audio and CRT visualization systems.

# Current Status
- Programming work is paused until the Daytron DT-505A is fully operational.
- The CRT currently suffers from minor vertical fold-over and requires a recap.
- A composite video injection point has been identified on the DT-505A for use with the Raspberry Pi's analog video output.
- 500W Combo Amp is in the schematic design — Initial Amp control/core and fault protection circuitry in place

  ![Initial test of SDL display with test image and waveform](projectImages/birdTestOne.png)
Very quick initial test of SDL libraries to make sure everything works (Final version will contain substantially nicer visuals)

# DT-505A Restoration
A vertical foldover issue is most likely caused by old, failing electrolytic capacitors in the vertical section of the circuit board. The initial foldover problem was diagnosed using a Heathkit test image generator in B&W mode, injected through the original RF input. The image below shows the top of the foldover problem, where the curving of the image is present.

![Vertical Foldover of Test Image](projectImages/IMG_2560.jpeg)

# Composite Injection Overview
The simplest way to display video from a Raspberry Pi on an older television is to pass its composite output through an RF modulator and feed the resulting signal into the TV's antenna input. While convenient, this method introduces several unnecessary stages into the signal path, including RF modulation, the television's tuner and IF circuitry, and subsequent video demodulation. Each additional stage provides another opportunity for interference, noise, and signal degradation.

This is especially noticeable on the Daytron DT-505A, where there is very little RF shielding between the mains transformer and portions of the television's signal-processing circuitry.

A cleaner approach is to inject the Raspberry Pi's composite video signal directly into the television's baseband video path.

After the television's tuner and IF stages recover the incoming broadcast signal, the downstream video circuitry ultimately operates on a baseband NTSC composite signal containing the luminance, synchronization, blanking, and, where applicable, chrominance information required to produce an image. Because the Raspberry Pi can generate a compatible composite NTSC signal directly, its output can be inserted after the RF decoding stages rather than being converted to RF and then demodulated again by the television.

On the Daytron DT-505A, the composite injection point was identified immediately after the video-output pin of the uPC1366C video processor. A through-hole AC-coupling capacitor sits in series with the video path at this location (Red Circle), providing a convenient point to isolate the original signal and connect the Raspberry Pi's composite output to the downstream video circuitry. A video output test point (Pink Circle) also provides an easy diagnostic point next to two parallel resistors (Yellow Circle).

![Series AC decoupling cap for injection point](projectImages/IMG_2557.jpg)

The original RF video path can either be permanently disconnected at this point or retained through the addition of a selector switch, allowing the television to alternate between its original RF input and the externally generated composite signal.

# Integrated Bass/Guitar Amplifier + HiFi Stereo Amplifier
Alongside the visualizer, this project includes a DIY class D instrument amplifier for bass and guitar, designed to match or exceed the quality of a ~$1000 commercial amp at a fraction of the parts cost. There will also be a smaller 150W stereo amp meant for listening to audio through normal speakers, not for musical instruments.

**Bass/Guitar Amp Design Overview:**
- Single shared amp power stage: a discrete class D design built around an IRS20957S-family driver IC plus external power MOSFETs, rather than an integrated class D chip. This was chosen over simpler integrated options (e.g., TPA3255) specifically to hit a 500W target for bass and for increased flexibility with design requirements.
- One mono 500W @ 4 Ohms amp power stage, since bass and guitar are never used simultaneously. A hardware switch/interlock selects one instrument and its speaker output at a time, avoiding the cost, complexity, and shared-rail crosstalk of running two full-power channels.
- Two fully separate preamp paths (bass and guitar), selected by the same switch, so each instrument's tone shaping can be tuned independently.
- Interacts with the CRT to display bass or guitar waveform at a safe signal level (Not sending amplified signal into a small SBC)
- Overall Design philosophy: keep the power amp stage clean and transparent, and let all tone character come from the preamps and pedals rather than the power stage itself.

**HiFi Stereo Amp Design Overview:**
- Shares the same 1kW power supply as the bass/guitar amp system and is designed to run at the same time as the other amp.
- 150W stereo output preamp + amp meant for listening to audio through a set of traditional stereo speakers.
- Rated for 150W @ 4 Ohms.
- Audio can come in the form of a locally stored audio file, vinyl, or Bluetooth/Airplay.
- Interacts with the CRT to create an audio waveform and displays the song's metadata.

