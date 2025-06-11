#include "./Analyzer/Analyzer.h"
#include <string>
#include <fstream>
#include <iostream>
#include <list>

//  g++ -std=c++11 main.cpp MidiFile.cpp -o midi_parser
int main() {
  Analyzer analyzer("test-s.mid");
  analyzer.Analyze();

  return 0;
}