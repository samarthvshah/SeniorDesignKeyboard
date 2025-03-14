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

#define BUTTON1_NOTE 60
#define BUTTON2_NOTE 61
#define BUTTON3_NOTE 62
#define BUTTON4_NOTE 63
#define BUTTON5_NOTE 64
#define BUTTON6_NOTE 65
#define BUTTON7_NOTE 66
#define BUTTON8_NOTE 67
#define BUTTON9_NOTE 68
#define BUTTON10_NOTE 69
#define BUTTON11_NOTE 70
#define BUTTON12_NOTE 71
#define BUTTON13_NOTE 72
#define BUTTON14_NOTE 73
#define BUTTON15_NOTE 74
#define BUTTON16_NOTE 75
#define BUTTON17_NOTE 76
#define BUTTON18_NOTE 77
#define BUTTON19_NOTE 78
#define BUTTON20_NOTE 79


#define VELOCITY 100

#define DEFAULT_BUTTON_STATE 1

#define RX_PIN 43
#define TX_PIN 44

// #define PIN_POWER_ON 15  // LCD and battery Power Enable
// #define PIN_LCD_BL 38    // BackLight enable pin (see Dimming.txt)

#define SDA_PIN 1
#define SCL_PIN 2

#define SD_CS_PIN 10

#define NOTE_ON  144
#define NOTE_OFF 128


#define MAX_SONGS 5

// HardwareSerial SerialPort2(2);

// SX1509 I2C address (set by ADDR1 and ADDR0 (00 by default):
const byte SX1509_ADDRESS = 0x3E; // SX1509 I2C address
SX1509 io;                        // Create an SX1509 object to be used throughout

// Objects for the screen, UART to the GPS board, and parsing GPS data
TFT_eSPI tft = TFT_eSPI();


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

bool isRecording = false;
bool isPlaying = false;
int recorded = 0;
unsigned long recordingStartTime = 0;
unsigned long recordingEndTime = 0;
int eventCount = 0;
int songIndex = 0;
int playBackIndex = 0;
int nextRecordingSlot = 1;


struct Song {
  byte note;
  byte velocity;
  bool isNoteOn;
  unsigned long timeStamp;
};

Song recordedSongs[MAX_SONGS];

void recordSong(int command, int note, int velocity) {
  if (!isRecording || eventCount >= MAX_SONGS) return;
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
  Serial.println(command);
  Serial.println(note);
  Serial.println(velocity);
  Serial.println("\n");
  if (isRecording) {
    recordSong(command, note, velocity);
  }
}




void startRecording() {
  recordingStartTime = millis();
  isRecording = true;
  Serial.println("Recording Started");
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0, 4);
  tft.setTextColor(TFT_WHITE);
  tft.println("Recording Started");
}

void stopRecording() {
  if (!isRecording) return;
  recordingEndTime = millis();
isRecording = false;
Serial.println("Recording Stopped");
tft.fillScreen(TFT_BLACK);
tft.setCursor(0, 0, 4);
tft.setTextColor(TFT_WHITE);
tft.println("Recording Stopped");


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
  Serial.println("Playback Started");
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0, 4);
  tft.setTextColor(TFT_WHITE);
  tft.println("Playback Started");
}

void stopPlayback() {
  isPlaying = false;
  Serial.println("Playback Stopped");
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0, 4);
  tft.setTextColor(TFT_WHITE);
  tft.println("Playback Stopped");
  for (byte note = 0; note < 128; note++) {
    Serial1.write(NOTE_OFF);
    Serial1.write(note);
    Serial1.write(0);
  }
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
    playBackIndex++;
    if (playBackIndex >= eventCount) {
      isPlaying = false;
      Serial.println("Playback Complete");
      // tft.fillScreen(TFT_BLACK);
      // tft.setCursor(0, 0, 4);
      // tft.setTextColor(TFT_WHITE);
      // tft.println("Playback Complete");
      break;
    }
  }
}

