#include "MidiFile.h"
#include <string>
#include <fstream>
#include <iostream>
#include <list>

//  g++ -std=c++11 main.cpp MidiFile.cpp -o midi_parser
int main() {
  MidiFile midiFile("test-s.mid");
  midiFile.ParseFile("test-s.mid");

  return 0;
}