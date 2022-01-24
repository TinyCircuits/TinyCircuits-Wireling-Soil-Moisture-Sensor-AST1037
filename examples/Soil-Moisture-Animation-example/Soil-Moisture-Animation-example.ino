/************************************************************************
 * Soil Moisture Wireling TinyScreen+ Animation Sketch
 * Port 0: Soil Moisture Sensor Wireling
 * 
 * This program will display an animation for the moisture levels read 
 * on a scale of 0-100 by the soil moisture sensor.
 * 
 * Hardware by: TinyCircuits
 * Written by: Jason Marcum & Laveréna Wienclaw for TinyCircuits
 *
 * Initiated: July 2019 
 * Updated: January 2021
 *  - updated to be compatible with the ATtiny25 library used by the Soil
 *  Moisture Sensor Wireling
 ************************************************************************/

#include <Wire.h>
#include <SPI.h>
#include <TinyScreen.h>
#include "TinyAnimation.h"
#include <Wireling.h>
#include <ATtiny25.h>      // For ATtiny25 sensor on the Soil Moisture Sensor Wireling

//Library must be passed the board type
//TinyScreenDefault for TinyScreen shields
//TinyScreenAlternate for alternate address TinyScreen shields
//TinyScreenPlus for TinyScreen+
TinyScreen display = TinyScreen(TinyScreenPlus);

// ***Setup objects for displaying on the screen***

//This is an 8 x 5 pixel bitmap using TS library color definitions for raindrop/watering animation
unsigned char RainDrop[8*5]={
  TS_8b_Black, TS_8b_Black, TS_8b_Blue, TS_8b_Black, TS_8b_Black,
  TS_8b_Black, TS_8b_Blue, TS_8b_Blue,  TS_8b_Blue,  TS_8b_Black,
  TS_8b_Black, TS_8b_Blue, TS_8b_Blue,  TS_8b_Blue,  TS_8b_Black,
  TS_8b_Blue,  TS_8b_White, TS_8b_Blue,  TS_8b_Blue,  TS_8b_Blue,
  TS_8b_Blue,  TS_8b_Blue, TS_8b_White,  TS_8b_Blue,  TS_8b_Blue,
  TS_8b_Black, TS_8b_Blue, TS_8b_Blue,  TS_8b_Blue,  TS_8b_Black
};

PercentBarHorizontal bar(display, 2, 2, 94, 14, TS_8b_Gray, TS_8b_Blue, TS_8b_Green, 0, "dry", "wet", true);
IdleCircle idle(display, 75, 47, TS_8b_Gray, TS_8b_White, 9, 1, 3);
FallDownSprite animator(display, 70, 34, 5, 5, 5, 100, RainDrop);


/* * * * * * MOISTURE SENSOR * * * * * * */
#define MOISTURE_PORT 0 
ATtiny25 moisture_sensor;
float moistness = 0.0;  // Variable that keeps raw reading from the sensor


// ***Setup variables for getting readings from the moisture sensor***
// These are close to correct values but calibration will make it more accurate
int offset = 0; // This is the value read off by the Moisture Sensor when completely dry
int range = 100; // The range og values from completely dry (700) to being submerged in water (964)

// Used to smooth out readings from moisture sensor 
// (reduces bar jitter due to fast refresh rate/readings)
int moist_readings_counter = 0;
float moist_readings_sum = 0.0;

const int avg_moist_size = 8;
float avg_moistness = 0.0; // Actual avg moisture
float avg_moistness_last = 0.0; // Used to turn on animation


// 0 = no calibration
// 1 = display hold sensor in air
// 2 = take dry readings display hold sensor in water
// 3 = take wet readings end
byte calibration_flag = 0;
bool write_once_cali_flag = false;

bool animation_on = false; // Used to display certain animations at correct times

// Make Serial Monitor compatible for all TinyCircuits processors
#if defined(ARDUINO_ARCH_AVR)
  #define SerialMonitorInterface Serial
#elif defined(ARDUINO_ARCH_SAMD)
  #define SerialMonitorInterface SerialUSB
#endif