void writeToFile(fs::FS &fs, const char *filename, Song events[], int count) {
  File file = fs.open(filename, FILE_WRITE);
  if (!file) {
    Serial.println("Error opening file for writing");
    return;
  }
  // Write each event as a CSV line.
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
  Serial.println("Song events loaded from SD card");
  return count;
}


void listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if (!root) {
        Serial.println("Failed to open directory");
        return;
    }
    if (!root.isDirectory()) {
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            Serial.print("DIR : ");
            Serial.println(file.name());
            if (levels) {
                listDir(fs, file.name(), levels - 1);
            }
        } else {
            Serial.print("FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}



void saveCurrentRecording() {
  char filename[20];
  sprintf(filename, "/recording%d.txt", nextRecordingSlot);
  writeToFile(SD, filename, recordedSongs, eventCount);
  Serial.print("Saved recording to ");
  Serial.println(filename);
  nextRecordingSlot++;
  if (nextRecordingSlot > MAX_SONGS) {
    nextRecordingSlot = 1;
    Serial.print("Recording slots maxed out. First recording being reset");
  }
}



// Helper function to check if any button is pressed to control playback
void checkButtonsForPlaybackControl() {
  // Check specifically for button 14 (stop)
  io.digitalWrite(ROW2_PIN, HIGH);
  io.digitalWrite(ROW1_PIN, LOW);
  int button14State = io.digitalRead(COLUMN4_PIN);
  
  // If button 20 is pressed, stop playback
  if (button14State != DEFAULT_BUTTON_STATE) {
    isPlaying = false;
    
    // Return to normal mode
    // tft.fillScreen(TFT_BLACK);
    // tft.setCursor(0, 0, 4);
    // tft.setTextColor(TFT_WHITE);
    // tft.println("Normal Mode");
    // tft.println("Buttons Pressed: ");
  }
}


void playRecording(int slot) {
  char filename[20];
  sprintf(filename, "/recording%d.txt", slot);
  eventCount = readFromFile(SD, filename, recordedSongs, MAX_SONGS);
  
  if (eventCount > 0) {
    // Initialize playback variables
    isPlaying = true;
    playBackIndex = 0;
    recordingStartTime = millis(); // Reset playback timer
    
    // Display playback status
    Serial.print("Playback started from ");
    Serial.println(filename);
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 0, 4);
    tft.setTextColor(TFT_GREEN);
    tft.println("Playback Started");
    tft.println(filename);
    
    // Start playback timing loop
    while (isPlaying) {
      // Process timing for playback events
      unsigned long currentTime = millis() - recordingStartTime;
      
      // Send MIDI events based on timing
      while (playBackIndex < eventCount && recordedSongs[playBackIndex].timeStamp <= currentTime) {
        byte command = recordedSongs[playBackIndex].isNoteOn ? NOTE_ON : NOTE_OFF;
        byte note = recordedSongs[playBackIndex].note;
        byte velocity = recordedSongs[playBackIndex].velocity;
        
        // Send MIDI message
        Serial1.write(command);
        Serial1.write(note);
        Serial1.write(velocity);
        
        // Display currently playing note on screen
        tft.setCursor(0, 60, 2);
        tft.fillRect(0, 60, 320, 30, TFT_BLACK);
        tft.print("Playing: Note ");
        tft.print(note);
        tft.print(command == NOTE_ON ? " On" : " Off");
        
        playBackIndex++;
        
        // Check if playback is complete
        if (playBackIndex >= eventCount) {
          isPlaying = false;
          Serial.println("Playback Complete");
          tft.fillScreen(TFT_BLACK);
          tft.setCursor(0, 0, 4);
          tft.setTextColor(TFT_WHITE);
          tft.println("Playback Complete");
          break;
        }
      }
      
      // Check for button presses that might stop playback
      checkButtonsForPlaybackControl();
      
      // Small delay to prevent CPU hogging
      delay(1);
    }
  } else {
    // No recording found
    Serial.print("No recording found in ");
    Serial.println(filename);
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 0, 4);
    tft.setTextColor(TFT_RED);
    tft.println("No recording found");
    delay(1000); // Show error message briefly
    
    // Return to normal mode
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 0, 4);
    tft.setTextColor(TFT_WHITE);
    tft.println("Normal Mode");
    tft.println("Buttons Pressed: ");
  }
}


