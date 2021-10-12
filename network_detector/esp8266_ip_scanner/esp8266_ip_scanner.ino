//wifi finder v 1.0
#include <Adafruit_GFX.h>       // include Adafruit graphics library
#include <Adafruit_ILI9341.h>   // include Adafruit ILI9341 TFT library
#include <Pinger.h>
#include <ESP8266WiFi.h>

extern "C"
{
  #include <lwip/icmp.h> // needed for icmp packet definitions
}

// Set global to avoid object removing after setup() routine
Pinger pinger;

#include "ESP8266WiFi.h"

#define TFT_CS    D2     // TFT CS  pin is connected to NodeMCU pin D2
#define TFT_RST   A0     // TFT RST pin is connected to NodeMCU pin D3
#define TFT_DC    D1     // TFT DC  pin is connected to NodeMCU pin D4
#define CENTER_PIN    D0
#define UP_PIN    D3
// initialize ILI9341 TFT library with hardware SPI module
// SCK (CLK) ---> NodeMCU pin D5 (GPIO14)
// MOSI(DIN) ---> NodeMCU pin D7 (GPIO13)
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

int mode = 0;
int UP;
int CENTER;

bool found = false;

IPAddress ip;

void setup()
{  
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(2);  //even = portrait, odd = landscape
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_GREEN);

  
  // Connect to WiFi access point
  bool stationConnected = WiFi.begin(
  "XXXXXXXX",
  "XXXXXXXX");

  // Check if connection errors
  if(!stationConnected)
  {
    Serial.println("Error, unable to connect specified WiFi network.");
  }
  
  // Wait connection completed
  Serial.print("Connecting to AP...");
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.print("Ok\n");
  
  pinger.OnReceive([](const PingerResponse& response)
  {
    
    if (response.ReceivedResponse)
    {
      Serial.printf(
        "Reply from %s: bytes=%d time=%lums TTL=%d\n",
        response.DestIPAddress.toString().c_str(),
        response.EchoMessageSize - sizeof(struct icmp_echo_hdr),
        response.ResponseTime,
        response.TimeToLive);
      //tft.println("...found");
      
      found = true;
    }
    else
    {
      Serial.printf("Request timed out.\n");
      //tft.println("...not found");
      found = false;
    }

    // return true to continue the ping sequence.
    // if current event returns false, the ping sequence is interrupted.
    return false;
  });
  
  pinger.OnEnd([](const PingerResponse& response)
  {
    tft.setTextSize(1);

    // Evaluate lost packet percentage
    float loss = 100;
    
    if(response.TotalReceivedResponses > 0)
    {
      loss = (response.TotalSentRequests - response.TotalReceivedResponses) * 100 / response.TotalSentRequests;
    }


    tft.setTextSize(2);
    return true;
  });
}

void loop() {
  ip = IPAddress(192, 168, 1, 0);

  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0,0);

  for (int i = 0; i < 20; i++) {
    found = false;
    tft.print("pinging ");
    tft.print(i);
    ip = IPAddress(192,168,1,i);
    pinger.Ping(ip);
    delay(3000);
    if (found == true) {
      tft.println("...detected");
    }
    else {
      tft.println("....... ");
    }
   
  }
  delay(10000);
}

