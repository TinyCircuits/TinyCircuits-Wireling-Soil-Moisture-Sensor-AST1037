// Written by Jason Marcum for TinyCircuits
// Initiated: July 2019

#include "Arduino.h"
#include "TinyAnimation.h"

void PercentBarHorizontal::tick(float percent_){
  // To keep track
  percent = percent_;

  // Only update bar if percent has changed
  if(percent != last_percent){
    // Draw the outline of the percent bar (always not filled)
    display.drawRect(x, y, width, height, TSRectangleNoFill, outline_color);
    
    // Calculate how much internal bar should be displayed
    int bar_x = (width-2) * percent;
    
    // Handle <= 0 separately since can't give draw rect zero for x
    // Also handle the bar going out of bounds when percent is more than 100%
    if(bar_x > 0 && percent < 1.0){
    
      // Display the internal bar (offset due to outline thickness)
      display.drawRect(x+1, y+1, bar_x, height-2, TSRectangleFilled, bar_color);
      
      // Calculate width of bar that's empty/needs to be cleared
      int clear_width = (width-bar_x);
      
      // Clear/overwrite the empty side of the bar (only do so if there is empty space)
      if (bar_x != width && clear_width-2 > 0){
        display.drawRect(x+bar_x+1, y+1, clear_width-2, height-2, TSRectangleFilled, TS_8b_Black);
      }
    
    }else if(bar_x <= 0){
      // The bar is empty, overwrite the whole thing with black
      display.drawRect(x+1, y+1, width-2, height-2, TSRectangleFilled, TS_8b_Black);
    }else if(percent >= 1.0){
      // The bar is full, overwrite the whole thing with bar color
      display.drawRect(x+1, y+1, width-2, height-2, TSRectangleFilled, bar_color);
    }
  }

  // If true, show the percentage text under the bar (always update text)
  if(show_percentage && percent >= 0){
    display.setFont(thinPixel7_10ptFontInfo);

    // Hard offset of 15 is fine, percent character size doesn't change
    display.setCursor(x+(width/2)-15, y+height+1);
    display.fontColor(font_color, TS_8b_Black);
    display.print(String(percent * 100) + "% ");
  }
}


// Function to do this once so it's not done every time/tick
void PercentBarHorizontal::updateBarEndInfo(){
  display.setFont(thinPixel7_10ptFontInfo);
  display.setCursor(x, y+height);
  display.fontColor(font_color, TS_8b_Black);
  display.print(left_end);
  
  display.setCursor(x+width - display.getPrintWidth(right_end), y+height);
  display.fontColor(font_color, TS_8b_Black);
  display.print(right_end);
}


void PercentBarHorizontal::erase(){
  // The constants here are for offsets and the letters under the bar
  display.drawRect(x, y, width, height+10, TSRectangleFilled, TS_8b_Black);
}



void IdleCircle::tick(){

  if(tick_counter == tick_fallover_limit){

    int x_last_last = x + (radius * cos(((angle_step_tracker - (angle_step*60)) * 71.0) / 4068.0));
    int y_last_last = y + (radius * sin(((angle_step_tracker - (angle_step*60)) * 71.0) / 4068.0));
    
    int x_last = x + (radius * cos(((angle_step_tracker - angle_step) * 71.0) / 4068.0));
    int y_last = y + (radius * sin(((angle_step_tracker - angle_step) * 71.0) / 4068.0));
  
    int x_cur = x + (radius * cos((angle_step_tracker * 71.0) / 4068.0));
    int y_cur = y + (radius * sin((angle_step_tracker * 71.0) / 4068.0));

    display.drawRect(x_last_last - (thickness/2), y_last_last - (thickness/2), thickness, thickness, TSRectangleFilled, TS_8b_Black);
    display.drawRect(x_last - (thickness/2), y_last - (thickness/2), thickness, thickness, TSRectangleFilled, TS_8b_Gray);
    display.drawRect(x_cur - (thickness/2), y_cur - (thickness/2), thickness, thickness, TSRectangleFilled, TS_8b_White);

    angle_step_tracker += angle_step;

    // Don't overstep the bounds!
    if(angle_step_tracker >= 360){
      angle_step_tracker -= 360;
    }

    // Reset for more tracking
    tick_counter = 0;
  }else{

    // Track!
    tick_counter += 1;
  }
}


void IdleCircle::erase(){
  display.drawRect(x - radius, y - radius, (radius * 2) + 1, (radius * 2) + 1, TSRectangleFilled, TS_8b_Black);
}



bool FallDownSprite::tick(){
  // Increment this every time this function is called
  animation_tick_counter += 1;

  // Did we reach the next frame? If so, animate!
  if(animation_tick_counter == animation_tick_size){
    animation_frame_counter += 1;

    // New frame, clear the old raindrops
    display.drawRect(x, y, 10, 26, TSRectangleFilled, TS_8b_Black);

    // When on even number of frames stay at given x, otherwise move over x_spacing pixels
    x_cur_pos = x + (abs(animation_frame_counter%2) * x_spacing);

    // Go down y_spacing pixels every frame
    y_cur_pos = y + (animation_frame_counter * y_spacing);

    // As long as we're not at the end of the frame limit write next frame
    if(animation_frame_counter != sprite_count){
      display.setX(x_cur_pos, x_cur_pos+5-1);
      display.setY(y_cur_pos, y_cur_pos+8-1);
      display.startData();
      display.writeBuffer(sprite, 5*8);
      display.endTransfer();
    }else{
      animation_frame_counter = 0;
      animation_tick_counter = 0;
      x_cur_pos = 0;
      y_cur_pos = 0;

      // Reached last frame, return true
      return true;
    }

    animation_tick_counter = 0;
  }

  // Not at last frame yet, return false
  return false;
}