void setup() {
  // put your setup code here, to run once:
  Wire.begin(SDA_PIN, SCL_PIN);

  Serial.begin(115200);
  Serial1.begin(31250, SERIAL_8N1, RX_PIN, TX_PIN);

  pinMode(PIN_POWER_ON, OUTPUT);
  digitalWrite(PIN_POWER_ON, HIGH);

  pinMode(PIN_LCD_BL, OUTPUT);
  digitalWrite(PIN_LCD_BL, HIGH);

  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0,0,4);
  tft.setTextColor(TFT_WHITE);
  tft.println("Normal Mode"); 
  tft.println("Buttons Pressed: ");
  

  delay(800);

  Serial.println("Starting SX1509");

  if (io.begin(SX1509_ADDRESS) == false)
  {
    Serial.println("Failed to communicate. Check wiring and address of SX1509.");
    while (1)
      ; // If we fail to communicate, loop forever.
  }
  Serial.println("GPIO expander initialized successfully.");

  Serial.println("Initializing SD card...");

  if (!SD.begin(SD_CS_PIN)) {
      Serial.println("SD card initialization failed!");
      return;
  }
  Serial.println("SD card initialized successfully.");

  // delay(500);

  // // List all files in root directory
  // writeToFile(SD, "/data.txt", "Hello");

  // listDir(SD, "/", 0);

  // readFromFile(SD, "/data.txt");

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


  Serial.println("Keyboard Ready, waiting for inputs");
}

