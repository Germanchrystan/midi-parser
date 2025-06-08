#include <string>
#include <fstream>
#include <iostream>
#include <list>

struct MidiEvent {
  enum class Type {
    NoteOff,
    NoteOn,
    Other
  } event;

  uint8_t nKey = 0;
  uint8_t nVelocity = 0;
  uint32_t nWallTick = 0;
  uint32_t nDeltaTick = 0;
};

struct MidiNote {
  uint8_t nKey = 0;
  uint8_t nVelocity = 0;
  uint32_t nStartTime = 0;
  uint32_t nDuration = 0;
};

struct MidiTrack {
  std::string sName;
  std::string sInstrument;
  std::vector<MidiEvent> vecEvents;
  std::vector<MidiNote> vecNotes;
  uint8_t nMaxNote = 64;
  uint8_t nMinNote = 64;
};

class MidiFile {
  enum EventName : uint8_t {
    VoiceNoteOff = 0x80,
    VoiceNoteOn = 0x90,
    VoiceAfterTouch = 0xA0,
    VoiceControlChange = 0xB0,
    VoiceProgramChange = 0xC0,
    VoiceChannelPressure = 0xD0,
    VoicePitchBend = 0xE0,
    SystemExclusive = 0xF0,
  };

  enum MetaEventName : uint8_t {
    MetaSequence = 0x00,
    MetaText = 0x01,
    MetaCopyright = 0x02,
    MetaTrackName = 0x03,
    MetaInstrumentName = 0x04,
    MetaLyric = 0x05,
    MetaMarker = 0x06,
    MetaCuePoint = 0x07,
    MetaChannelPrefix = 0x20,
    MetaEndOfTrack = 0x2F,
    MetaSetTempo = 0x51,
    MetaSMPTEOffset = 0x54,
    MetaTimeSignature = 0x58,
    MetaKeySignature = 0x59,
    MetaSequencerSpecific = 0x7F,
  };

  public: 
    std::vector<MidiTrack> vecTracks;
    MidiFile() {}
    MidiFile(const std::string& sFilename) {
      ParseFile(sFilename);
    }

