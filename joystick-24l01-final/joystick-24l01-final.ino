const int xJoystick = A0;
const int yJoystick = A1;
//Asign pin numbers for the Joystick
const int sw_pin = 7; // digital pin connected to switch output

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(9, 10); // CE, CSN
const uint64_t pipes[2] = { 0xEEFDFDFDECLL, 0xEEFDFDF0DFLL };

struct package {
  int xAxis;
  int yAxis;
  int lights;
};

typedef struct package Package;
Package data;


void setup() {
  Serial.begin(57600);
  // ***** Transmitter 2.4GHz nRF24L01 settings *****
  pinMode(2, OUTPUT); // LED for the Transmitt
  radio.begin();
  radio.openWritingPipe(pipes[0]);
  // radio.setPALevel(RF24_PA_HIGH);
  radio.setPALevel(RF24_PA_MAX);
  radio.enableDynamicPayloads(); // Not working without this
  radio.stopListening();

  // For the Joystick
  // Reading from the Joystick is as follows:
  // Reading X - Axis Front to back, the positions are 1023 - 500 - 0 while 500 is the middle (idle) stage
  // Reading Y - Axis Left to Right the pistions are 0 - 520 - 1023 while the 520 is the middle (idle) stage
  pinMode(sw_pin, INPUT);
  digitalWrite(sw_pin, HIGH);

}

void loop() {
  radio.begin();
  radio.openWritingPipe(pipes[0]);
  // radio.setPALevel(RF24_PA_HIGH);
  radio.setPALevel(RF24_PA_MAX);
  radio.enableDynamicPayloads(); // Not working without this
  Serial.print("X_pin: ");
  data.xAxis = analogRead(xJoystick); // Read Joysticks X-axis
  if (data.xAxis > 540) {
    radio.write(&data, sizeof(data));
    Serial.print( data.xAxis);
  } else if (data.xAxis < 470) {
    radio.write(&data, sizeof(data));
    Serial.print( data.xAxis);
  }

  Serial.print("  Y_pin: ");
  data.yAxis = analogRead(yJoystick); // Read Joysticks Y-axis
  if (data.yAxis > 540) {
    radio.write(&data, sizeof(data));
    Serial.print(data.yAxis);
  } else if (data.yAxis < 470) {
    radio.write(&data, sizeof(data));
    Serial.print(data.yAxis);
  }

  Serial.print("  SW: ");
  if (digitalRead(sw_pin) == LOW) {
    data.lights = 1;
    Serial.print(data.lights);
    radio.write(&data, sizeof(data));
  } else {
    data.lights = 0;
  }
  Serial.println("");
  // -----------------------------------------

  delay(10);
  // ****************************************

}
