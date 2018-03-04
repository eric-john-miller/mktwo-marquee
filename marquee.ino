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

const int MARQUEE_LEN = 64;
char marqueeText[MARQUEE_LEN] = "Hello World";
int  pixelPerChar = 6;
int  maxDisplacement;
int x = matrix.width();
int pass = 0;
int delayval = 500; // delay for half a second
const uint16_t colors[] = 
{
    matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255) 
};

unsigned int raw = 0;
float volt = 0.0;
#define BATT_CHECK_PIN 20

//setup wifi server
const char* ssid = "SDGOL";
const char* password = "ButterBeer9836";
int status = WL_IDLE_STATUS;
ESP8266WebServer server(80);


// prepare a web page to be send to a client (web browser)
void onRoot() 
{
    Serial.println("onRoot()");
    char temp[1000];
	int sec = millis() / 1000;
	int min = sec / 60;
	int hr = min / 60;

	snprintf ( temp, 1000,
        "<html>\
        <head>\
           <title>%s</title>\
            <style>\
            body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
            </style>\
        </head>\
        <body>\
            <p>Uptime: %02d:%02d:%02d</p>\
            <p><img src=\"/test.svg\" /></p>\
            <p><form action='/submit' method='POST'>\
                Marquee text: <input type=\"text\" name=\"marquee\" value=\"%s\"><br>\
                <input type=\"submit\" value=\"Submit\">\
            </form></p>\
        </body>\
        </html>",
		ssid, hr, min % 60, sec % 60, marqueeText
	);
    Serial.println(temp);
	server.send ( 200, "text/html", temp);
}

void onSubmit()
{
    Serial.println("onSubmit()");
    if (server.args() > 0 ) 
    {
        for ( uint8_t i = 0; i < server.args(); i++ ) 
        {
            Serial.printf("%s : %s\n", server.argName ( i ).c_str(), server.arg ( i ).c_str());
            if (server.argName(i) == "marquee") 
            {
                // do something here with value from server.arg(i);
                snprintf(marqueeText, MARQUEE_LEN, server.arg(i).c_str());
            }
        }
    }
    
}

void onNotFound() 
{
    Serial.println("onNotFound()");
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";

    for ( uint8_t i = 0; i < server.args(); i++ ) 
    {
        message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
    }

    server.send (404, "text/plain", message );
}


void drawGraph() 
{
     Serial.println("drawGraph()");
    String out = "";
    char temp[100];
    out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"400\" height=\"150\">\n";
    out += "<rect width=\"400\" height=\"150\" fill=\"rgb(250, 230, 210)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";
    out += "<g stroke=\"black\">\n";
    int y = rand() % 130;
    for (int x = 10; x < 390; x+= 10) 
    {
        int y2 = rand() % 130;
        sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"1\" />\n", x, 140 - y, x + 10, 140 - y2);
        out += temp;
        y = y2;
    }
    out += "</g>\n</svg>\n";

    server.send ( 200, "image/svg+xml", out);
}

void setup() 
{
    Serial.begin(115200);
    Serial.setDebugOutput(true);

    Serial.println("setup()");
    
    //prepare to read battery voltage 
    pinMode(A0, INPUT);

 	Serial.print("Configuring access point...");
	WiFi.softAP(ssid, password);

	IPAddress myIP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(myIP);
	server.on("/", onRoot);
    server.on ( "/test.svg", drawGraph );
	server.on ( "/inline", []() { server.send ( 200, "text/plain", "this works as well" ); } );
    server.on ( "/submit", onSubmit);
	server.onNotFound ( onNotFound );
	server.begin();
    Serial.printf("Web server started, open %s in a web browser\n", WiFi.softAPIP().toString().c_str());

    snprintf(marqueeText, MARQUEE_LEN, WiFi.softAPIP().toString().c_str());

    //prepare marquee
    matrix.begin();
    matrix.setTextWrap(false);
    matrix.setBrightness(30);
    matrix.setTextColor(matrix.Color(80,255,0));
    matrix.setFont(&TomThumb);
    //maxDisplacement = strlen(marqueeText) * pixelPerChar + matrix.width();
}

void displayMarquee()
{
    //Serial.println("displayMarquee()");
    //display marquee
    maxDisplacement = strlen(marqueeText) * pixelPerChar + matrix.width();
    matrix.fillScreen(0);
    matrix.setTextWrap( false );
    matrix.fillScreen(0);
    matrix.setCursor(x, NEO_HEIGHT);
    matrix.print(marqueeText);
    if (--x < -maxDisplacement)
    {
        x = matrix.width();
        if(++pass >= 3) pass = 0;
        matrix.setTextColor(colors[pass]);
    }
    matrix.show();
}

void loop()
{
    //NOTE: Here's where the code for the main program should start. 
    //Serial.println("Main program loop begins...");
    
    //web server 
    server.handleClient();

    displayMarquee();
    delay(100);

    
    //Serial.println("Main program loop ends...");
    
}

