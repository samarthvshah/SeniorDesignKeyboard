// 7 buttons on the bottom will play notes, 8 buttons above it, so buttons 1-7 and 11-18
// 19 and 20 will be stop and start recording, if song 1 slot is open then will save to that slot, if song 2 is open then will save to 
// that slot, if not then will have to overwrite the first one
// 8 will start playback of pre-programmed song 

#include <Arduino.h>
#include <Wire.h> 
#include <HardwareSerial.h>
#include <SparkFunSX1509.h>
#include <TFT_eSPI.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "pin_config.h"

#define ROW1_PIN 0
#define ROW2_PIN 1

#define COLUMN1_PIN 2 
#define COLUMN2_PIN 3 
#define COLUMN3_PIN 4
#define COLUMN4_PIN 5
#define COLUMN5_PIN 6
#define COLUMN6_PIN 7
#define COLUMN7_PIN 8
#define COLUMN8_PIN 9
#define COLUMN9_PIN 10
#define COLUMN10_PIN 11

// MIDI Notes mapping

#define BUTTON1_NOTE 74  // C2
#define BUTTON2_NOTE 73  // D2
#define BUTTON3_NOTE 72  // E2
#define BUTTON4_NOTE 71  // F2
#define BUTTON5_NOTE 70  // G2
#define BUTTON6_NOTE 69  // A2
#define BUTTON7_NOTE 68  // B2
#define BUTTON8_NOTE 67  // C3

#define BUTTON11_NOTE 66 // C3
#define BUTTON12_NOTE 65 // D3
#define BUTTON13_NOTE 64 // E3
#define BUTTON14_NOTE 63 // F3
#define BUTTON15_NOTE 62 // G3
#define BUTTON16_NOTE 61 // A3
#define BUTTON17_NOTE 60 // B3




// Control buttons
#define BUTTON9_NOTE 90  // Playback pre-programmed song
#define BUTTON10_NOTE 91  // Playback recorded song 1
#define BUTTON18_NOTE 92 // Playback recorded song 2
#define BUTTON19_NOTE 93 // Start recording
#define BUTTON20_NOTE 94 // Stop recording & save

#define VELOCITY 100

#define DEFAULT_BUTTON_STATE 1

#define RX_PIN 43
#define TX_PIN 44

#define SDA_PIN 1
#define SCL_PIN 2

#define SD_CS_PIN 10

#define NOTE_ON  144
#define NOTE_OFF 128

#define MAX_SONGS 3 // Maximum number of songs to record
#define MAX_EVENTS 500  // Increased maximum events per song

// SX1509 I2C address (set by ADDR1 and ADDR0 (00 by default):
const byte SX1509_ADDRESS = 0x3E; // SX1509 I2C address
SX1509 io;                        // Create an SX1509 object to be used throughout

// Objects for the screen, UART to the GPS board, and parsing GPS data
TFT_eSPI tft = TFT_eSPI();

// Button states
int lastButton1State = DEFAULT_BUTTON_STATE;
int lastButton2State = DEFAULT_BUTTON_STATE;
int lastButton3State = DEFAULT_BUTTON_STATE;
int lastButton4State = DEFAULT_BUTTON_STATE;
int lastButton5State = DEFAULT_BUTTON_STATE;
int lastButton6State = DEFAULT_BUTTON_STATE;
int lastButton7State = DEFAULT_BUTTON_STATE;
int lastButton8State = DEFAULT_BUTTON_STATE;
int lastButton9State = DEFAULT_BUTTON_STATE;
int lastButton10State = DEFAULT_BUTTON_STATE;
int lastButton11State = DEFAULT_BUTTON_STATE;
int lastButton12State = DEFAULT_BUTTON_STATE;
int lastButton13State = DEFAULT_BUTTON_STATE;
int lastButton14State = DEFAULT_BUTTON_STATE;
int lastButton15State = DEFAULT_BUTTON_STATE;
int lastButton16State = DEFAULT_BUTTON_STATE;
int lastButton17State = DEFAULT_BUTTON_STATE;
int lastButton18State = DEFAULT_BUTTON_STATE;
int lastButton19State = DEFAULT_BUTTON_STATE;
int lastButton20State = DEFAULT_BUTTON_STATE;

