#include "Analyzer.h"
#include "../MidiFile/MidiFile.h"

Analyzer::Analyzer(const std::string& sFilename) : sFilename(sFilename) {
  MidiFile midiFile(sFilename);
}

void Analyzer::Analyze() {
  midifile.ParseFile(sFilename);
  for (const auto& track : midifile.vecTracks) {
    std::cout << "Track Name: " << track.sName << std::endl;
    std::cout << "Instrument: " << track.sInstrument << std::endl;
    for (const auto& event : track.vecEvents) {
      std::cout << "Event Type: " << static_cast<int>(event.event)
                << ", Key: " << static_cast<int>(event.nKey)
                << ", Velocity: " << static_cast<int>(event.nVelocity)
                << ", Delta Tick: " << event.nDeltaTick << std::endl;
    }
  }
}