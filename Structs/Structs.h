#pragma once

struct MidiEvent
{
  enum class Type
  {
    NoteOff,
    NoteOn,
    Other
  } event;
  uint8_t nKey = 0;
  uint8_t nVelocity = 0;
  uint32_t nDeltaTick = 0;
  MidiEvent(Type e, uint8_t key, uint8_t velocity, uint32_t deltaTick)
      : event(e), nKey(key), nVelocity(velocity), nDeltaTick(deltaTick) {}
  MidiEvent() = default;
};

struct MidiNote
{
  uint8_t nKey = 0;
  uint8_t nVelocity = 0;
  uint32_t nStartTime = 0;
  uint32_t nDuration = 0;
  MidiNote(uint8_t key, uint8_t velocity, uint32_t startTime, uint32_t duration)
      : nKey(key), nVelocity(velocity), nStartTime(startTime), nDuration(duration) {}
  MidiNote() = default;
};

struct MidiTrack
{
  std::string sName;
  std::string sInstrument;
  std::vector<MidiEvent> vecEvents;
  std::vector<MidiNote> vecNotes;
  uint8_t nMaxNote = 64;
  uint8_t nMinNote = 64;
};

enum EventName : uint8_t
{
  VoiceNoteOff = 0x80,
  VoiceNoteOn = 0x90,
  VoiceAfterTouch = 0xA0,
  VoiceControlChange = 0xB0,
  VoiceProgramChange = 0xC0,
  VoiceChannelPressure = 0xD0,
  VoicePitchBend = 0xE0,
  SystemExclusive = 0xF0,
  SystemTimeCodeQuarterFrame = 0xF1,
  SystemSongPosition = 0xF2,
  SystemSongSelect = 0xF3,
  SystemTuneRequest = 0xF6,
  SystemEndOfExclusive = 0xF7,
  SystemTimingClock = 0xF8,
  SystemStart = 0xFA,
  SystemContinue = 0xFB,
  SystemStop = 0xFC,
  SystemActiveSensing = 0xFE,
  SystemReset = 0xFF
};

enum MetaEventName : uint8_t
{
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
