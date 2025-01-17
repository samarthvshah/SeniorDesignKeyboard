#include <Arduino.h>
#include <HardwareSerial.h>

#define BUTTON1_PIN 43
#define BUTTON2_PIN 44
#define BUTTON3_PIN 21
#define BUTTON4_PIN 16

#define BUTTON1_NOTE 60
#define BUTTON2_NOTE 61
#define BUTTON3_NOTE 62
#define BUTTON4_NOTE 63

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


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  SerialPort2.begin(31250, SERIAL_8N1, RX_PIN, TX_PIN);

  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  pinMode(BUTTON3_PIN, INPUT_PULLUP);
  pinMode(BUTTON4_PIN, INPUT_PULLUP);
  
  delay(100);

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


  int button1State = digitalRead(BUTTON1_PIN);
  int button2State = digitalRead(BUTTON2_PIN);
  int button3State = digitalRead(BUTTON3_PIN);
  int button4State = digitalRead(BUTTON4_PIN);

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

  // Serial.print("Button 1 State: ");
  // Serial.print(button1State);
  // Serial.print(", Button 2 State: ");
  // Serial.print(button2State);
  // Serial.print(", Button 3 State: ");
  // Serial.print(button3State);
  // Serial.print(", Button 4 State: ");
  // Serial.println(button4State);

  delay(10);
}