#pragma once
#include <string>
#include <vector>
#include <list>
#include <cstdint>
#include <iostream>
#include "../Structs/Structs.h"

class MidiFile {
public:
    std::vector<MidiTrack> vecTracks;
    MidiFile() {}
    MidiFile(const std::string& sFilename);
    bool ParseFile(const std::string& sFilename);
};