// Song recording and playback variables
bool isRecording = false;
bool isPlaying = false;
unsigned long recordingStartTime = 0;
int eventCount = 0;
int playBackIndex = 0;
int nextRecordingSlot = 1;  // Auto-increment through slots 1-2

const int MIA_SEBASTIANS_THEME_LENGTH = 32;

// A simplified melody (example):
// These note values (in MIDI numbers) roughly translate to pitches (E4 = 64, F4 = 65, G4 = 67, A4 = 69, etc.)
byte preProgSongNotes[MIA_SEBASTIANS_THEME_LENGTH] = {
  // First phrase
  64, 64, 65, 67,  // E,  E,  F,  G
  69, 67, 65, 64,  // A,  G,  F,  E
  
  // Second phrase
  60, 60, 64, 62,  // C,  C,  E,  D
  60, 60, 64, 62,  // C,  C,  E,  D
  
  // Third phrase
  64, 64, 65, 67,  // E,  E,  F,  G
  69, 67, 65, 64,  // A,  G,  F,  E
  
  // Final phrase with a rest at the end
  60, 60, 67, 67,  // C,  C,  G,  G
  69, 69, 64, 0    // A,  A,  E,  (rest)
};

// Durations for each note (in milliseconds)
// The durations below are set to produce a rhythmic phrasing; you may tweak them for timing nuances.
unsigned long preProgSongDurations[MIA_SEBASTIANS_THEME_LENGTH] = {
  // First phrase durations
  300, 300, 300, 300,
  600, 300, 300, 600,
  
  // Second phrase durations
  300, 300, 300, 300,
  600, 300, 300, 600,
  
  // Third phrase durations
  300, 300, 300, 300,
  600, 300, 300, 600,
  
  // Final phrase durations
  300, 300, 300, 300,
  600, 300, 300, 600
};

struct Song {
  byte note;
  byte velocity;
  bool isNoteOn;
  unsigned long timeStamp;
};

// Arrays to store song events (support for 2 recorded songs)
Song recordedSongs[MAX_EVENTS];

void recordSong(int command, int note, int velocity) {
  if (!isRecording || eventCount >= MAX_EVENTS) return;
  recordedSongs[eventCount].note = note;
  recordedSongs[eventCount].velocity = velocity;
  recordedSongs[eventCount].isNoteOn = (command == NOTE_ON);
  recordedSongs[eventCount].timeStamp = millis() - recordingStartTime;
  eventCount++;
}

void MIDIMessage(int command, int note, int velocity) {
  Serial1.write(command);
  Serial1.write(note);
  Serial1.write(velocity);
  
  // Debug output to Serial monitor
  Serial.print("MIDI: ");
  Serial.print(command == NOTE_ON ? "ON" : "OFF");
  Serial.print(" Note: ");
  Serial.print(note);
  Serial.print(" Vel: ");
  Serial.println(velocity);
  
  if (isRecording) {
    recordSong(command, note, velocity);
  }
}

void startRecording() {
  recordingStartTime = millis();
  isRecording = true;
  eventCount = 0;  // Reset event counter
  Serial.println("Recording Started");
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0, 4);
  tft.setTextColor(TFT_RED);  // Red for recording mode
  tft.println("Recording Mode");
  tft.println("Press button 20 to stop");
}
void writeToFile(fs::FS &fs, const char *filename, Song events[], int count) {
  File file = fs.open(filename, FILE_WRITE);
  if (!file) {
    Serial.println("Error opening file for writing");
    return;
  }
  
  // First clear the file by truncating it
  file.close();
  file = fs.open(filename, FILE_WRITE);
  
  // Write each event as a CSV line
  for (int i = 0; i < count; i++) {
    file.print(events[i].timeStamp);
    file.print(",");
    file.print(events[i].note);
    file.print(",");
    file.print(events[i].velocity);
    file.print(",");
    file.println(events[i].isNoteOn ? 1 : 0);
  }
  file.close();
  Serial.println("Song events saved to SD card");
}

