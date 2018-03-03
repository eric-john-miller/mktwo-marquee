#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>
#include "WiFiManager.h"
#include <Adafruit_NeoPixel.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_GFX.h>  
#include "TomThumb.h"

#define LED_DATA 13
#define NUM_PIXELS 25
// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
#define NEO_PIN    13 // Arduino pin to NeoPixel data input
#define NEO_WIDTH  5 // Hat circumference in pixels
#define NEO_HEIGHT 5 // Number of pixel rows (round up if not equal)
#define NEO_OFFSET  (((NEO_WIDTH * NEO_HEIGHT) - 240) / 2)
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(25, LED_DATA, NEO_GRB + NEO_KHZ800);

// MATRIX DECLARATION:
// Parameter 1 = width of NeoPixel matrix
// Parameter 2 = height of matrix
// Parameter 3 = pin number (most are valid)
// Parameter 4 = matrix layout flags, add together as needed:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the matrix; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs are arranged in horizontal
//     rows or in vertical columns, respectively; pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns proceed
//     in the same order, or alternate lines reverse direction; pick one.
//   See example below for these values in action.
// Parameter 5 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic ‘v1’ (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)

// Example for NeoPixel Shield.  In this application we’d like to use it
// as a 5×8 tall matrix, with the USB port positioned at the top of the
// Arduino.  When held that way, the first pixel is at the top right, and
// lines are arranged in columns, progressive order.  The shield uses
// 800 KHz (v2) pixels that expect GRB color data.
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(NEO_WIDTH, NEO_HEIGHT, NEO_PIN,
NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
NEO_GRB            + NEO_KHZ800);

char yourText[64] = "Hello World";
int  pixelPerChar = 6;
int  maxDisplacement;
int x = matrix.width();
int pass = 0;
int delayval = 500; // delay for half a second
const uint16_t colors[] = 
{
    matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255) 
};


void setup() 
{
    Serial.begin(115200);
    Serial.println("setup()");
    //pixels.begin(); // This initializes the NeoPixel library.
    matrix.begin();
    matrix.setTextWrap(false);
    matrix.setBrightness(30);
    matrix.setTextColor(matrix.Color(80,255,0));
    matrix.setFont(&TomThumb);
    maxDisplacement = strlen(yourText) * pixelPerChar + matrix.width();
}

void loop()
{
    //NOTE: Here's where the code for the main program should start. 
    Serial.println("Main program loop begins...");
    
    matrix.fillScreen(0);
    matrix.setTextWrap( false );
    matrix.fillScreen(0);
    matrix.setCursor(x, NEO_HEIGHT);
    matrix.print(F("Hello World"));
    if (--x < -maxDisplacement)
    {
        x = matrix.width();
        if(++pass >= 3) pass = 0;
        matrix.setTextColor(colors[pass]);
    }
    matrix.show();
    delay(100);
  
    
    Serial.println("Main program loop ends...");
    
}
