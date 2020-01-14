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
#define MIN_CAP_READ 710 /* Toggle this to raw minimum value */
#define MAX_CAP_READ 975 /* Toggle this to raw maximum value */

#define ANALOG_READ_MAX 1023
#define THERMISTOR_NOMINAL 10000
#define TEMPERATURE_NOMINAL 25
#define B_COEFFICIENT 3380
#define SERIES_RESISTOR 35000

void setup() {
  Wire.begin();
  Wireling.begin();
  delay(10);
  Wireling.selectPort(MOISTURE_PORT);
  SerialMonitorInterface.begin(9600);
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
    c = constrain(c, MIN_CAP_READ, MAX_CAP_READ);
    c = map(c, MIN_CAP_READ, MAX_CAP_READ, 0, 100);
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
    uint32_t adcVal = ANALOG_READ_MAX - c;
    uint32_t resistance = (SERIES_RESISTOR * ANALOG_READ_MAX) / adcVal - SERIES_RESISTOR;
    float steinhart = (float)resistance / THERMISTOR_NOMINAL;     // (R/Ro)
    steinhart = log(steinhart);                  // ln(R/Ro)
    steinhart /= B_COEFFICIENT;                   // 1/B * ln(R/Ro)
    steinhart += 1.0 / (TEMPERATURE_NOMINAL + 273.15); // + (1/To)
    steinhart = 1.0 / steinhart;                 // Invert
    steinhart -= 273.15;                         // convert to C
    return steinhart;
  }
  return c;
}