    bool ParseFile(const std::string& sFilename) {
      // Open the MIDI File as a stream
      std::ifstream ifs;
      ifs.open(sFilename, std::ios::in | std::ios::binary);
      if(!ifs.is_open()) {
        return false; // Failed to open file
      }

      // Swaps byte order of 32-bit integers
      auto Swap32 = [](uint32_t n) {
        return (((n >> 24) & 0xff) | ((n << 8) & 0xff0000) |
                ((n >> 8) & 0xff00) | ((n << 24) & 0xff000000));
      };

      // Swap byte order of 16-bit integers
      auto Swap16 = [](uint16_t n) {
        return (n >> 8) | (n << 8);
      };
      
      // Reads nLength bytes from file stream, and constructs a text string
      auto ReadString = [&ifs](size_t nLength) {
        std::string s;
        for (uint32_t i = 0; i < nLength; i++) s += ifs.get();
        return s;
      };

      auto ReadValue = [&ifs]() {
        uint32_t nValue = 0;
        uint8_t nByte = 0;

        // Read byte
        nValue = ifs.get();
        // Check MSB, if set more bytes need reading
        if (nValue & 0x80) {
          // Extract bottom 7 bits of read byte
          nValue &= 0x7F;

          do {
            // Read next byte
            nByte = ifs.get();
            // Construct value by setting bottom 7 bits, then shifting 7 bits
            nValue = (nValue << 7) | (nByte & 0x7F);
          } while (nByte & 0x80); // Continue until MSB is not set
        }

        return nValue;
      };

      // Parse MIDI file
      uint32_t n32 = 0;
      uint16_t n16 = 0;

      // Read MIDI Header (Fixed size)
      ifs.read((char*)&n32, sizeof(uint32_t));
      uint32_t nFileID = Swap32(n32);
      ifs.read((char*)&n32, sizeof(uint32_t));
      uint32_t nHeaderLenght = Swap32(n32);
      ifs.read((char*)&n16, sizeof(uint16_t));
      uint16_t nFormatType = Swap16(n16);
      ifs.read((char*)&n16, sizeof(uint16_t));
      uint16_t nTrackChunks = Swap16(n16); // Number of track chunks
      ifs.read((char*)&n16, sizeof(uint16_t));
      uint16_t nDivision = Swap16(n16);

      for (uint16_t nChunk = 0; nChunk < nTrackChunks; nChunk++) {
        std::cout<< "========= NEW TRACK =========" << std::endl;
        // Read Track Header
        ifs.read((char*)&n32, sizeof(uint32_t));
        uint32_t nTrackID = Swap32(n32);
        if (nTrackID != 0x4D54726B) { // 'MTrk' in ASCII
          std::cerr << "Invalid track ID: " << std::hex << nTrackID << std::dec << std::endl;
          return false; // Invalid track ID
        }
        ifs.read((char*)&n32, sizeof(uint32_t));
        uint32_t nTrackLength = Swap32(n32);

        bool bEndOfTrack = false;
        int8_t nPreviousStatus = 0;

        vecTracks.push_back(MidiTrack());
        
        // Read MIDI Events until end of track
        while (!ifs.eof() && !bEndOfTrack) {
          // Fundsmentally all MIDI Events contain a timecode, and a status byte*
          uint32_t nStatusTimeDelta = 0;
          uint8_t nStatus = 0;

          // Read Timecode
          nStatusTimeDelta = ReadValue();
          nStatus = ifs.get();

          if (nStatus < 0x80)  {
            nStatus = nPreviousStatus;
            ifs.seekg(-1, std::ios::cur); // Go back one byte
          }

          if ((nStatus & 0xF0) == EventName::VoiceNoteOff) {
            nPreviousStatus = nStatus;
            uint8_t nChannel = nStatus & 0x0F; // Extract channel number
            uint8_t nNoteID = ifs.get(); // Read note number
            uint8_t nVelocity = ifs.get(); // Read velocity
            vecTracks[nChunk].vecEvents.push_back({ MidiEvent::Type::NoteOff, nNoteID, nVelocity, nStatusTimeDelta });
          }

          else if ((nStatus & 0xF0) == EventName::VoiceNoteOn) {
            nPreviousStatus = nStatus;
            uint8_t nChannel = nStatus & 0x0F; // Extract channel number
            uint8_t nNoteID = ifs.get(); // Read note number
            uint8_t nVelocity = ifs.get(); // Read velocity

            if(nVelocity == 0) {
              // Note Off event
              vecTracks[nChunk].vecEvents.push_back({ MidiEvent::Type::NoteOff, nNoteID, nVelocity, nStatusTimeDelta });
            } else {
              // Note On event
              vecTracks[nChunk].vecEvents.push_back({ MidiEvent::Type::NoteOn, nNoteID, nVelocity, nStatusTimeDelta });
            }


          }

          else if ((nStatus & 0xF0) == EventName::VoiceAfterTouch) {
            nPreviousStatus = nStatus;
            uint8_t nChannel = nStatus & 0x0F; // Extract channel number
            uint8_t nNoteID = ifs.get(); // Read note number
            uint8_t nPressure = ifs.get(); // Read pressure value
          }

          else if ((nStatus & 0xF0) == EventName::VoiceControlChange) {
            nPreviousStatus = nStatus;
            uint8_t nChannel = nStatus & 0x0F; // Extract channel number
            uint8_t nControllerNumber = ifs.get(); // Read controller number
            uint8_t nControllerValue = ifs.get(); // Read controller value
          }

          else if ((nStatus & 0xF0) == EventName::VoiceProgramChange) {
            nPreviousStatus = nStatus;
            uint8_t nChannel = nStatus & 0x0F; // Extract channel number
            uint8_t nProgramNumber = ifs.get(); // Read program number
          }

          else if ((nStatus & 0xF0) == EventName::VoiceChannelPressure) {
            nPreviousStatus = nStatus;
            uint8_t nChannel = nStatus & 0x0F; // Extract channel number
            uint8_t nPressureValue = ifs.get(); // Read pressure value
          }

          else if ((nStatus & 0xF0) == EventName::VoicePitchBend) {
            nPreviousStatus = nStatus;
            uint8_t nChannel = nStatus & 0x0F; // Extract channel number
            uint8_t nLS7B = ifs.get(); // Read LSB of pitch bend value
            uint8_t nMS7B = ifs.get(); // Read MSB of pitch bend value
          }

          else if ((nStatus & 0xF0) == EventName::VoiceProgramChange) {

          }

          else if ((nStatus & 0xF0) == EventName::VoiceChannelPressure) {

          }

          else if ((nStatus & 0xF0) == EventName::VoicePitchBend) {

          }

          else if (nStatus == EventName::SystemExclusive) {
            if (nStatus == 0xF0) {
              // System exclusive message begin
              std::cout<< "System Exclusive Message Begin: " << ReadString(ReadValue()) << std::endl;
            }

            if (nStatus == 0xF7) {
              // System exclusive message end
              std::cout<< "System Exclusive Message End" <<  ReadString(ReadValue()) << std::endl;              
            }

            if (nStatus == 0xFF) {
              // Meta Message
              uint8_t nType = ifs.get(); // Read meta type
              uint32_t nLength = ReadValue(); // Read meta data length

              switch(nType) {
                case MetaEventName::MetaSequence:
                  std::cout << "Sequence Number: " << ifs.get() << ifs.get() << std::endl;
                  break;
                case MetaEventName::MetaText:
                  std::cout << "Meta Text: " << ReadString(nLength) << std::endl;
                  break;
                case MetaEventName::MetaCopyright:
                  std::cout << "Meta Copyright: " << ReadString(nLength) << std::endl;
                  break;
                case MetaEventName::MetaTrackName:
                  vecTracks[nChunk].sName = ReadString(nLength);
                  std::cout << "Track Name: " << vecTracks[nChunk].sName << std::endl;
                  break;
                case MetaEventName::MetaInstrumentName:
                  vecTracks[nChunk].sInstrument = ReadString(nLength);
                  std::cout << "Instrument Name: " << vecTracks[nChunk].sInstrument << std::endl;
                  break;
                case MetaEventName::MetaLyric:
                  std::cout << "Lyric: " << ReadString(nLength) << std::endl;
                  break;
                case MetaEventName::MetaEndOfTrack:
                  bEndOfTrack = true; // End of track event
                  break;
                default:
                  std::cout << "Unknown Meta Event: " << static_cast<int>(nType) 
                            << ", Data: " << ReadString(nLength) << std::endl;
              }
            }
          }

          else {
            std::cerr << "Unknown MIDI event: " << static_cast<int>(nStatus) << std::endl;
            return false; // Unknown MIDI event
          }
          
        }
      }

      // Convert Time Events to Notes
      for(auto& track : vecTracks) {
        uint32_t nWallTime = 0;
        std::list<MidiNote> listNotesBeingProcessed;

        for(auto& event : track.vecEvents) {
          nWallTime += event.nDeltaTick;
          if (event.event == MidiEvent::Type::NoteOn) {
            // New Note
            listNotesBeingProcessed.push_back(MidiNote{event.nKey, event.nVelocity, nWallTime, 0});
          }
          if (event.event == MidiEvent::Type::NoteOff) {
            auto note = std::find_if(listNotesBeingProcessed.begin(), listNotesBeingProcessed.end(), [&](const MidiNote& n) {
              return n.nKey == event.nKey;
            });

            if (note != listNotesBeingProcessed.end()) {
              note -> nDuration = nWallTime - note->nStartTime;
              track.vecNotes.push_back(*note);
              track.nMaxNote = std::max(track.nMaxNote, note->nKey);
              track.nMinNote = std::min(track.nMinNote, note->nKey);
              listNotesBeingProcessed.erase(note);
            }
          }
        }
      }

      return true;
    }
};

int main() {
  return 0;
}