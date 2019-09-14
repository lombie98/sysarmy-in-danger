//Arduino code to receive I2C communication from Raspberry Pi
 
#include <Wire.h>;
 
// Define the slave address of this device.
#define SLAVE_ADDRESS 0x04
int a=8;
 
// string to store what the RPi sends
String str_recieved_from_RPi = "";
 
void setup() {
 
  // setup the LED
  pinMode(a, OUTPUT);
 
  // begin running as an I2C slave on the specified address
  Wire.begin(SLAVE_ADDRESS);
  // start serial for output
  Serial.begin(9600);           
  // create event for receiving data
  Wire.onReceive(receiveData);
  
}
 
void loop() {
  // nothing needed here since we're doing event based code
}
 
void receiveData(int byteCount) {
 
   while (Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
   }
   
  // turn on or off the LED
  if (str_recieved_from_RPi == "on") {
    digitalWrite(a, HIGH); 
  }
  if (str_recieved_from_RPi == "off") {
    digitalWrite(a, LOW);
  }
 
  str_recieved_from_RPi = "";
 
}
