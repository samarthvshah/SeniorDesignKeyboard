#include <Arduino.h>
#include <Wire.h> 
#include <HardwareSerial.h>
#include <SparkFunSX1509.h>

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

#define VELOCITY 100

#define DEFAULT_BUTTON_STATE 1

#define RX_PIN 3
#define TX_PIN 10

#define SDA_PIN 1
#define SCL_PIN 2

#define NOTE_ON  144
#define NOTE_OFF 128

// HardwareSerial SerialPort2(2);

// SX1509 I2C address (set by ADDR1 and ADDR0 (00 by default):
const byte SX1509_ADDRESS = 0x3E; // SX1509 I2C address
SX1509 io;                        // Create an SX1509 object to be used throughout

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


void setup() {
  // put your setup code here, to run once:
  Wire.begin(SDA_PIN, SCL_PIN);

  Serial.begin(115200);
  Serial1.begin(31250, SERIAL_8N1, RX_PIN, TX_PIN);

  delay(800);

  Serial.println("Starting SX1509");

  if (io.begin(SX1509_ADDRESS) == false)
  {
    Serial.println("Failed to communicate. Check wiring and address of SX1509.");
    while (1)
      ; // If we fail to communicate, loop forever.
  }

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

  io.digitalWrite(ROW1_PIN, HIGH);
  io.digitalWrite(ROW2_PIN, HIGH);

  Serial.println("Keyboard Ready, waiting for inputs");
}

void MIDIMessage(int command, int note, int velocity) {
  Serial1.write(command);
  Serial1.write(note);
  Serial1.write(velocity);
  // Serial.println(command);
  // Serial.println(note);
  // Serial.println(velocity);
  // Serial.println("\n");
}

