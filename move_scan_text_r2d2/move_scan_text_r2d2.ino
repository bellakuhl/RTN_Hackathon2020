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
#define MAX_L 128 // max speed compared to max speed of right motor
#define MAX_R 160 // max speed compared to max speed of left motor
#define REVERSE_F -80 // reverse fast
#define REVERSE_S -40 // reverse slow

// Define hardware type, size, and output pins:
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 1 //number of matrix leds used
#define CS_PIN 3
#define speakerPin 8

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
unsigned char sensorAngle[NUM_ANGLES] = { 30, 70, 80, 90, 100, 110, 150 };
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

void r2d2() {
    int K = 2000;
    switch (random(1,5)) {
        case 1:phrase1(); break;
        case 2:phrase2(); break;
        case 3:phrase1(); phrase2(); break;
        case 4:phrase1(); phrase2(); phrase1();break;
        case 5:phrase2(); phrase1(); phrase2(); break;
    }
    noTone(speakerPin);         
    delay(random(1000, 2000));  
}

void phrase1() {
    
    int k = random(1000,2000);
    for (int i = 0; i <=  random(100,2000); i++){
        tone(speakerPin, k+(-i*2));          
        delay(random(.9,2));             
    } 
    for (int i = 0; i <= random(100,1000); i++){
        tone(speakerPin, k + (i * 10));          
        delay(random(.9,2));             
    } 
}

void phrase2() {
    
    int k = random(1000,2000);
    for (int i = 0; i <= random(100,2000); i++){
        tone(speakerPin, k+(i*2));          
        delay(random(.9,2));             
    } 
    for (int i = 0; i <= random(100,1000); i++){
        tone(speakerPin, k + (-i * 10));          
        delay(random(.9,2));             
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
  pinMode(speakerPin, OUTPUT);

  // random seed for r2d2 sounds
  randomSeed(analogRead(0));

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
bool avoided = 0;
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
      //i = NUM_ANGLES;
    }
    //Serial.println(i);
  }
  for (int i = 0; i < 7; i++) {
    Serial.print(distance[i]);
    Serial.print(" ");
  }
  Serial.println();
  if (tooClose == 1 && avoided == 0) {
    avoid(detectedAngle);
    avoided = 1;
    r2d2(); // play r2d2 sound
  } else {
    // Nothing in our way: go forward
    go(LEFT, MAX_L);
    go(RIGHT, MAX_R);
    avoided = 0;
  }
  // Check the next direction in 50 ms
  delay (50);
}
