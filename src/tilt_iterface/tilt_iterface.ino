// ------------------------------
// Simple Tilt Interface
// ------------------------------

//Variables
int analogPin = A0; // Pin A0 on UNO
int val = 0;        // variable to store the value read

void setup() {
  Serial.begin(9600);           //  setup serial
}

void loop() {
  
  val = analogRead(analogPin);    // read the input pin
  Serial.print("Analog Value: "); // display on serial monitor
  Serial.print(val);              // debug value

  //When the tilt sensor is flat analog voltage reads ~ 1000
  if (val > 1000){  
    Serial.println(" FLAT");
  } 

  // When the tilt sensor is tilted the analog voltage reads ~ 100
  else if (val < 500) {
    Serial.println(" TILTED");
  }
  
}
