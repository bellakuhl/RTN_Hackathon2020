/* program to scan for objects while moving straight 
servo mounted on front of bot scans for objects within certain distance
When object detected, bot reverses to the right
Also included Dot Matrix with scrolling text*/

#include <Servo.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

Servo servo;

// Ultrasonic Module pins
const int trigPin = 10; // 10 microsecond high pulse causes chirp , wait 50 us //changed from 13
const int echoPin = 12; // Width of high pulse indicates distance
// Servo motor that aims ultrasonic sensor .
const int servoPin = 9; // PWM output for hobby servo // changed from 11
// Motor control pins : L298N H bridge
const int enBPin = 6; // Left motor PWM speed control
const int in3Pin = 7; // Left motor Direction 1
const int in4Pin = 5; // Left motor Direction 2
const int in1Pin = 4; // Right motor Direction 1
const int in2Pin = 2; // Right motor Direction 2
const int enAPin = 1; // Right motor PWM speed control //changed from 3

enum Motor { LEFT, RIGHT };

#define TOO_CLOSE_DIST 150 // object distance thats considered too close (mm)

// compensate for motor differences
#define MAX_L 145 // max speed compared to max speed of right motor
#define MAX_R 185 // max speed compared to max speed of left motor
#define REVERSE_F -90 // reverse fast
#define REVERSE_S -50 // reverse slow

// Define hardware type, size, and output pins:
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 1 //number of matrix leds used
#define CS_PIN 3

// Create a new instance of the MD_Parola class with hardware SPI connection:
MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

// Set motor speed: 255 full ahead, -255 full reverse , 0 stop
void go( enum Motor m, int speed){
  digitalWrite (m == LEFT ? in1Pin : in3Pin, speed > 0 ? HIGH : LOW );
  digitalWrite (m == LEFT ? in2Pin : in4Pin, speed <= 0 ? HIGH : LOW );
  analogWrite(m == LEFT ? enAPin : enBPin, speed < 0 ? -speed : speed );
}


// Read distance from the ultrasonic sensor , return distance in mm
//
// Speed of sound in dry air , 20C is 343 m/s
// pulseIn returns time in microseconds (10ˆ−6)
// 2d = p * 10ˆ−6 s * 343 m/s = p * 0.00343 m = p * 0.343 mm/us
unsigned int readDistance ()
{
  digitalWrite ( trigPin , HIGH );
  delayMicroseconds (10);
  digitalWrite ( trigPin , LOW );
  unsigned long period = pulseIn ( echoPin, HIGH );
  return period * 343 / 2000;
}

#define NUM_ANGLES 7
unsigned char sensorAngle[NUM_ANGLES] = { 60, 70, 80, 90, 100, 110, 120 };
unsigned int distance [NUM_ANGLES];

// Scan the area ahead by sweeping the ultrasonic sensor left and right
// and recording the distance observed. This takes a reading , then
// sends the servo to the next angle. Call repeatedly once every 50 ms or so.
void readNextDistance () {
  static unsigned char angleIndex = 0; // used in array array to go from 60 to 120 degrees
  static signed char step = 1;
  distance[angleIndex] = readDistance(); // get distance reading, store in array
  angleIndex += step;
  
  if (angleIndex == NUM_ANGLES - 1) step = -1; // if at max angle, rotate back the other way
  else if (angleIndex == 0) step = 1;
    servo.write( sensorAngle[angleIndex ] );
}

// blick to indicate avoiding obstacle
void blink_indicate() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(200);                       // wait for half a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(200);                       // wait for half a second
}

// logic to avoid obstacle based on postion
void avoid(int i) {
  if (i < 3) { // if obstacle is detected at an angle of 60, 70, or 80
    go(LEFT, REVERSE_F); // reverse right
    go(RIGHT, REVERSE_S);
  } else if (i == 3) { // if obstacle is at 90 degrees (straight ahead)
    go(LEFT, REVERSE_F); // reverse right (arbitrary choice)
    go(RIGHT, REVERSE_S);
  } else {
    go(LEFT, REVERSE_S); // reverse left
    go(RIGHT, REVERSE_F);
  }
}

// Initial configuration
void setup () {
  
  // Configure the input and output pins
  pinMode(trigPin , OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite ( trigPin , LOW);
  pinMode(enAPin, OUTPUT);
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(in3Pin, OUTPUT);
  pinMode(in4Pin, OUTPUT);
  pinMode(enBPin, OUTPUT);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  // Intialize the object:
  myDisplay.begin();
  // Set the intensity (brightness) of the display (0-15):
  myDisplay.setIntensity(0);
  // Clear the display:
  myDisplay.displayClear();
  myDisplay.displayText("Go!", PA_CENTER, 100, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  
  // display text (scrolling finishes after the amount of time required to count to 25000)
  for (int i = 0; i < 25000; i++) {
    if (myDisplay.displayAnimate()) {
      myDisplay.displayReset();
    }
  }
  
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);

  // Center the servo
  servo.attach( servoPin );
  servo.write(90);
  
  // Scan the surroundings before starting
  servo.write(sensorAngle[0]);
  delay (200);
  
  for (unsigned char i = 0; i < NUM_ANGLES; i++) {
    readNextDistance(), delay(200);
  }
}

// Main loop:
//
// Get the next sensor reading
// If anything appears to be too close , back up
// Otherwise, go forward
//
void loop () {
  // print smiley face
  myDisplay.print(":D");
  // Get the next sensor reading
  readNextDistance ();
  
  // See if something is too close at any angle
  unsigned char tooClose = 0;
  int detectedAngle = 0;
  
  for (unsigned char i = 0 ; i < NUM_ANGLES ; i++) {
    if ( distance[i] < TOO_CLOSE_DIST) {
      tooClose = 1; 
      detectedAngle = i; // save index of angle that object is detected at
    }
  }
//Serial.println(distance[0]);
  if (tooClose) {
    avoid(detectedAngle);
  } else {
    // Nothing in our way: go forward
    go(LEFT, MAX_L-50); // minus 50 to normal speed
    go(RIGHT, MAX_R-50);
  }
  // Check the next direction in 50 ms
  delay (50);
}
