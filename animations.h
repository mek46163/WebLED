// used by meteorrain
void fadeToBlack(int ledNo, byte fadeValue) {
 #ifdef ADAFRUIT_NEOPIXEL_H 
    // NeoPixel
    uint32_t oldColor;
    uint8_t r, g, b;
    int value;
    
    oldColor = strip.getPixelColor(ledNo);
    r = (oldColor & 0x00ff0000UL) >> 16;
    g = (oldColor & 0x0000ff00UL) >> 8;
    b = (oldColor & 0x000000ffUL);

    r=(r<=10)? 0 : (int) r-(r*fadeValue/256);
    g=(g<=10)? 0 : (int) g-(g*fadeValue/256);
    b=(b<=10)? 0 : (int) b-(b*fadeValue/256);
    
    strip.setPixelColor(ledNo, r,g,b);
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   leds[ledNo].fadeToBlackBy( fadeValue );
 #endif  
}


void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) {  
  while(true){
      setAll(0,0,0);
      for(int i = 0; i<5; i++){
          switch(i){
            case 0: {
              red = 0xff;
              green = 0x00;
              blue = 0x00;
              break;
            }
            case 1: {
              red = 0x00;
              green = 0xff;
              blue = 0x00;
              break;
            }
            case 2: {
              red = 0x00;
              green = 0x00;
              blue = 0xff;
              break;
            }
            case 3: {
              red = 0xff;
              green = 0xff;
              blue = 0xff;
              break;
            }
            case 4: {
              red = 0xff;
              green = 0x00;
              blue = 0x35;
              break;
            }
          }
        
        for(int i = 0; i < NUM_LEDS+NUM_LEDS; i++) {
          
          
          // fade brightness all LEDs one step
          for(int j=0; j<NUM_LEDS; j++) {
            if( (!meteorRandomDecay) || (random(10)>5) ) {
              fadeToBlack(j, meteorTrailDecay );        
            }
          }
          
          // draw meteor
          for(int j = 0; j < meteorSize; j++) {
            if( ( i-j <NUM_LEDS) && (i-j>=0) ) {
              setPixel(i-j, red, green, blue);
            } 
          }
          if(checkIR()){
             return;
          }
          showStrip();
          delay(SpeedDelay);
        }
      }
  }
}

void colorWipe() {
  while(true){
      for(int i = 0; i<3; i++){
        switch(i){
          case 0: {
                    for(uint16_t i=0; i<NUM_LEDS; i++) {
                      setPixel(i, 0x00, 0xff, 0x00);
                      showStrip();
                      if(checkIR()){
                        return;
                      }
                      delay(10);
                    }
          }
          case 1: {
                    for(uint16_t i=0; i<NUM_LEDS; i++) {
                      setPixel(i, 0x00, 0x00, 0xff);
                      showStrip();
                      if(checkIR()){
                        return;
                      }
                      delay(10);
                    }             
          }
          case 2: {
                    for(uint16_t i=0; i<NUM_LEDS; i++) {
                      setPixel(i, 0xff, 0x00, 0x00);
                      showStrip();
                      if(checkIR()){
                        return;
                      }
                      delay(10);
                    }            
          }
        }
      }    
  }


}




void nextAnimation(){
  selected_animation++;
  if(selected_animation>5){
    selected_animation = 1;
  }
  Serial.println("Animation:");
  Serial.println(selected_animation);
  delay(100);
  switch(selected_animation){
    case 1: {
      RGBLoop();
      break;
    }
    case 2: {
      FadePurple();
      break;
    }
    case 3: {
      rainbowCycle(5);
      break;
    }
    case 4: {
      // meteorRain - Color (red, green, blue), meteor size, trail decay, random trail decay (true/false), speed delay 
      meteorRain(0xff,0xff,0xff,10, 15, true, 10);
      break;
    }
   case 5: {
      // colorWipe - Color (red, green, blue), speed delay
        colorWipe();


      break;
   }
  }
}