void stopRecording() {
  if (!isRecording) return;
  isRecording = false;
  Serial.println("Recording Stopped");
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0, 4);
  tft.setTextColor(TFT_WHITE);
  tft.println("Recording Stopped");
  
  // Save the recording to the appropriate slot
  char filename[20];
  sprintf(filename, "/recording%d.txt", nextRecordingSlot);
  writeToFile(SD, filename, recordedSongs, eventCount);
  
  Serial.print("Saved to slot ");
  Serial.println(nextRecordingSlot);
  
  tft.setCursor(0, 40, 2);
  tft.print("Saved to slot ");
  tft.println(nextRecordingSlot);
  
  // Rotate between slots 1 and 2
  nextRecordingSlot = (nextRecordingSlot % MAX_SONGS) + 1;
  
  delay(1000);  // Brief delay to show message
  
  // Return to normal mode
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0, 4);
  tft.setTextColor(TFT_WHITE);
  tft.println("Normal Mode");
}

void startPlayback() {
  if (eventCount == 0) {
    Serial.println("No events to play"); 
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 0, 4);
    tft.setTextColor(TFT_WHITE);
    tft.println("Nothing to play");
    return;
  }

  isPlaying = true;
  playBackIndex = 0;
  recordingStartTime = millis();  // Reset time reference
  Serial.println("Playback Started");
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0, 4);
  tft.setTextColor(TFT_GREEN);
  tft.println("Playing Recording ");

}

void stopPlayback() {
  isPlaying = false;
  Serial.println("Playback Stopped");
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0, 4);
  tft.setTextColor(TFT_WHITE);
  tft.println("Playback Stopped");
  
  // Send Note Off for all notes to prevent stuck notes
  for (byte note = 0; note < 128; note++) {
    Serial1.write(NOTE_OFF);
    Serial1.write(note);
    Serial1.write(0);
  }
  
  // Return to normal mode
  delay(1000);
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 60, 4);
  tft.setTextColor(TFT_WHITE);
  
}

void playbackTiming() {
  if (!isPlaying || eventCount == 0) return;
  unsigned long currentTime = millis() - recordingStartTime;
  
  while (playBackIndex < eventCount && recordedSongs[playBackIndex].timeStamp <= currentTime) {
    byte command = recordedSongs[playBackIndex].isNoteOn ? NOTE_ON : NOTE_OFF;
    byte note = recordedSongs[playBackIndex].note;
    byte velocity = recordedSongs[playBackIndex].velocity;
    
    Serial1.write(command);
    Serial1.write(note);
    Serial1.write(velocity);
    
    // Display currently playing note
    tft.setCursor(0, 60, 4);
    tft.fillRect(0, 60, 320, 30, TFT_BLACK);
    tft.print("Playing: Note ");
    tft.print(note);
    tft.print(command == NOTE_ON ? " On" : " Off");
    
    playBackIndex++;
    
    // Check if playback is complete
    if (playBackIndex >= eventCount) {
      isPlaying = false;
      Serial.println("Playback Complete");
      stopPlayback();
      break;
    }
  }
}

