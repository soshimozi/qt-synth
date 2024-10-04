# Qt-Synth

Qt-Synth is a software synthesizer written in C++ with a user interface built using Qt. It aims to emulate the classic sound and functionality of the **Moog Prodigy** analog synthesizer, providing users with a rich, vintage sound experience in a modern, cross-platform application.

## Features

- **Oscillators**: Dual oscillators with selectable waveforms (saw, triangle, pulse) to mimic the Moog Prodigy's oscillator capabilities.
- **Filter**: Classic low-pass filter with resonance control for shaping sound.
- **Envelope Generators**: ADS (Attack, Decay, Sustain) envelope control for volume and filter shaping.
- **Modulation**: LFO (Low Frequency Oscillator) to modulate pitch or filter cutoff for classic synthesizer effects.
- **Arpeggiator**: Simple arpeggiator to create patterns with held notes.
- **MIDI Support**: Ability to receive MIDI input for real-time performance.
- **Preset Management**: Save and load custom sound presets.
- **Cross-Platform**: Built with Qt, Qt-Synth can run on Windows, macOS, and Linux.

## Installation

### Prerequisites

- **Qt Framework**: Ensure you have the Qt development libraries installed.
- **C++ Compiler**: Compatible with any modern C++ compiler (GCC, Clang, MSVC).
- **CMake**: Build system generator used to compile the project.

### Build Instructions

1. Clone the repository:
   ```bash
   git clone https://github.com/your-username/qt-synth.git
   cd qt-synth
