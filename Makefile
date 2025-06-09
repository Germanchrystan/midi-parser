CXX = g++
CXXFLAGS = -std=c++11

midi_parser: main.cpp MidiFile.cpp
	$(CXX) $(CXXFLAGS) main.cpp MidiFile.cpp -o midi_parser