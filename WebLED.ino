#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>



#include <String.h>
#ifndef STASSID
#define STASSID "WIFISSID"
#define STAPSK  "PASSWORD"
#endif

#include "index.h";
#include "FastLED.h"
#define NUM_LEDS 270 
CRGB leds[NUM_LEDS];
#define PIN 4

#define NUM_CODES 10

const uint16_t kRecvPin = 0;
 

void setAll(byte red, byte green, byte blue);
void rainbowCycle(int);
void showStrip();
void loop(void);
byte * Wheel(byte WheelPos);
void FadeInOut(byte red, byte green, byte blue, byte fadeTime);
void setPixel(int Pixel, byte red, byte green, byte blue);
void FadeRed();
void RGBLoop();
void FadePurple();
bool checkIR();


IRrecv irrecv(kRecvPin);

decode_results results;

bool check;
const char* ssid = STASSID;
const char* password = STAPSK;
long code[NUM_CODES];
bool set_code[NUM_CODES];
long effect[NUM_CODES][3];

bool set_next_animation;
int code_next_animation;
int selected_animation;
#include "animations.h";
ESP8266WebServer server(80);

const int led = 13;

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from esp8266!");
  digitalWrite(led, 0);
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {
  FastLED.addLeds<WS2811, PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  irrecv.enableIRIn();  // Start the receiver
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  setAll(255,90,40);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  //JSON
  const size_t bufferSize = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(8) + 370;



  //code fuer next animation vordefinieren
  code_next_animation = 16762935;
  // codes fuer an und aus vordefinieren
  code[8] = 16742535; //aus
  code[9] = 16714485; //weiss
  effect[8][0] = 0;
  effect[8][1] = 0;
  effect[8][2] = 0;

  effect[9][0] = 255;
  effect[9][1] = 70;
  effect[9][2] = 25;
  
  server.on("/test", handleRoot);
  server.on("/color", [](){
    DynamicJsonDocument color(bufferSize);
    deserializeJson(color, server.arg("plain"));
    int red = color["red"].as<int>();
    int green = color["green"].as<int>();
    int blue = color["blue"].as<int>();
    Serial.println(color["red"].as<int>());
    Serial.println(color["green"].as<int>());
    Serial.println(color["blue"].as<int>());
    setAll(red, green, blue);
  });
  server.on("/fade", [](){
    DynamicJsonDocument color(bufferSize);
    deserializeJson(color, server.arg("plain"));
    int red = color["red"].as<int>();
    int green = color["green"].as<int>();
    int blue = color["blue"].as<int>();
    int fadeTime = color["fadeTime"].as<int>();
    Serial.println("Rot:");
    Serial.println(color["red"].as<int>());
    Serial.println("Gruen:");
    Serial.println(color["green"].as<int>());
    Serial.println("Blau:");
    Serial.println(color["blue"].as<int>());
    Serial.println("Verzoegerung:");
    Serial.println(color["fadeTime"].as<int>());
    Serial.println(server.arg("plain"));
    FadeInOut(red, green, blue, fadeTime);
  });
  server.on("/red", []() {
    //String test = showPage();
    server.send(200, "text/html", PAGE);
    setAll(255,0,0);
  });
  server.on("/green", []() {
    //String test = showPage();
    server.send(200, "text/html", PAGE);
    setAll(0,255,0);
  });
  server.on("/blue", []() {
    //String test = showPage();
    server.send(200, "text/html", PAGE);
    setAll(0,0,255);
  });

  server.on("/rainbow", [](){
   // String test = showPage();
    server.send(200, "text/html", PAGE);

        rainbowCycle(5);  
    

    
  });

  server.on("/white", []() {
   // String test = showPage();
    server.send(200, "text/html", PAGE);
    setAll(255,90,40);
  });
  
  server.on("/", []() {
  // String test = showPage();
  server.send(200, "text/html", PAGE);
  setAll(255,90,40);
});

   server.on("/fadered", []() {
    //String test = showPage();
    server.send(200, "text/html", PAGE);

      FadeRed();
    
    
   });

    server.on("/setCode", []() {
      DynamicJsonDocument color(bufferSize);
      deserializeJson(color, server.arg("plain"));
      int red = color["red"].as<int>();
      int green = color["green"].as<int>();
      int blue = color["blue"].as<int>();
      int i = color["effect"].as<int>();
      set_code[i] = true;
      effect[i][0] = red;
      effect[i][1] = green;
      effect[i][2] = blue;
      Serial.println(color["red"].as<int>());
      Serial.println(color["green"].as<int>());
      Serial.println(color["blue"].as<int>());
      Serial.println(color["effect"].as<int>());
      Serial.println(server.arg("plain")); 
    });

   server.on("/setNextAnimation", [] (){
    Serial.println("Set Code for next Animation");
    set_next_animation = true;
   });
    
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");


   
   server.on("/rgbloop", []() {
    //String test = showPage();
    server.send(200, "text/html", PAGE);
 
       RGBLoop();  


   });

   server.on("/fadepurple", []() {
    //String test = showPage();
    server.send(200, "text/html", PAGE);
      Serial.println("FadePurple");
  
         FadePurple();  

      
  });
}


void loop(void) {
  server.handleClient();
  MDNS.update();
  checkIR();
//   if (irrecv.decode(&results)) {
//    // print() & println() can't handle printing long longs. (uint64_t)
//    serialPrintUint64(results.value, HEX);
//    Serial.println("");
//    for(int i = 0; i<NUM_CODES; i++){
//        if(set_code[i]){
//          code[i] = results.value;
//          set_code[i] = false;
//        }
//    }
//    for(int i = 0; i<NUM_CODES; i++){
//      if(code[i] == results.value){
//        Serial.println(effect[i][0]);
//        Serial.println(effect[i][1]);
//        Serial.println(effect[i][2]);
//        
//         setAll(effect[i][0], effect[i][1], effect[i][2]);
//      }
//    }
//    
//    irrecv.resume();  // Receive the next value
//    delay(100);
//  }
}

