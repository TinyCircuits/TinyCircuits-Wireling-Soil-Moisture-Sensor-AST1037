/************************************************************************
 * Soil Moisture Sensor Wireling Example Sketch
 * This program uses one of the Wirelings included with the Starter Kit:
 * Port 0: Soil Moisture Sensor Wireling
 * 
 * This program will display the temperature and moisture detected by the
 * soil moisture sensor Wireling on the serial monitor.
 * 
 * Hardware by: TinyCircuits
 * Written by: Ben Rose and Hunter Hykes for TinyCircuits
 *
 * Initiated: 12/26/2019 
 * Updated: 01/05/2021
 ************************************************************************/

#include <Wire.h>
#include <SPI.h>
#include <Wireling.h>
#include <ATtiny25.h>      // For ATtiny25 sensor

// Make compatible with all TinyCircuits processors
#if defined(ARDUINO_ARCH_AVR)
#define SerialMonitorInterface Serial
#elif defined(ARDUINO_ARCH_SAMD)
#define SerialMonitorInterface SerialUSB
#endif

/* * * * * * MOISTURE SENSOR * * * * * * */
#define MOISTURE_PORT 0 
ATtiny25 moisture_sensor;

void setup() {
  Wire.begin();
  Wireling.begin();
  delay(10);
  Wireling.selectPort(MOISTURE_PORT);
  SerialMonitorInterface.begin(9600);
}

void loop() {
  SerialMonitorInterface.print("M: ");
  SerialMonitorInterface.print(moisture_sensor.readMoisture());
  SerialMonitorInterface.print("\tT: ");
  SerialMonitorInterface.println(moisture_sensor.readTemp());
  delay(50);
}
