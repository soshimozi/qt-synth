#include "voice.h"
#include <cmath>


// Helper function to convert note numbers to frequency.
float frequencyFromNoteNumber(int note) {
    // Example formula, replace with actual conversion logic.
    return 440.0 * std::pow(2.0, (note - 69) / 12.0);
}

Voice::Voice(int noteIndex, float velocity) {
    m_originalFrequency = frequencyFromNoteNumber(noteIndex);

    // build audio chain
}
