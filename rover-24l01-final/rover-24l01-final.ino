/*
   Ultrasonic Sensor HC-SR04
*/
const int trigPin = 7;
const int echoPin = 8;
long duration;
long distance;

/*
     FL []=-|-=[] FR
            |
            |
            |
     RL []-=|=-[] RR
*/

// This is the library for the Servo motor micro
#include <Servo.h>
Servo servo; // using the Class Servo
const int servo_pin = 6;
long sval = 0;

// This is the library for the TB6612 that contains the class Motor and all the
// functions
#include <SparkFun_TB6612.h>
// Pins for all inputs, keep in mind the PWM defines must be on PWM pins
// the default pins listed are the ones used on the Redbot (ROB-12097) with
// the exception of STBY which the Redbot controls with a physical switch
#define AIN1 2
#define AIN2 4
#define PWMA 5
#define STBY 3  // Changed after Rover 1.0 with 433MHz, so we need for 2.4GHz 24l01 the 9, 10 pins for CSN, CE
// these constants are used to allow you to make your motor configuration
// line up with function names like forward.  Value can be 1 or -1
const int offsetA = 1;
// Initializing motors.  The library will allow you to initialize as many
// motors as you have memory for.  If you are using functions like forward
// that take 2 motors as arguements you can either write new functions or
// call the function more than once.
Motor motor1 = Motor(AIN1, AIN2, PWMA, offsetA, STBY);
long mval = 0;
long mval1 = 0;



// Define the LED for the Incoming Transmitter commands



// Head and Back lights
const int stop_led_pin = 11;
const int front_led_pin = 12;
int turnStopLights = 0;
int turnFrontLights = 0;


// 2.4GHz 24L01 Transiever settings
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10); // CE, CSN
const uint64_t pipes[2] = { 0xEEFDFDFDECLL, 0xEEFDFDF0DFLL };
//char receivedData[32] = "";


struct package {
  int xAxis;
  int yAxis;
  int lights;
  int stopLights;
};

typedef struct package Package;
Package data;




void setup() {
  Serial.begin(57600);
  Serial.println("******** SKL Electronics *******");
  Serial.println("System started. Tests will start in 1 sec.");
  Serial.println("------------------------- - -----------------------");
  // Servo settings starts here *****
  servo.attach(servo_pin); // Setting pin 6 to be OUTPUT to the servo motor
  Serial.println("Servo diagnostics starts.");
  Serial.println("Servo go to middle position!");


  //  Turn the backlights off  and setup the pinMode
  // pinMode(backLights, OUTPUT);
  pinMode(front_led_pin, OUTPUT);
  pinMode(stop_led_pin, OUTPUT);
  //  digitalWrite(backLights, LOW);


  // 2.4GHz Settings starts here
  radio.begin();
  radio.openReadingPipe(1, pipes[0]);
  // radio.setPALevel(RF24_PA_HIGH);
  radio.setPALevel(RF24_PA_MAX);
  radio.enableDynamicPayloads(); // not working without this
  radio.startListening();
  // 2.4GHz Settings ends here

  // HC-SR04 Ultrasonic sensor starts here
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);
  // HC-SR04 Ultrasonic sensor starts here


  Serial.println("End of diagnostics.");
 // delay(1000);
  Serial.println("System boot finished.");
  Serial.println("------------------ --- ---------------------");

}


void loop() {
  if (radio.available()) {
    radio.read(&data, sizeof(data)); // Read the data and put it into the data object

    Serial.print("  X: ");
    Serial.print(data.xAxis);
    Serial.print("  Y: ");
    Serial.print(data.yAxis);
    Serial.print("  SW: ");
    Serial.print(data.lights);
    Serial.println("");

    delay(10);

    if (data.xAxis > 540) {
      mval = map(data.xAxis, 540, 1023, 0, -255);
      motor1.drive(mval, 10);
      Serial.println("MOTOR GO FORD");
    } else if (data.xAxis < 470) {
      mval = map(data.xAxis, 470, 0, 0, 255);
      motor1.drive(mval, 10);
      Serial.println("MOTOR GO BACK");
    } else {
      motor1.standby();
    }
    if (data.yAxis > 540) {
      sval = map(data.yAxis, 530, 1023, 110, 190);
      servo.write(sval);
    } else if (data.yAxis < 470) {
      sval = map(data.yAxis, 490, 0, 110, 40);
      servo.write(sval);
    } else {
      servo.write(110);
    }

    if (data.lights == 1) {
      if (turnFrontLights == 0) {
        digitalWrite(front_led_pin, HIGH);
        turnFrontLights = 1;
        Serial.println("FRONT LIGHTS ON");
      } else if (turnFrontLights == 1) {
        digitalWrite(front_led_pin, LOW);
        turnFrontLights = 0;
        Serial.println("Front LIGHTS OFF");
      }
    }

    if (data.stopLights == 1) {
      if (turnStopLights == 0) {
        digitalWrite(stop_led_pin, HIGH);
        turnStopLights = 1;
        Serial.println("STOP LIGHTS ON");
      } else if (turnStopLights == 1) {
        digitalWrite(stop_led_pin, LOW);
        turnStopLights = 0;
        Serial.println("STOP LIGHTS OFF");
      }
    }

  } else {
    Serial.println("No command Recieved");
    motor1.standby();
  }
}


