#include <Wire.h>
#include <SPI.h>
#include <Wireling.h>

void setup() {
  Wire.begin();
  Wireling.begin();
  Wireling.selectPort(0);
  SerialUSB.begin(9600);
}

void loop() {
  SerialUSB.println(readMoisture());
  delay(250);
}

int readMoisture(){
  Wire.beginTransmission(0x30);
  Wire.write(1);
  Wire.endTransmission();
  delay(5);
  int c=0;
  Wire.requestFrom(0x30, 2);
  if(Wire.available()==2)
  { 
    c = Wire.read();
    c<<=8;
    c|=Wire.read();
  }
  return c;
}
