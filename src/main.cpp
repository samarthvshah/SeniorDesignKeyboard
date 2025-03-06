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




void MIDIMessage(int command, int note, int velocity) {
  Serial1.write(command);
  Serial1.write(note);
  Serial1.write(velocity);
  Serial.println(command);
  Serial.println(note);
  Serial.println(velocity);
  Serial.println("\n");
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


void writeToFile(fs::FS &fs, const char *filename, String message) {
  fs::File dataFile = SD.open(filename, FILE_WRITE);
  dataFile.println(message);
  dataFile.close();
}


void readFromFile(fs::FS &fs, const char *filename) {
  fs::File dataFile = SD.open(filename, FILE_READ);
  if (dataFile) {
    while (dataFile.available()) {
        char c = dataFile.read(); // Get the next character
        Serial.print(c);
      }
      // close the file:
    dataFile.close();
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

  io.digitalWrite(ROW2_PIN, HIGH);
  io.digitalWrite(ROW1_PIN, LOW);

  int button11State =  io.digitalRead(COLUMN1_PIN);
  int button12State =  io.digitalRead(COLUMN2_PIN);
  int button13State =  io.digitalRead(COLUMN3_PIN);
  int button14State =  io.digitalRead(COLUMN4_PIN);
  int button15State =  io.digitalRead(COLUMN5_PIN);
  int button16State =  io.digitalRead(COLUMN6_PIN);
  int button17State =  io.digitalRead(COLUMN7_PIN);
  int button18State =  io.digitalRead(COLUMN8_PIN);
  int button19State =  io.digitalRead(COLUMN9_PIN);
  int button20State = io.digitalRead(COLUMN10_PIN);

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

  if (button13State != lastButton13State) {
    if (button13State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON13_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON13_NOTE, VELOCITY);
    }
  }

  if (button14State != lastButton14State) {
    if (button14State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON14_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON14_NOTE, VELOCITY);
    }
  }

    if (button15State != lastButton15State) {
    if (button15State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON15_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON15_NOTE, VELOCITY);
    }
  }

  if (button16State != lastButton16State) {
    if (button16State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, 75, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, 75, VELOCITY);
    }
  }

  if (button17State != lastButton17State) {
    if (button17State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, 76, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, 76, VELOCITY);
    }
  }

  if (button18State != lastButton18State) {
    if (button18State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, 77, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, 77, VELOCITY);
    }
  }

  if (button19State != lastButton19State) {
    if (button19State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, 78, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, 78, VELOCITY);
    }
  }

  if (button20State != lastButton20State) {
    if (button20State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, 79, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, 79, VELOCITY);
    }
  }

  lastButton11State =  button11State;
  lastButton12State =  button12State;
  lastButton13State =  button13State;
  lastButton14State =  button14State;
  lastButton15State =  button15State;
  lastButton16State =  button16State;
  lastButton17State =  button17State;
  lastButton18State =  button18State;
  lastButton19State =  button19State;
  lastButton20State = button20State;

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

  // String buttonsPressedString = "";

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0,0,4);
  tft.setTextColor(TFT_WHITE);
  tft.println("Normal Mode"); 
  tft.println("Buttons Pressed: "); 

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
  if (lastButton20State != DEFAULT_BUTTON_STATE) {
    tft.print("20 "); 
  }


  delay(50);
}