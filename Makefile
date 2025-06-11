CXX = g++
CXXFLAGS = -std=c++11

midi_parser: main.cpp ./MidiFile/MidiFile.cpp ./Analyzer/Analyzer.cpp
	$(CXX) $(CXXFLAGS) main.cpp ./MidiFile/MidiFile.cpp ./Analyzer/Analyzer.cpp -o ./_build/midi_parser