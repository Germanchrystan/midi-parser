#pragma once
#include "../MidiFile/MidiFile.h"

class Analyzer {
  private:
    // std::vector<MidiFile::MidiTrack> vecTracks;
    std::string sFilename;
    MidiFile midifile;
public:
    Analyzer(const std::string& sFilename);
    void Analyze();
};