void setup(void) {
  Wire.begin(); //initialize I2C before we can initialize TinyScreen- not needed for TinyScreen+
  display.begin();
  SerialMonitorInterface.begin(9600);
  
  display.setBrightness(10);

  // Select the port the Wireling (moisture sensor) is connected to on the Adapter board
  Wireling.begin();
  Wireling.selectPort(MOISTURE_PORT);
  delay(100);

  // Take sensor reading to get an initial value for displaying the moisture bar
  moistness = moisture_sensor.readMoisture();
  moistness = (moistness - offset) / range;

  // Apply the letters to the screen once so we don't have to every tick
  bar.updateBarEndInfo();

  // Show a message about what the plant is doing
  ShowIdleMessage();
}

void loop() {
  // Only show monitoring information if not currently in calibration mode
  if(calibration_flag == 0){
    // animation is on? Make raindrops!
    if(animation_on){
      // If got to the last frame then stop animation next time
      if(animator.tick()){
        animation_on = false;
        
        // Show a message about what the plant is doing
        ShowIdleMessage();
      }
    }
  
    // If no animation/not being watered show idle circle
    if(!animation_on){
      idle.tick();
    }
    
    // Get a moisture value
    moistness = moisture_sensor.readMoisture();
  
    // Offset the reading to put in range of 0 to 100
    moistness = (moistness - offset) / range;
  
    // Handle getting an average and updating the percent bar
    HandleMostureReading(moistness);
  
    // Press button by power switch to start sensor cailbration or enter c
    if(display.getButtons(TSButtonUpperRight) || (SerialMonitorInterface.available() > 0 && SerialMonitorInterface.read() == 'c')){
      SerialMonitorInterface.println("\nCalibration started!\n");
      calibration_flag++;
      write_once_cali_flag = false;
      
      // Delay so last button press doesn't register right away
      delay(800);
    }
  }

  // Handle the calibration steps
  if(calibration_flag >= 1){
    if(display.getButtons(TSButtonUpperRight|TSButtonUpperLeft|TSButtonLowerRight|TSButtonLowerLeft) || (SerialMonitorInterface.available() > 0 && SerialMonitorInterface.read() == 'n')){
      calibration_flag++; // Move on to next stwp of calibration
      write_once_cali_flag = false;
      
      // Delay so last button press doesn't register right away
      delay(800);
    }

    // Have to use flags since infinite loops don't allow new input from serial
    CalibrateMoisutreSensor();
  }
}

// Handles averaging and updating percent bar
void HandleMostureReading(float offset_moisture_reading){
  // Sum for avg's numerator
  moist_readings_sum += offset_moisture_reading;

  // Increment the tracker/counter
  moist_readings_counter += 1;

  // Calculate the average when the counter reaches the size
  if(moist_readings_counter == avg_moist_size){
    avg_moistness = moist_readings_sum/avg_moist_size;

    // Reset these for next average/reading
    moist_readings_counter = 0;
    moist_readings_sum = 0.0;
  }

  // Animate/update text for the percent bar
  bar.tick(avg_moistness);

  // Display plant status to the serial monitor
  if(!animation_on){
    SerialMonitorInterface.print("Plant is -> IDLE | Moisture level = ");
  }else{
    SerialMonitorInterface.print("Plant is -> HYDRATING | Moisture level = ");
  }

  // Display the moisture level to the serial monitor
  SerialMonitorInterface.println(avg_moistness);

  // Looks like a big jump, turn on animation and clear idle animation
  if(avg_moistness - avg_moistness_last > 0.1){
    animation_on = true;

    // Show a message about what the plant is doing
    ShowHydratingMessage();
    
    idle.erase();
  }

  avg_moistness_last = avg_moistness;
}

void ShowHydratingMessage(){
  display.setFont(thinPixel7_10ptFontInfo);
  display.fontColor(TS_8b_Green, TS_8b_Black);
  display.setCursor(0, 40);
  display.println("Plant is:");
  display.setCursor(0, 50);
  display.println("->");
  display.fontColor(TS_8b_Blue, TS_8b_Black);
  display.setCursor(11, 50);
  display.println("Hydrating");
}

void ShowIdleMessage(){
  display.setFont(thinPixel7_10ptFontInfo);
  display.fontColor(TS_8b_Green, TS_8b_Black);
  display.setCursor(0, 40);
  display.println("Plant is:");
  display.setCursor(0, 50);
  display.println("->");
  display.fontColor(TS_8b_Gray, TS_8b_Black);
  display.setCursor(11, 50);
  display.println("Idle     ");
}




