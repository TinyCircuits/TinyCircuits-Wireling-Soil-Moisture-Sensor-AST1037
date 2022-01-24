// Written by Jason Marcum for TinyCircuits
// Initiated: July 2019


#ifndef TinyAnimation_h
#define TinyAnimation_h

// For Color definitions
#include "TinyScreen.h"


class PercentBarHorizontal{
public:
    PercentBarHorizontal(TinyScreen display_, int x_, int y_, int width_, int height_, uint8_t outline_color_, uint8_t bar_color_, uint8_t font_color_, float percent_, char *left_end_, char *right_end_, bool show_percentage_) 
    : display(display_)
    , x(x_)
    , y(y_)
    , width(width_)
    , height(height_)
    , percent(percent_)
    , left_end(left_end_)
    , right_end(right_end_)
    , show_percentage(show_percentage_)
    , outline_color(outline_color_)
    , bar_color(bar_color_)
    , font_color(font_color_)
    {}
    
    // Clear/overwrite the graphic with black
    void erase();

    // Sets the end bar text info
    void updateBarEndInfo();
    
    // Show the next frame if it is time, otherwise keep track of the tick/speed
    void tick(float percent_);
    
private:
    TinyScreen display;
    
    // Information about the graphic and how it behaves/looks
    int x;
    int y;
    int width;
    int height;
    float percent = 0.0;
    float last_percent = percent;
    char *left_end;
    char *right_end;
    bool show_percentage;
    uint8_t outline_color;
    uint8_t bar_color;
    uint8_t font_color;
};



class IdleCircle{
public:

    // Rotation speed is frame per rotation_slowness_ (ticks). If rotation_slowness_ = 4, then 1 frame every 4 ticks (frame/rotation_slowness_)
    IdleCircle(TinyScreen display_, int x_, int y_, uint8_t trail_color_, uint8_t head_color_, int radius_, int thickness_, int rotation_slowness_) 
    : display(display_)
    , x(x_)
    , y(y_)
    , radius(radius_)
    , thickness(thickness_)
    , rotation_slowness(rotation_slowness_)
    , trail_color(trail_color)
    , head_color(head_color_)
    {}
    
    // Clear/overwrite the graphic with black
    void erase();
    
    // Show the next frame if it is time, otherwise keep track of the tick/speed
    void tick();
    
private:
    TinyScreen display;
    
    // Information about the graphic and how it behaves/looks
    int x;
    int y;
    int radius;
    int thickness;
    int rotation_slowness;
    uint8_t trail_color;
    uint8_t head_color;

    // Internal stuff for the circle itself
    int tick_counter = 0;
    int tick_fallover_limit = rotation_slowness;
    float angle_step = 4;
    float angle_step_tracker = 0;
};



class FallDownSprite{
public:
    FallDownSprite(TinyScreen display_, int x_, int y_, int sprite_count_, int x_spacing_, int y_spacing_, int speed_, unsigned char *sprite_) 
    : display(display_)
    , x(x_)
    , y(y_)
    , sprite_count(sprite_count_)
    , x_spacing(x_spacing_)
    , y_spacing(y_spacing_)
    , speed(speed_)
    , sprite(sprite_)
    {}
    
    // Clear/overwrite the graphic with black
    void erase();
    
    // Show the next frame if it is time, otherwise keep track of the tick/speed
    // Returns true when it reaches the last frame, otherwise false 
    bool tick();
    
private:
    TinyScreen display;
    
    // Information about the graphic and how it behaves/looks
    int x;
    int y;
    int sprite_count;
    int x_spacing;
    int y_spacing;
    int speed;
    unsigned char *sprite;

    // Every 110 loops/ticks update the animation
    int animation_tick_size = speed;
    
    // Tracks how many ticks it has been
    int animation_tick_counter = 0;
    
    // Tracks current animation frame (changes every animation_tick_size)
    int animation_frame_counter = 0;

    int x_cur_pos = 0;
    int y_cur_pos = 0;
    
};

#endif