void playPreProgrammedSong() {
  // Stop any ongoing recording or playback
  isRecording = false;
  isPlaying = false;
  
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0, 4);
  tft.setTextColor(TFT_GREEN);
  tft.println("Playing Demo Song");
  
  // Play the pre-programmed song
  for (int i = 0; i < MIA_SEBASTIANS_THEME_LENGTH; i++) {
    if (preProgSongNotes[i] > 0) {  // If not a rest
      // Note On
      Serial1.write(NOTE_ON);
      Serial1.write(preProgSongNotes[i]);
      Serial1.write(VELOCITY);
      
      // Display current note
      tft.setCursor(0, 60, 4);
      tft.fillRect(0, 60, 320, 30, TFT_BLACK);
      tft.print("Playing: Note ");
      tft.print(preProgSongNotes[i]);
      
      // Wait for note duration
      delay(preProgSongDurations[i]);
      
      // Note Off
      Serial1.write(NOTE_OFF);
      Serial1.write(preProgSongNotes[i]);
      Serial1.write(0);
    } else {
      // This is a rest - just wait
      delay(preProgSongDurations[i]);
    }
    
    // Check if button 20 is pressed to stop playback
    io.digitalWrite(ROW2_PIN, HIGH);
    io.digitalWrite(ROW1_PIN, LOW);
    if (io.digitalRead(COLUMN10_PIN) != DEFAULT_BUTTON_STATE) {
      break;  // Exit the loop if stop button is pressed
    }
  }
  
  // Return to normal mode
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0, 4);
  tft.setTextColor(TFT_WHITE);
  tft.println("Normal Mode");
  
}

int readFromFile(fs::FS &fs, const char *filename, Song events[], int maxCount) {
  File file = fs.open(filename, FILE_READ);
  if (!file) {
    Serial.println("Error opening file for reading");
    return 0;
  }
  
  int count = 0;
  while (file.available() && count < maxCount) {
    String line = file.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) continue;
    
    int firstComma = line.indexOf(',');
    int secondComma = line.indexOf(',', firstComma + 1);
    int thirdComma = line.indexOf(',', secondComma + 1);
    
    if (firstComma == -1 || secondComma == -1 || thirdComma == -1) continue;
    
    String tsStr = line.substring(0, firstComma);
    String noteStr = line.substring(firstComma + 1, secondComma);
    String velStr = line.substring(secondComma + 1, thirdComma);
    String isNoteOnStr = line.substring(thirdComma + 1);
    
    events[count].timeStamp = tsStr.toInt();
    events[count].note = noteStr.toInt();
    events[count].velocity = velStr.toInt();
    events[count].isNoteOn = (isNoteOnStr.toInt() == 1);
    
    count++;
  }
  
  file.close();
  Serial.print("Loaded ");
  Serial.print(count);
  Serial.println(" song events from SD card");
  return count;
}

