# VisAmp
A cross-platform C/SDL audio-visual system combining custom display software, embedded video hardware, and a modular high-power audio platform. A lightweight Linux runtime runs on a Raspberry Pi or similar embedded computer, while a more feature-rich Windows application provides a development and preview environment for configuring layouts, album art, metadata, animations, and real-time audio visualizers.

Video output is designed for an analog CRT, bypassing the television's RF tuner and demodulation stages by injecting a baseband NTSC composite signal directly into the CRT's video circuitry for improved signal quality and tighter control over the displayed image.

The audio subsystem consists of a modular 500 W Class-D bass/guitar amplifier and a separate 150 W stereo HiFi amplifier. The 500 W amplifier incorporates a dedicated instrument preamplifier, high-power Class-D output stage, startup muting, protection circuitry, and supporting power-supply and control systems. The modular architecture is intended to support interchangeable preamplifiers, signal processing, wireless audio, and future integration between the audio and CRT visualization systems.

# Current Status
- Programming work is paused until the Daytron DT-505A is fully operational.
- The CRT currently suffers from minor vertical fold-over and requires a recap.
- A composite video injection point has been identified on the DT-505A for use with the Raspberry Pi's analog video output.
- 500W Combo Amp is in the schematic design phase — Initial Amp control/core and fault protection circuitry in place

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

## Amplifier System Overview

Alongside the visualizer, this project includes a custom Class-D instrument amplifier for bass and guitar, designed with the goal of approaching high-end commercial amplifier performance at a substantially lower parts cost. The system also includes a separate 150 W stereo HiFi amplifier intended for conventional music playback through standard speakers.

### Bass/Guitar Amplifier

- **500 W mono output into 4 Ω**
- Uses a shared power stage for both bass and guitar
- Built around an **IRS20957S-family Class-D controller** with external power MOSFETs rather than a highly integrated amplifier IC
- This architecture was chosen over simpler integrated solutions such as the TPA3255 to better support the 500 W output target and provide greater control over the power stage, protection circuitry, switching behavior, and overall system design

Bass and guitar are not intended to operate simultaneously. A hardware selector/interlock switches the active instrument path and associated speaker output, allowing both instruments to share the same high-power output stage. This avoids the cost, thermal load, and power-supply requirements of implementing two independent 500 W amplifier channels.

#### Modular Preamp Architecture

Instrument tone shaping is handled through a modular preamp-card system rather than being permanently integrated into the main amplifier.

Each preamp card can implement its own:

- Input conditioning
- Gain structure
- EQ and filtering
- Tone shaping
- Effects or signal-processing circuitry
- Instrument-specific voicing

Separate bass and guitar cards can therefore be optimized independently while feeding the same clean 500 W power stage.

The card-based architecture also allows new preamp designs to be developed and swapped without redesigning the main amplifier. Future cards could support different tonal characteristics, active EQ designs, effects, alternate instrument inputs, analog modeling, or DSP-based processing.

The overall design philosophy is to keep the Class-D power stage as clean and transparent as practical while allowing the selected preamp card and external pedals to determine the instrument's tonal character.

#### CRT Integration

A conditioned low-level version of the instrument signal is routed to the visualization system so the CRT can display the bass or guitar waveform in real time.

The visualization interface is isolated from the high-power speaker output so that amplifier-level voltages are never applied directly to the Raspberry Pi or other embedded computer.

---

### HiFi Stereo Amplifier

The system also includes a separate stereo amplifier intended for general music playback through conventional loudspeakers rather than musical-instrument amplification.

- **150 W per channel into 4 Ω**
- Operates independently from the bass/guitar amplifier
- Shares the system's approximately **1 kW power supply**
- Designed to allow the HiFi and instrument amplifiers to operate simultaneously within the supply's power and thermal limits

Planned audio sources include:

- Locally stored digital audio
- Vinyl
- Bluetooth
- AirPlay

The playback signal is also routed to the CRT visualization system through a conditioned low-level signal path, allowing real-time waveform and audio-reactive visualization.

When digital metadata is available, the CRT interface can also display information such as:

- Track title
- Artist
- Album
- Album artwork
- Playback status
- Other source or track metadata
