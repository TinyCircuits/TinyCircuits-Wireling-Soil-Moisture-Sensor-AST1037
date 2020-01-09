/************************************************************************
 * Soil Moisture Sensor Wireling Example Sketch
 * This program uses four of the Wirelings included with the Starter Kit:
 * Port 0: Soil Moisture Sensor Wireling
 * 
 * This program will display the temperature and moisture detected by the
 * soil moisture sensor Wireling on the serial monitor.
 * 
 * Hardware by: TinyCircuits
 * Written by: Ben Rose and Hunter Hykes for TinyCircuits
 *
 * Initiated: 12/26/2019 
 * Updated: 01/09/2020
 ************************************************************************/

#include <Wire.h>
#include <SPI.h>
#include <Wireling.h>

// Make compatible with all TinyCircuits processors
#if defined(ARDUINO_ARCH_AVR)
#define SerialMonitorInterface Serial
#elif defined(ARDUINO_ARCH_SAMD)
#define SerialMonitorInterface SerialUSB
#endif

/* * * * * * MOISTURE SENSOR * * * * * * */
#define MOISTURE_PORT 0
#define MINCAPREAD 710
#define MAXCAPREAD 975
#define ANALOGREADMAX 1023
#define THERMISTORNOMINAL 10000
#define TEMPERATURENOMINAL 25
#define BCOEFFICIENT 3380
#define SERIESRESISTOR 35000

void setup() {
  Wire.begin();
  Wireling.begin();
  Wireling.selectPort(MOISTURE_PORT);
  SerialUSB.begin(9600);
}

void loop() {
  SerialMonitorInterface.print("M: ");
  SerialMonitorInterface.print(readMoisture());
  SerialMonitorInterface.print("\tT: ");
  SerialMonitorInterface.println(readTemp());
  delay(50);
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
    c <<= 8;
    c |= Wire.read();
    c = constrain(c, MINCAPREAD, MAXCAPREAD);
    c = map(c, MINCAPREAD, MAXCAPREAD, 0, 100);
  }
  return c;
}

float readTemp() {
  Wire.beginTransmission(0x30);
  Wire.write(2);
  Wire.endTransmission();
  delay(5);
  int c = 0;
  Wire.requestFrom(0x30, 2);
  if (Wire.available() == 2)
  {
    c = Wire.read();
    c <<= 8;
    c |= Wire.read();
    //https://learn.adafruit.com/thermistor/using-a-thermistor thanks!
    uint32_t adcVal = ANALOGREADMAX - c;
    uint32_t resistance = (SERIESRESISTOR * ANALOGREADMAX) / adcVal - SERIESRESISTOR;
    float steinhart = (float)resistance / THERMISTORNOMINAL;     // (R/Ro)
    steinhart = log(steinhart);                  // ln(R/Ro)
    steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
    steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
    steinhart = 1.0 / steinhart;                 // Invert
    steinhart -= 273.15;                         // convert to C
    return steinhart;
  }
  return c;
}
