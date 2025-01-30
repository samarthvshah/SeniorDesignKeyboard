#include <Arduino.h>
#include <HardwareSerial.h>

#define COLUMN1_PIN 43
#define COLUMN2_PIN 44
#define COLUMN3_PIN 21
#define COLUMN4_PIN 16

#define ROW1_PIN 1
#define ROW2_PIN 2

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

#define RX_PIN 18
#define TX_PIN 17

#define NOTE_ON  144
#define NOTE_OFF 128

HardwareSerial SerialPort2(2);

int lastButton1State = DEFAULT_BUTTON_STATE;
int lastButton2State = DEFAULT_BUTTON_STATE;
int lastButton3State = DEFAULT_BUTTON_STATE;
int lastButton4State = DEFAULT_BUTTON_STATE;
int lastButton5State = DEFAULT_BUTTON_STATE;
int lastButton6State = DEFAULT_BUTTON_STATE;
int lastButton7State = DEFAULT_BUTTON_STATE;
int lastButton8State = DEFAULT_BUTTON_STATE;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  SerialPort2.begin(31250, SERIAL_8N1, RX_PIN, TX_PIN);

  pinMode(COLUMN1_PIN, INPUT_PULLUP);
  pinMode(COLUMN2_PIN, INPUT_PULLUP);
  pinMode(COLUMN3_PIN, INPUT_PULLUP);
  pinMode(COLUMN4_PIN, INPUT_PULLUP);

  pinMode(ROW1_PIN, OUTPUT);
  pinMode(ROW2_PIN, OUTPUT);
  
  delay(800);

  digitalWrite(ROW1_PIN, HIGH);
  digitalWrite(ROW2_PIN, HIGH);

  Serial.println("Keyboard Ready, waiting for inputs");
}

void MIDIMessage(int command, int note, int velocity) {
  // SerialPort2.write(command);
  // SerialPort2.write(note);
  // SerialPort2.write(velocity);
  Serial.println(command);
  Serial.println(note);
  Serial.println(velocity);
  Serial.println("\n");
}

void loop() {

  digitalWrite(ROW2_PIN, HIGH);
  digitalWrite(ROW1_PIN, LOW);

  int button1State = digitalRead(COLUMN1_PIN);
  int button2State = digitalRead(COLUMN2_PIN);
  int button3State = digitalRead(COLUMN3_PIN);
  int button4State = digitalRead(COLUMN4_PIN);

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

  lastButton1State = button1State;
  lastButton2State = button2State;
  lastButton3State = button3State;
  lastButton4State = button4State;

  digitalWrite(ROW1_PIN, HIGH);
  digitalWrite(ROW2_PIN, LOW);

  int button5State = digitalRead(COLUMN1_PIN);
  int button6State = digitalRead(COLUMN2_PIN);
  int button7State = digitalRead(COLUMN3_PIN);
  int button8State = digitalRead(COLUMN4_PIN);

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

  lastButton5State = button5State;
  lastButton6State = button6State;
  lastButton7State = button7State;
  lastButton8State = button8State;


  delay(10);
}