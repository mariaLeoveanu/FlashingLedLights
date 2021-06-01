#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include "arduinoFFT.h"

#define PIN 13 
#define N_LEDS 15  
#define RX 10
#define TX 11
#define MUSIC_MODE 1
#define STATIC_MODE 2
#define V_METER_MODE 3
#define SPECTRUM_MODE 4

#define FADE_MODE 5
     
Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);
SoftwareSerial BTserial(RX, TX); 

int RED[3] = {255, 0, 0};
int ORANGE[3] = {255,100,0};
int YELLOW[3] = {255,255,0};
int GREEN[3] = {0,128,0};
int AQUA[3] = {0, 150, 255};
int DARK_BLUE[3] = {0, 50, 255};
int PINK[3] = {255, 10,100};
int MOV[3] = {150, 0, 255};
int WHITE[3] = {255, 255, 255};

int set_colour;
float sensorValue = 0;
float localMax = 0;
int val = 0;
int mode = 0;
// level tresholds
int high = 19;
int medium = 17;
int low = 15;
// delay values 
int v_meter_delay = 11;
int spectrum_delay = 30;

void setup() {
  strip.begin();
  Serial.begin(9600); 
  BTserial.begin(9600);
  pinMode(RX, INPUT);
  pinMode(TX, OUTPUT);
  pinMode(A2, INPUT);
  mode = MUSIC_MODE;
}

void music(){
  sensorValue = (float)analogRead(A2);
  Serial.println(sensorValue);
  if(sensorValue > localMax){
    localMax = sensorValue;
  } else { 
    localMax = sensorValue;
  }
  ChooseColour();
}

void ChooseColour(){
  if(localMax >= high){
     set(strip.Color(DARK_BLUE[0], DARK_BLUE[1], DARK_BLUE[2]));
  }  else if(localMax >= medium){
     set(strip.Color(GREEN[0], GREEN[1], GREEN[2]));
  } else if(localMax >= low){
     set(strip.Color(YELLOW[0], YELLOW[1], YELLOW[2]));
  }
}

void static_colour(int colour[]){
  set(strip.Color(colour[0], colour[1], colour[2])); 
}

void v_meter(){
  sensorValue = (float)analogRead(A2);
  if(sensorValue > localMax){
    localMax = sensorValue;
  } else { 
    localMax = sensorValue;
  }
  chase_meter();
  
}

void chase_meter(){
  int level = 0;
  int level_1 = low;
  int level_2 = low + 1;
  int level_3 = medium;
  int level_4 = medium + 1;
  int level_5 = high;
  
  if(localMax >= level_5){
     level = 15;
  } else if(localMax >= level_3){
     level = 10;
  } else if(localMax >= level_1){
     level = 4;
  } 
 
  for(int i=0; i<level; i++) {
    if(i < 10){
      strip.setPixelColor(i, strip.Color(GREEN[0], GREEN[1], GREEN[2])); // green
    } else if (i <= 12){
      strip.setPixelColor(i, strip.Color(ORANGE[0], ORANGE[1], ORANGE[2])); // orange
    } else {
      strip.setPixelColor(i, strip.Color(RED[0], RED[1], RED[2])); // red
    }
    
    strip.show();
    delay(v_meter_delay);
  }
  for(int i=level-1; i>=0; i--) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
    strip.show();
    delay(v_meter_delay);
  }
}

void rainbow(){
  for(int j = 0; j < 256; j++){
     for(int i = 0; i < N_LEDS; i++){
      strip.setPixelColor(i, Wheel((i + j) & 255));
     } 
     strip.show(); 
     delay(spectrum_delay);
  }
}

// https://codebender.cc/sketch:80438#Neopixel%20Rainbow.ino
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } 
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void fade(){
  for(int i = 0; i <= 255; i++){
    for(int j = 0; j < N_LEDS; j++){
      strip.setPixelColor(j, strip.Color(0, i, 255-i));
      strip.show();
    }
  }
  for(int i = 0; i <= 255; i++){
    for(int j = 0; j < N_LEDS; j++){
      strip.setPixelColor(j, strip.Color(0, 255-i, i));
      strip.show();
    }
  }
}


void loop(){
  if(mode == MUSIC_MODE){
    music();
  } else if(mode == STATIC_MODE){
    static_colour(set_colour); 
 } else if(mode == V_METER_MODE){
    v_meter();
 } else if(mode == SPECTRUM_MODE){
    rainbow();
 } else if(mode == FADE_MODE){
    fade();
 }
 
 if(BTserial.available() > 0){ 
  mode = BTserial.parseInt();
  if(mode == 2){
    String colour;
    while(BTserial.available()>0){
      char letter = BTserial.read();
      if(letter == 'R'){
        set_colour = RED;
      } else if(letter == 'O'){
        set_colour = ORANGE; 
      }else if(letter == 'Y'){
        set_colour = YELLOW; 
      }else if(letter == 'G'){
        set_colour = GREEN; 
      } else if(letter ==  'A'){
        set_colour = AQUA;
      } else if(letter == 'D'){
        set_colour = DARK_BLUE;
      } else if(letter == 'P'){
        set_colour = PINK;
      } else if(letter == 'M'){
        set_colour = MOV;
      } else {
        set_colour = WHITE;
      }
    }
  } else if(mode == 3){
    set(strip.Color(0,0,0));
  }
 }
}


static void set(uint32_t c) {
  for(uint16_t i=0; i<N_LEDS; i++) {
    strip.setPixelColor(i , c);
  }
  strip.show();
}
