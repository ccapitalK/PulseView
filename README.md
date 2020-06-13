# PulseView

## What is this

A simple SFML based oscilloscope for pulseaudio. Also renders DFTs for each channel.

## Dependencies

- Pacat
- Pulseaudio
- fftw3
- Pthreads
- A C++17 compatible version of g++

## Connecting to other audio sources

Currently, this application does not have any way of choosing which audio source to use.
It is however possible to choose an audio source through the use of pavucontrol.