void playRecording(int slot) {
  char filename[20];
  sprintf(filename, "/recording%d.txt", slot);
  
  Serial.print("Attempting to play recording from slot ");
  Serial.println(slot);
  
  eventCount = readFromFile(SD, filename, recordedSongs, MAX_EVENTS);
  
  if (eventCount > 0) {
    // Set up for playback
    isPlaying = true;
    playBackIndex = 0;
    recordingStartTime = millis();
    
    // Update display
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 0, 4);
    tft.setTextColor(TFT_GREEN);
    tft.print("Playing Recording ");
    tft.println(slot);
    delay(1000);  // Brief delay to show message
    // Start playback (timing handled in loop)
    startPlayback();
    tft.print(slot);
  } else {
    // No recording found
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 0, 4);
    tft.setTextColor(TFT_RED);
    tft.print("No recording in slot ");
    tft.print(slot);
    
    delay(1500);  // Show message briefly
    
    // Return to normal mode
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 0, 4);
    tft.setTextColor(TFT_WHITE);
    tft.println("Normal Mode");
    
  }
}

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  Serial1.begin(31250, SERIAL_8N1, RX_PIN, TX_PIN);
  
  // Initialize I2C
  Wire.begin(SDA_PIN, SCL_PIN);
  
  // Power on LCD
  pinMode(PIN_POWER_ON, OUTPUT);
  digitalWrite(PIN_POWER_ON, HIGH);
  
  // Backlight on
  pinMode(PIN_LCD_BL, OUTPUT);
  digitalWrite(PIN_LCD_BL, HIGH);
  
  // Initialize display
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0, 4);
  tft.setTextColor(TFT_WHITE);
  tft.println("MIDI Keyboard");
  tft.println("Initializing...");
  
  // Initialize SX1509 GPIO expander
  delay(500);
  Serial.println("Initializing SX1509");
  if (io.begin(SX1509_ADDRESS) == false) {
    Serial.println("Failed to communicate with SX1509");
    tft.setCursor(0, 60, 2);
    tft.setTextColor(TFT_RED);
    tft.println("SX1509 Error!");
    while (1); // If communication fails, halt
  }
  Serial.println("SX1509 initialized");
  
  // Initialize SD card
  Serial.println("Initializing SD card");
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD card initialization failed");
    tft.setCursor(0, 90, 2);
    tft.setTextColor(TFT_RED);
    tft.println("SD Card Error!");
    // Continue without SD - it's not critical for basic operation
  } else {
    Serial.println("SD card initialized");
  }
  
  // Configure GPIO expander pins
  io.pinMode(COLUMN1_PIN, INPUT_PULLUP);
  io.pinMode(COLUMN2_PIN, INPUT_PULLUP);
  io.pinMode(COLUMN3_PIN, INPUT_PULLUP);
  io.pinMode(COLUMN4_PIN, INPUT_PULLUP);
  io.pinMode(COLUMN5_PIN, INPUT_PULLUP);
  io.pinMode(COLUMN6_PIN, INPUT_PULLUP);
  io.pinMode(COLUMN7_PIN, INPUT_PULLUP);
  io.pinMode(COLUMN8_PIN, INPUT_PULLUP);
  io.pinMode(COLUMN9_PIN, INPUT_PULLUP);
  io.pinMode(COLUMN10_PIN, INPUT_PULLUP);
  
  io.pinMode(ROW1_PIN, OUTPUT);
  io.pinMode(ROW2_PIN, OUTPUT);
  
  delay(800);
  
  // Final setup - display instructions
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0, 4);
  tft.setTextColor(TFT_WHITE);
  tft.println("Normal Mode");
  Serial.println("MIDI Keyboard Ready");
}