// Used to calibrate the moisture sensor
void CalibrateMoisutreSensor(){

  // Only write stuff to the screen once, avoids flicker
  if(write_once_cali_flag)
    return;

  // Handle each flag for the calibration process
  switch(calibration_flag){
    case 1:
      display.clearScreen();
      display.setFont(thinPixel7_10ptFontInfo);
      display.fontColor(TS_8b_Green,TS_8b_Black);
      display.setCursor(0, 0);
      
      display.println("Hold sensor in the");
      display.setCursor(0, 10);
      display.fontColor(0x90, TS_8b_Black);
      display.println("AIR");
      display.fontColor(TS_8b_Green, TS_8b_Black);
      display.setCursor(22, 10);
      display.println("and press any");
      display.setCursor(0, 20);
      display.println("button");
      display.setCursor(0, 30);
      display.fontColor(TS_8b_Red, TS_8b_Black);
      display.println("(hold by wire only!)");
      display.fontColor(TS_8b_Green, TS_8b_Black);
      SerialMonitorInterface.println("Hold the sensor in the AIR and press any button or enter 'n' (hold by the wire only!)");
      write_once_cali_flag = true;
    break;
    case 2:
    
      display.clearScreen();
      display.setCursor(0, 0);
      display.println("Keep holding");
      SerialMonitorInterface.println("Keep holding the sensor");
      display.setCursor(0, 10);
      display.println("Taking readings...");
      SerialMonitorInterface.println("Taking readings...");
    
      // Take the dry readings
      moist_readings_sum = 0;
    
      for(int i=0; i<100; i++){
        
        moist_readings_sum += moisture_sensor.readMoisture();
    
        // Space readings apart
        delay(25);
      }
    
      // Dry readings are the offset (use average of 5 readings)
      offset = moist_readings_sum/100;
    
      display.clearScreen();
      display.setCursor(0, 0);
      display.println("Dry readings done");
      SerialMonitorInterface.println("Dry readings done");
      display.setCursor(0, 20);
      display.println("offset = " + String(offset));
      SerialMonitorInterface.println("offset = " + String(offset));
      delay(2000);
      write_once_cali_flag = true;
    
      display.setCursor(0, 0);
      display.clearScreen();
      display.println("Submerge sensor in");
      display.setCursor(0, 10);
      display.fontColor(TS_8b_Blue, TS_8b_Black);
      display.println("WATER");
      display.fontColor(TS_8b_Green, TS_8b_Black);
      display.setCursor(31, 10);
      display.println("and press any");
      display.setCursor(0, 20);
      display.println("button");
      SerialMonitorInterface.println("Submerge the sensor in WATER and press any button or enter 'n'");
      write_once_cali_flag = true;
    break;
    case 3:
      display.clearScreen();
      display.setCursor(0, 0);
      display.println("Keep holding");
      SerialMonitorInterface.println("Keep holding the sensor");
      display.setCursor(0, 10);
      display.println("Taking readings...");
      SerialMonitorInterface.println("Taking readings...");
    
      // Take the submerged readings
      moist_readings_sum = 0;
    
      for(int i=0; i<100; i++){
        
        moist_readings_sum += moisture_sensor.readMoisture();
    
        // Space readings apart
        delay(25);
      }
    
      range = (moist_readings_sum/100) - offset;
    
      // Range wasn't big enough, error!
      if(range <= 0){
        display.clearScreen();
        display.setCursor(0, 0);
        display.fontColor(TS_8b_Red, TS_8b_Black);
        display.println("Range not big");
        display.setCursor(0, 10);
        display.println("enough!");
        display.setCursor(0, 30);
        display.fontColor(TS_8b_Green, TS_8b_Black);
        display.println("Set to defaults...");
        SerialMonitorInterface.println("The range is not big enough! Setting offset and range back to defaults");
    
        // Give an extra second to read error
        delay(1000);
    
        offset = 0;
        range = 100;
      }else{
        display.clearScreen();
        display.setCursor(0, 0);
        display.println("Wet readings done");
        SerialMonitorInterface.println("Wet readings done");
        display.setCursor(0, 20);
        display.println("range = " + String(range));
        SerialMonitorInterface.println("range = " + String(range));
      }
    
      // Delay so last button press doesn't register right away and to give time to read
      delay(2000);
      display.clearScreen();
      delay(100);
      moist_readings_sum = 0;
      moist_readings_counter = 0;
      bar.updateBarEndInfo();
    
      // Show a message about what the plant is doing
      ShowIdleMessage();

      calibration_flag = 0;
    break;
  }
}
