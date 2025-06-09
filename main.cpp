#include "MidiFile.h"
#include <string>
#include <fstream>
#include <iostream>
#include <list>


int main() {
  MidiFile midiFile("test-s.mid");
  midiFile.ParseFile("test-s.mid");

  return 0;
}