void loop() {
  // Handle playback timing if in playback mode
  playbackTiming();
  
  // === ROW 1 BUTTONS (1-10) ===
  io.digitalWrite(ROW1_PIN, HIGH);
  io.digitalWrite(ROW2_PIN, LOW);

  int button1State = io.digitalRead(COLUMN1_PIN);
  int button2State = io.digitalRead(COLUMN2_PIN);
  int button3State = io.digitalRead(COLUMN3_PIN);
  int button4State = io.digitalRead(COLUMN4_PIN);
  int button5State = io.digitalRead(COLUMN5_PIN);
  int button6State = io.digitalRead(COLUMN6_PIN);
  int button7State = io.digitalRead(COLUMN7_PIN);
  int button8State = io.digitalRead(COLUMN8_PIN);
  int button9State = io.digitalRead(COLUMN9_PIN);
  int button10State = io.digitalRead(COLUMN10_PIN);

  // Process musical note buttons (1-7)
  if (button1State != lastButton1State) {
    if (button1State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON1_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON1_NOTE, VELOCITY);
    }
    lastButton1State = button1State;
  }

  if (button2State != lastButton2State) {
    if (button2State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON2_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON2_NOTE, VELOCITY);
    }
    lastButton2State = button2State;
  }

  if (button3State != lastButton3State) {
    if (button3State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON3_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON3_NOTE, VELOCITY);
    }
    lastButton3State = button3State;
  }

  if (button4State != lastButton4State) {
    if (button4State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON4_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON4_NOTE, VELOCITY);
    }
    lastButton4State = button4State;
  }

  if (button5State != lastButton5State) {
    if (button5State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON5_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON5_NOTE, VELOCITY);
    }
    lastButton5State = button5State;
  }

  if (button6State != lastButton6State) {
    if (button6State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON6_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON6_NOTE, VELOCITY);
    }
    lastButton6State = button6State;
  }

  if (button7State != lastButton7State) {
    if (button7State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON7_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON7_NOTE, VELOCITY);
    }
    lastButton7State = button7State;
  }

  // Button 8 - Play pre-programmed song
  if (button8State != lastButton8State) {
    if (button8State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON8_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON8_NOTE, VELOCITY);
    }
    lastButton8State = button8State;
  }

  // Button 9 - Play recorded song 2
  if (button9State != lastButton9State) {
    if (button9State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON9_NOTE, VELOCITY);
      if (!isRecording && !isPlaying) {
        playRecording(2);  // Play recording from slot 1
      }
      if (isPlaying) {
        stopPlayback();
      }
    } else {
      MIDIMessage(NOTE_OFF, BUTTON9_NOTE, VELOCITY);
    }
    lastButton9State = button9State;
  }

  // Button 10 - Play recorded song 3
  if (button10State != lastButton10State) {
    if (button10State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON10_NOTE, VELOCITY);
      if (!isRecording && !isPlaying) {
        playRecording(3);  // Play recording from slot 2
      }
      if (isPlaying) {
        stopPlayback();
      }
    } else {
      MIDIMessage(NOTE_OFF, BUTTON10_NOTE, VELOCITY);
    }
    lastButton10State = button10State;
  }

  // === ROW 2 BUTTONS (11-20) ===
  io.digitalWrite(ROW2_PIN, HIGH);
  io.digitalWrite(ROW1_PIN, LOW);

  int button11State = io.digitalRead(COLUMN1_PIN);
  int button12State = io.digitalRead(COLUMN2_PIN);
  int button13State = io.digitalRead(COLUMN3_PIN);
  int button14State = io.digitalRead(COLUMN4_PIN);
  int button15State = io.digitalRead(COLUMN5_PIN);
  int button16State = io.digitalRead(COLUMN6_PIN);
  int button17State = io.digitalRead(COLUMN7_PIN);
  int button18State = io.digitalRead(COLUMN8_PIN);
  int button19State = io.digitalRead(COLUMN9_PIN);
  int button20State = io.digitalRead(COLUMN10_PIN);

  // Process upper row of musical notes (11-18)
  if (button11State != lastButton11State) {
    if (button11State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON11_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON11_NOTE, VELOCITY);
    }
    lastButton11State = button11State;
  }

  if (button12State != lastButton12State) {
    if (button12State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON12_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON12_NOTE, VELOCITY);
    }
    lastButton12State = button12State;
  }

  if (button13State != lastButton13State) {
    if (button13State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON13_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON13_NOTE, VELOCITY);
    }
    lastButton13State = button13State;
  }

  if (button14State != lastButton14State) {
    if (button14State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON14_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON14_NOTE, VELOCITY);
    }
    lastButton14State = button14State;
  }

  if (button15State != lastButton15State) {
    if (button15State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON15_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON15_NOTE, VELOCITY);
    }
    lastButton15State = button15State;
  }

  if (button16State != lastButton16State) {
    if (button16State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON16_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON16_NOTE, VELOCITY);
    }
    lastButton16State = button16State;
  }

  if (button17State != lastButton17State) {
    if (button17State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON17_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON17_NOTE, VELOCITY);
    }
    lastButton17State = button17State;
  }

  if (button18State != lastButton18State) {
    if (button18State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON18_NOTE, VELOCITY);
      if (!isRecording && !isPlaying) {
        playRecording(1);  // Play recording from slot 1
      }
      if (isPlaying) {
        stopPlayback();
      }
    } else {
      MIDIMessage(NOTE_OFF, BUTTON18_NOTE, VELOCITY);
    }
    lastButton18State = button18State;
  }

  // Button 19 - Play Pre-programmed song
  if (button19State != lastButton19State) {
    if (button19State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON19_NOTE, VELOCITY);
      playPreProgrammedSong();  // Play demo melody
    } else {
      MIDIMessage(NOTE_OFF, BUTTON19_NOTE, VELOCITY);
    }
    lastButton19State = button19State;
  }

  // Button 20 - Start/Stop Recording/Playback
  if (button20State != lastButton20State) {
    if (button20State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON20_NOTE, VELOCITY);
      if (!isPlaying) {
        startRecording();
      }
      if (isRecording) {
        stopRecording();
      }
      // if (isPlaying) {
      //   stopPlayback();
      // }
    } else {
      MIDIMessage(NOTE_OFF, BUTTON20_NOTE, VELOCITY);
    }
    lastButton20State = button20State;
  }

  // Display currently pressed buttons in normal mode
  if (!isRecording && !isPlaying) {
    // Clear display area for button numbers
    tft.setCursor(0, 60, 4);
    tft.fillRect(0, 60, 320, 30, TFT_BLACK);
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 0, 4);
    tft.setTextColor(TFT_WHITE);
    tft.println("Normal Mode");
    tft.print("Notes: ");
    
    // Check which buttons are pressed
    if (button1State != DEFAULT_BUTTON_STATE) {
      //tft.print("1 ");
      tft.print("C2 ");
    }
    if (button2State != DEFAULT_BUTTON_STATE) {
      //tft.print("2 ");
      tft.print("D2 ");
    }
    if (button3State != DEFAULT_BUTTON_STATE) {
      //tft.print("3 ");
      tft.print("E2 ");
    }
    if (button4State != DEFAULT_BUTTON_STATE) {
      //tft.print("4 ");
      tft.print("F2 ");
    }
    if (button5State != DEFAULT_BUTTON_STATE) {
      //tft.print("5 ");
      tft.print("G2 ");
    }
    if (button6State != DEFAULT_BUTTON_STATE) {
      //tft.print("6 ");
      tft.print("A2 ");
    }
    if (button7State != DEFAULT_BUTTON_STATE) {
      //tft.print("7 ");
      tft.print("B2 ");
    }
    if (button8State != DEFAULT_BUTTON_STATE) {
      //tft.print("8 ");
      tft.print("C3 ");
    }
    if (button11State != DEFAULT_BUTTON_STATE) {
      //tft.print("11 ");
      tft.print("D3 ");

    }
    if (button12State != DEFAULT_BUTTON_STATE) {
      //tft.print("12 ");
      tft.print("E3 ");
    }
    if (button13State != DEFAULT_BUTTON_STATE) {
      //tft.print("13 ");
      tft.print("F3 ");
    }
    if (button14State != DEFAULT_BUTTON_STATE) {
      //tft.print("14 ");
      tft.print("G3 ");
    }
    if (button15State != DEFAULT_BUTTON_STATE) {
      //tft.print("15 ");
      tft.print("A4 ");
    }
    if (button16State != DEFAULT_BUTTON_STATE) {
      //tft.print("16 ");
      tft.print("B3 ");
    }
    if (button17State != DEFAULT_BUTTON_STATE) {
      //tft.print("17 ");
      tft.print("C4 ");
    }
    if (button18State != DEFAULT_BUTTON_STATE) {
      //tft.print("18 ");
    }
    if (button19State != DEFAULT_BUTTON_STATE) {
      //tft.print("19 ");
    }
    if (button20State != DEFAULT_BUTTON_STATE) {
      //tft.print("20 ");
    }
  }
  // Short delay to prevent excessive CPU usage and debounce the buttons
  delay(10);  // 10ms delay - fast enough for responsiveness but allows for debouncing
}
  