void loop() {

  
  if (Serial1.available()) {
    int incomingByte = Serial1.read();

    Serial.println(incomingByte);

  }

  io.digitalWrite(ROW2_PIN, HIGH);
  io.digitalWrite(ROW1_PIN, LOW);

  int button1State =  io.digitalRead(COLUMN1_PIN);
  int button2State =  io.digitalRead(COLUMN2_PIN);
  int button3State =  io.digitalRead(COLUMN3_PIN);
  int button4State =  io.digitalRead(COLUMN4_PIN);
  int button5State =  io.digitalRead(COLUMN5_PIN);
  int button6State =  io.digitalRead(COLUMN6_PIN);
  int button7State =  io.digitalRead(COLUMN7_PIN);
  int button8State =  io.digitalRead(COLUMN8_PIN);
  int button9State =  io.digitalRead(COLUMN9_PIN);
  int button10State = io.digitalRead(COLUMN10_PIN);

  if (button1State != lastButton1State) {
    if (button1State != DEFAULT_BUTTON_STATE) {
      // MIDIMessage(NOTE_ON, BUTTON1_NOTE, VELOCITY);
    } else {
      // MIDIMessage(NOTE_OFF, BUTTON1_NOTE, VELOCITY);
    }
  }

  if (button2State != lastButton2State) {
    if (button2State != DEFAULT_BUTTON_STATE) {
      // MIDIMessage(NOTE_ON, BUTTON2_NOTE, VELOCITY);
    } else {
      // MIDIMessage(NOTE_OFF, BUTTON2_NOTE, VELOCITY);
    }
  }

  if (button3State != lastButton3State) {
    if (button3State != DEFAULT_BUTTON_STATE) {
      // MIDIMessage(NOTE_ON, BUTTON3_NOTE, VELOCITY);
    } else {
      // MIDIMessage(NOTE_OFF, BUTTON3_NOTE, VELOCITY);
    }
  }

  if (button4State != lastButton4State) {
    if (button4State != DEFAULT_BUTTON_STATE) {
      // MIDIMessage(NOTE_ON, BUTTON4_NOTE, VELOCITY);
    } else {
      // MIDIMessage(NOTE_OFF, BUTTON4_NOTE, VELOCITY);
    }
  }

    if (button5State != lastButton5State) {
    if (button5State != DEFAULT_BUTTON_STATE) {
      // MIDIMessage(NOTE_ON, BUTTON5_NOTE, VELOCITY);
    } else {
      // MIDIMessage(NOTE_OFF, BUTTON5_NOTE, VELOCITY);
    }
  }

  if (button6State != lastButton6State) {
    if (button6State != DEFAULT_BUTTON_STATE) {
      // MIDIMessage(NOTE_ON, BUTTON6_NOTE, VELOCITY);
    } else {
      // MIDIMessage(NOTE_OFF, BUTTON6_NOTE, VELOCITY);
    }
  }

  if (button7State != lastButton7State) {
    if (button7State != DEFAULT_BUTTON_STATE) {
      // MIDIMessage(NOTE_ON, BUTTON7_NOTE, VELOCITY);
    } else {
      // MIDIMessage(NOTE_OFF, BUTTON7_NOTE, VELOCITY);
    }
  }

  if (button8State != lastButton8State) {
    if (button8State != DEFAULT_BUTTON_STATE) {
      // MIDIMessage(NOTE_ON, BUTTON8_NOTE, VELOCITY);
    } else {
      // MIDIMessage(NOTE_OFF, BUTTON8_NOTE, VELOCITY);
    }
  }

  if (button9State != lastButton9State) {
    if (button9State != DEFAULT_BUTTON_STATE) {
      // MIDIMessage(NOTE_ON, BUTTON9_NOTE, VELOCITY);
    } else {
      // MIDIMessage(NOTE_OFF, BUTTON9_NOTE, VELOCITY);
    }
  }

  if (button10State != lastButton10State) {
    if (button10State != DEFAULT_BUTTON_STATE) {
      // MIDIMessage(NOTE_ON, BUTTON10_NOTE, VELOCITY);
    } else {
      // MIDIMessage(NOTE_OFF, BUTTON10_NOTE, VELOCITY);
    }
  }

  lastButton1State =  button1State;
  lastButton2State =  button2State;
  lastButton3State =  button3State;
  lastButton4State =  button4State;
  lastButton5State =  button5State;
  lastButton6State =  button6State;
  lastButton7State =  button7State;
  lastButton8State =  button8State;
  lastButton9State =  button9State;
  lastButton10State = button10State;

  digitalWrite(ROW1_PIN, HIGH);
  digitalWrite(ROW2_PIN, LOW);

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

  if (button11State != lastButton11State) {
    if (button11State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON1_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON1_NOTE, VELOCITY);
    }
  }

  if (button12State != lastButton12State) {
    if (button12State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON2_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON2_NOTE, VELOCITY);
    }
  }

  if (button13State != lastButton13State) {
    if (button13State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON3_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON3_NOTE, VELOCITY);
    }
  }

  if (button14State != lastButton14State) {
    if (button14State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON4_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON4_NOTE, VELOCITY);
    }
  }

    if (button15State != lastButton15State) {
    if (button15State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON5_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON5_NOTE, VELOCITY);
    }
  }

  if (button16State != lastButton16State) {
    if (button16State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON6_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON6_NOTE, VELOCITY);
    }
  }

  if (button17State != lastButton17State) {
    if (button17State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON7_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON7_NOTE, VELOCITY);
    }
  }

  if (button18State != lastButton18State) {
    if (button18State != DEFAULT_BUTTON_STATE) {
      MIDIMessage(NOTE_ON, BUTTON8_NOTE, VELOCITY);
    } else {
      MIDIMessage(NOTE_OFF, BUTTON8_NOTE, VELOCITY);
    }
  }

  if (button19State != lastButton19State) {
    if (button19State != DEFAULT_BUTTON_STATE) {
      // MIDIMessage(NOTE_ON, BUTTON9_NOTE, VELOCITY);
    } else {
      // MIDIMessage(NOTE_OFF, BUTTON9_NOTE, VELOCITY);
    }
  }

  if (button20State != lastButton20State) {
    if (button20State != DEFAULT_BUTTON_STATE) {
      // MIDIMessage(NOTE_ON, BUTTON10_NOTE, VELOCITY);
    } else {
      // MIDIMessage(NOTE_OFF, BUTTON10_NOTE, VELOCITY);
    }
  }

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


  delay(10);
}