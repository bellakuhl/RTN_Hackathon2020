/*
 * Rui Santos 
 * Complete Project Details https://randomnerdtutorials.com
*/

int ledPin=13;
int analogSoundPin = A0;
int sensorPin=7;
boolean val =0;
int data = 0;

void setup(){
  pinMode(ledPin, OUTPUT);
  pinMode(sensorPin, INPUT);
  pinMode(analogSoundPin, INPUT);
  Serial.begin (9600);
}
  
void loop (){
  //delay(2000);
  val =digitalRead(sensorPin);
  data=analogRead(analogSoundPin);
  //Serial.println(val);
  //Serial.println(data);

  if (data > 14) {
    Serial.println("It works, too fast");
    digitalWrite(ledPin, LOW);
    delay(5000);
  }
  else {
    digitalWrite(ledPin, HIGH);
  }
}
