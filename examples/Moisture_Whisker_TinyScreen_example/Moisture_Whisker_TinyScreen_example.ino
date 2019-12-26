#include <Wire.h>
#include <SPI.h>
#include <TinyScreen.h>


//Library must be passed the board type
//TinyScreenDefault for TinyScreen shields
//TinyScreenAlternate for alternate address TinyScreen shields
//TinyScreenPlus for TinyScreen+
TinyScreen display = TinyScreen(TinyScreenPlus);


int offset = 700; // This is the value read off by the Moisture Sensor when completely dry
int range = 264; // The range og values from completely dry (700) to being submerged in water (964)

float moistness = 0.0;  // Variable that keeps raw reading from the sensor


void setup() { 
  Wire.begin(); //initialize I2C before we can initialize TinyScreen- not needed for TinyScreen+
  display.begin();
  SerialUSB.begin(9600);
  
  //setBrightness(brightness);  //sets main current level, valid levels are 0-15
  display.setBrightness(10);

  // Select the port the Whisker (moisture sensor) is connected to on the Adapter board
  selectPort(0);
  delay(100);

  // Take sensor reading to get an initial value for displaying the moisture bar
  moistness = readMoisture();

  moistness = (moistness - offset) / range;


}



void loop() {
    /*  SerialUSB.print("moistness: ");
  SerialUSB.println(moistness);
  SerialUSB.print("offset: ");
  SerialUSB.println(offset);
  SerialUSB.print("range: ");
  SerialUSB.println(range);*/
  // Get a moisture value
  moistness = readMoisture();
  moistness = (moistness - offset) / range;
  SerialUSB.println(moistness);
    delay(200);
  
}


// Read moisture from Soil Moisture Whisker
int readMoisture(){
  Wire.beginTransmission(0x23);
  Wire.write(1);
  Wire.endTransmission();
  delay(5);
  int c=0;
  Wire.requestFrom(0x23, 2);
  if(Wire.available()==2)
  { 
    c = Wire.read();
    c<<=8;
    c|=Wire.read();
  }
  return c;
}


void selectPort(int port) {
  Wire.beginTransmission(0x70);
  Wire.write(0x04 + port);
  Wire.endTransmission();
}
