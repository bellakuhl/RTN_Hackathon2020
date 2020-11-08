// program that turns motos forward and backwards to test orientation

const int enBPin = 6; // Left motor PWM speed control
const int in3Pin = 7; // Left motor Direction 1
const int in4Pin = 5; // Left motor Direction 2
const int in1Pin = 4; // Right motor Direction 1
const int in2Pin = 2; // Right motor Direction 2
const int enAPin = 1; // Right motor PWM speed control //changed from 3
#define TRAVEL_TIME 500 // travel for half a sec
enum Motor { LEFT, RIGHT };

void setup() {
  pinMode(enAPin, OUTPUT);
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(in3Pin, OUTPUT);
  pinMode(in4Pin, OUTPUT);
  pinMode(enBPin, OUTPUT);
  Serial.begin(9600);

}

void loop() {
  //testMotors();
  //go(RIGHT, 200);
  testMotors();
  //delay(5000);
}

// Set motor speed: 255 full ahead, -255 full reverse , 0 stop
void go( enum Motor m, int speed){
  /*digitalWrite (m == LEFT ? in1Pin : in3Pin , speed > 0 ? HIGH : LOW );
  digitalWrite (m == LEFT ? in2Pin : in4Pin , speed <= 0 ? HIGH : LOW );
  analogWrite(m == LEFT ? enAPin : enBPin, speed < 0 ? -speed : speed );*/
  if (m == LEFT) {
    digitalWrite(in1Pin, HIGH);
    analogWrite(m == LEFT ? enAPin : enBPin, speed < 0 ? -speed : speed );
    delay(TRAVEL_TIME);
    digitalWrite(in1Pin, LOW);
  } else {
    digitalWrite(in2Pin, HIGH);
    analogWrite(m == LEFT ? enAPin : enBPin, speed < 0 ? -speed : speed );
    delay(TRAVEL_TIME);
    digitalWrite(in2Pin, LOW);
  }
}

// Initial motor test :
// left motor forward then back
// right motor forward then back
void testMotors () {
{
static int speed[8] = { 128, 255, 128, 0 ,-128, -255, -128, 0};
go(RIGHT, 0);
go(LEFT, 0);
for (unsigned char i = 0 ; i < 8 ; i++)
go(LEFT, speed[i ]), delay (200);
for (unsigned char i = 0 ; i < 8 ; i++)
go(RIGHT, speed[i ]), delay (200);
}
}