void loop() {
  // Check if playback is in progress and handle timing
  playbackTiming();
  
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

  // Regular MIDI buttons (11-12)
  if (button11State != lastButton11State) {
    if (button11State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON11_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON11_NOTE, VELOCITY);
    }
  }

  if (button12State != lastButton12State) {
    if (button12State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON12_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON12_NOTE, VELOCITY);
    }
  }

  // --- Start Recording (Button 13) ---
  if (button13State != lastButton13State) {
    if (button13State != DEFAULT_BUTTON_STATE) {
      startRecording();
      MIDIMessage(NOTE_ON, BUTTON13_NOTE, VELOCITY);
      
      // Update display for recording mode
      tft.fillScreen(TFT_BLACK);
      tft.setCursor(0, 0, 4);
      tft.setTextColor(TFT_RED);  // Red color to indicate recording
      tft.println("Recording Mode");
      tft.setTextColor(TFT_WHITE);
      tft.println("Buttons Pressed: ");
    } else {
      MIDIMessage(NOTE_OFF, BUTTON13_NOTE, VELOCITY);
    }
  }

  // --- Stop Recording (Button 14) ---
  if (button14State != lastButton14State) {
    if (button14State != DEFAULT_BUTTON_STATE) {
      stopRecording();
      MIDIMessage(NOTE_ON, BUTTON14_NOTE, VELOCITY);
      // Save the current recording to the SD card
      saveCurrentRecording();
      
      // Return to normal mode display
      tft.fillScreen(TFT_BLACK);
      tft.setCursor(0, 0, 4);
      tft.setTextColor(TFT_WHITE);
      tft.println("Normal Mode");
      tft.println("Buttons Pressed: ");
    } else {
      MIDIMessage(NOTE_OFF, BUTTON14_NOTE, VELOCITY);
    }
  }

  // --- Playback Recording 1 (Button 15) ---
  if (button15State != lastButton15State) {
    if (button15State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON15_NOTE, VELOCITY);
      playRecording(1);
      // Set up variables for playback timing
      isPlaying = true;
      playBackIndex = 0;
      recordingStartTime = millis();
      
      // Update display for playback
      tft.fillScreen(TFT_BLACK);
      tft.setCursor(0, 0, 4);
      tft.setTextColor(TFT_GREEN);  // Green color to indicate playback
      tft.println("Playing Recording 1");
    } else {
      MIDIMessage(NOTE_OFF, BUTTON15_NOTE, VELOCITY);
    }
  }

  // --- Playback Recording 2 (Button 16) ---
  if (button16State != lastButton16State) {
    if (button16State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON16_NOTE, VELOCITY);
      playRecording(2);
      // Set up variables for playback timing
      isPlaying = true;
      playBackIndex = 0;
      recordingStartTime = millis();
      
      // Update display for playback
      tft.fillScreen(TFT_BLACK);
      tft.setCursor(0, 0, 4);
      tft.setTextColor(TFT_GREEN);
      tft.println("Playing Recording 2");
    } else {
      MIDIMessage(NOTE_OFF, BUTTON16_NOTE, VELOCITY);
    }
  }

  // --- Playback Recording 3 (Button 17) ---
  if (button17State != lastButton17State) {
    if (button17State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON17_NOTE, VELOCITY);
      playRecording(3);
      // Set up variables for playback timing
      isPlaying = true;
      playBackIndex = 0;
      recordingStartTime = millis();
      
      // Update display for playback
      tft.fillScreen(TFT_BLACK);
      tft.setCursor(0, 0, 4);
      tft.setTextColor(TFT_GREEN);
      tft.println("Playing Recording 3");
    } else {
      MIDIMessage(NOTE_OFF, BUTTON17_NOTE, VELOCITY);
    }
  }

  // --- Playback Recording 4 (Button 18) ---
  if (button18State != lastButton18State) {
    if (button18State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON18_NOTE, VELOCITY);
      playRecording(4);
      // Set up variables for playback timing
      isPlaying = true;
      playBackIndex = 0;
      recordingStartTime = millis();
      
      // Update display for playback
      tft.fillScreen(TFT_BLACK);
      tft.setCursor(0, 0, 4);
      tft.setTextColor(TFT_GREEN);
      tft.println("Playing Recording 4");
    } else {
      MIDIMessage(NOTE_OFF, BUTTON18_NOTE, VELOCITY);
    }
  }

  // --- Playback Recording 5 (Button 19) ---
  if (button19State != lastButton19State) {
    if (button19State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON19_NOTE, VELOCITY);
      playRecording(5);
      // Set up variables for playback timing
      isPlaying = true;
      playBackIndex = 0;
      recordingStartTime = millis();
      
      // Update display for playback
      tft.fillScreen(TFT_BLACK);
      tft.setCursor(0, 0, 4);
      tft.setTextColor(TFT_GREEN);
      tft.println("Playing Recording 5");
    } else {
      MIDIMessage(NOTE_OFF, BUTTON19_NOTE, VELOCITY);
    }
  }

  // --- Return to Normal Mode (Button 20) ---
  if (button20State != lastButton20State) {
    if (button20State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON20_NOTE, VELOCITY);
      
      // Stop any ongoing recording or playback
      stopRecording();
      isPlaying = false;  // Stop playback
      
      // Reset to normal mode
      tft.fillScreen(TFT_BLACK);
      tft.setCursor(0, 0, 4);
      tft.setTextColor(TFT_WHITE);
      tft.println("Normal Mode");
      tft.println("Buttons Pressed: ");
    } else {
      MIDIMessage(NOTE_OFF, BUTTON20_NOTE, VELOCITY);
    }
  }

  // Update last states
  lastButton11State = button11State;
  lastButton12State = button12State;
  lastButton13State = button13State;
  lastButton14State = button14State;
  lastButton15State = button15State;
  lastButton16State = button16State;
  lastButton17State = button17State;
  lastButton18State = button18State;
  lastButton19State = button19State;
  lastButton20State = button20State;

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

  // Process MIDI input for buttons 1-10
  if (button1State != lastButton1State) {
    if (button1State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON1_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON1_NOTE, VELOCITY);
    }
  }

  if (button2State != lastButton2State) {
    if (button2State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON2_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON2_NOTE, VELOCITY);
    }
  }

  if (button3State != lastButton3State) {
    if (button3State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON3_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON3_NOTE, VELOCITY);
    }
  }

  if (button4State != lastButton4State) {
    if (button4State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON4_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON4_NOTE, VELOCITY);
    }
  }

  if (button5State != lastButton5State) {
    if (button5State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON5_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON5_NOTE, VELOCITY);
    }
  }

  if (button6State != lastButton6State) {
    if (button6State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON6_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON6_NOTE, VELOCITY);
    }
  }

  if (button7State != lastButton7State) {
    if (button7State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON7_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON7_NOTE, VELOCITY);
    }
  }

  if (button8State != lastButton8State) {
    if (button8State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON8_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON8_NOTE, VELOCITY);
    }
  }

  if (button9State != lastButton9State) {
    if (button9State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON9_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON9_NOTE, VELOCITY);
    }
  }

  if (button10State != lastButton10State) {
    if (button10State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON10_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON10_NOTE, VELOCITY);
    }
  }

  // Update last states
  lastButton1State = button1State;
  lastButton2State = button2State;
  lastButton3State = button3State;
  lastButton4State = button4State;
  lastButton5State = button5State;
  lastButton6State = button6State;
  lastButton7State = button7State;
  lastButton8State = button8State;
  lastButton9State = button9State;
  lastButton10State = button10State;

  // Only update the display if not in playback mode
  if (!isPlaying) {
    // Display currently pressed buttons
    tft.setCursor(0, 60, 4);  // Move cursor to position for button display
    tft.fillRect(0, 60, 320, 30, TFT_BLACK);  // Clear just the button display area
    
    if (lastButton1State != DEFAULT_BUTTON_STATE) {
      tft.print("1 ");
    }
    if (lastButton2State != DEFAULT_BUTTON_STATE) {
      tft.print("2 ");
    }
    if (lastButton3State != DEFAULT_BUTTON_STATE) {
      tft.print("3 ");
    }
    if (lastButton4State != DEFAULT_BUTTON_STATE) {
      tft.print("4 ");
    }
    if (lastButton5State != DEFAULT_BUTTON_STATE) {
      tft.print("5 ");
    }
    if (lastButton6State != DEFAULT_BUTTON_STATE) {
      tft.print("6 ");
    }
    if (lastButton7State != DEFAULT_BUTTON_STATE) {
      tft.print("7 ");
    }
    if (lastButton8State != DEFAULT_BUTTON_STATE) {
      tft.print("8 ");
    }
    if (lastButton9State != DEFAULT_BUTTON_STATE) {
      tft.print("9 ");
    }
    if (lastButton10State != DEFAULT_BUTTON_STATE) {
      tft.print("10 ");
    }
    if (lastButton11State != DEFAULT_BUTTON_STATE) {
      tft.print("11 ");
    }
    if (lastButton12State != DEFAULT_BUTTON_STATE) {
      tft.print("12 ");
    }
    if (lastButton13State != DEFAULT_BUTTON_STATE) {
      tft.print("13 ");
    }
    if (lastButton14State != DEFAULT_BUTTON_STATE) {
      tft.print("14 ");
    }
    if (lastButton15State != DEFAULT_BUTTON_STATE) {
      tft.print("15 ");
    }
    if (lastButton16State != DEFAULT_BUTTON_STATE) {
      tft.print("16 ");
    }
    if (lastButton17State != DEFAULT_BUTTON_STATE) {
      tft.print("17 ");
    }
    if (lastButton18State != DEFAULT_BUTTON_STATE) {
      tft.print("18 ");
    }
    if (lastButton19State != DEFAULT_BUTTON_STATE) {
      tft.print("19 ");
    }
  }
}