bool checkIR(){
    check = false;
     if (irrecv.decode(&results)) {
      check = true;
    // print() & println() can't handle printing long longs. (uint64_t)
    serialPrintUint64(results.value, HEX);
    Serial.println("");
    for(int i = 0; i<NUM_CODES; i++){
        if(set_code[i]){
          code[i] = results.value;
          set_code[i] = false;
        }

    }
    if(set_next_animation){
        code_next_animation = results.value;
        set_next_animation = false;
    }
    for(int i = 0; i<NUM_CODES; i++){
      if(code[i] == results.value){
        Serial.println(effect[i][0]);
        Serial.println(effect[i][1]);
        Serial.println(effect[i][2]);
        
         setAll(effect[i][0], effect[i][1], effect[i][2]);
         check = true;
      }

    }
    if(code_next_animation == results.value){
        Serial.println("Naechste Animation gewaehlt");
        
        delay(500);
        irrecv.resume();
        nextAnimation();
        
      }
    irrecv.resume();  // Receive the next value
    //delay(100);
    return check;
  }
}

void rainbowCycle(int SpeedDelay) {
  while(true){
    byte *c;
    uint16_t i, j;
  
    for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
      for(i=0; i< NUM_LEDS; i++) {
        c=Wheel(((i * 256 / NUM_LEDS) + j) & 255);
        setPixel(i, *c, *(c+1), *(c+2));
      }
      if(checkIR()){
        return;
      }
      showStrip();
      delay(SpeedDelay);
    }    
  }

}

// used by rainbowCycle and theaterChaseRainbow
byte * Wheel(byte WheelPos) {
  static byte c[3];
  
  if(WheelPos < 85) {
   c[0]=WheelPos * 3;
   c[1]=255 - WheelPos * 3;
   c[2]=0;
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   c[0]=255 - WheelPos * 3;
   c[1]=0;
   c[2]=WheelPos * 3;
  } else {
   WheelPos -= 170;
   c[0]=0;
   c[1]=WheelPos * 3;
   c[2]=255 - WheelPos * 3;
  }

  return c;
}


void FadeInOut(byte red, byte green, byte blue, byte fadeTime){
  float r, g, b;
  while(true){
    for(int k = 0; k < 256; k=k+1) { 
      r = (k/256.0)*red;
      g = (k/256.0)*green;
      b = (k/256.0)*blue;
      setAll(r,g,b);
      showStrip();
      Serial.println(r);
      Serial.println(g);
      Serial.println(b);
      if(checkIR()){
        return;
      }
      delay(fadeTime);
    }
       
    for(int k = 255; k >= 0; k=k-2) {
      r = (k/256.0)*red;
      g = (k/256.0)*green;
      b = (k/256.0)*blue;
      setAll(r,g,b);
      showStrip();
      Serial.println(r);
      Serial.println(g);
      Serial.println(b);
      if(checkIR()){
        return;
      }
      delay(fadeTime);
    }
    Serial.println(fadeTime);        
  }

}

void RGBLoop(){
  while(true){
      for(int j = 0; j < 3; j++ ) { 
      // Fade IN
      for(int k = 0; k < 256; k++) { 
        switch(j) { 
          case 0: setAll(k,0,0); break;
          case 1: setAll(0,k,0); break;
          case 2: setAll(0,0,k); break;
        }
        showStrip();
        if(checkIR()){
          return;
        }
        delay(3);
      }
      // Fade OUT
      for(int k = 255; k >= 0; k--) { 
        switch(j) { 
          case 0: setAll(k,0,0); break;
          case 1: setAll(0,k,0); break;
          case 2: setAll(0,0,k); break;
        }
  
        showStrip();
        if(checkIR()){
          return;
        }
        delay(3);
      }
    }
  }

}

void FadeRed(){
  while(true){
    // Fade IN
    for(int k = 0; k < 256; k++) { 
      
       setAll(k,0,0);

      
      showStrip();
      if(checkIR()){
        return;
      }
      delay(3);
    }
    // Fade OUT
    for(int k = 255; k >= 0; k--) { 

      setAll(k,0,0);
      
      showStrip();
      if(checkIR()){
        return;
      }
      delay(3);
    }    
  }

  
}

void FadePurple(){
  while(true){
    // Fade IN
    for(int k = 30; k < 256; k++) { 
      
       setAll(k,0,k);

      
      showStrip();
      if(checkIR()){
        return;
      }
      delay(10);
    }
    // Fade OUT
    for(int k = 255; k >= 30; k--) { 

      setAll(k,0,k);
      
      showStrip();
      if(checkIR()){
        return;
      }
      delay(10);
    }
       
  }

}


// Apply LED color changes
void showStrip() {
 #ifdef ADAFRUIT_NEOPIXEL_H 
   // NeoPixel
   strip.show();
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   FastLED.show();
 #endif
}

// Set a LED color (not yet visible)
void setPixel(int Pixel, byte red, byte green, byte blue) {
 #ifdef ADAFRUIT_NEOPIXEL_H 
   // NeoPixel
   strip.setPixelColor(Pixel, strip.Color(red, green, blue));
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H 
   // FastLED
   leds[Pixel].r = red;
   leds[Pixel].g = green;
   leds[Pixel].b = blue;
 #endif
}

// Set all LEDs to a given color and apply it (visible)
void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, blue, green); 
  }
  showStrip();
}
