#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "config.h"

#define STRIP_CONTROL D8
#define BUTTON_PIN D3

int lastBtnStatus = HIGH;

int leftColor[] = {0, 0, 255};
int centerColor[] = {0, 0, 255};
int rightColor[] = {0, 0, 255};

float leftPercent = 1;
float centerPercent = 1;
float rightPercent = 1;

float lastCheck = -100;
float stepStartTime = 0;
int step = 1;


Adafruit_NeoPixel strip(stripLeftLEDCount + stripCenterLEDCount + stripRightLEDCount, STRIP_CONTROL, NEO_GRB + NEO_KHZ800);
HTTPClient sender;
WiFiClientSecure wifiClient;


void paint()
{
  //Serial.println("Painting: Left: " + String(leftPercent) + " /  Center: " + String(centerPercent) + " / Right: " + String(rightPercent));


  // start by switching all off
  strip.clear();

  // Set left side (note, that this needs to be inverted)
  for (int l = 0.0; l < stripLeftLEDCount; l += 1)
  {
    if (l * 1.0 / stripLeftLEDCount * 1.0 <= leftPercent)
    {
      strip.setPixelColor(stripLeftLEDCount - l - 1, strip.Color(leftColor[0], leftColor[1], leftColor[2]));
    }
  }
  
  // Set center
  for (int c = 0.0; c < stripCenterLEDCount; c += 1)
  {
    if (c * 1.0 / stripCenterLEDCount * 1.0 <= centerPercent)
    {
      strip.setPixelColor(stripLeftLEDCount + c, strip.Color(centerColor[0], centerColor[1], centerColor[2]));
    }
  }
  
  // Set right
  for (int r = 0.0; r < stripRightLEDCount; r += 1)
  {
    if (r * 1.0 / stripRightLEDCount * 1.0 <= rightPercent)
    {
      strip.setPixelColor(stripLeftLEDCount + stripCenterLEDCount + r, strip.Color(rightColor[0], rightColor[1], rightColor[2]));
    }
  }



  strip.show();
}


void paintFromCenter(int numPixels, int centerCol[], int borderCol[])
{
  strip.clear();

  int centerPos = (stripLeftLEDCount + stripCenterLEDCount + stripRightLEDCount) / 2;
  Serial.println("Setting pixel " + String(numPixels));

  for (int px = 0; px < numPixels; px++)
  {
    if (centerPos + px < stripLeftLEDCount + stripCenterLEDCount + stripRightLEDCount)
    {
      int mixColorR = int(((px * 1.0 / numPixels) * centerCol[0]) + ((1.0 - (px * 1.0 / numPixels)) * borderCol[0]));
      int mixColorG = int(((px * 1.0 / numPixels) * centerCol[1]) + ((1.0 - (px * 1.0 / numPixels)) * borderCol[1]));
      int mixColorB = int(((px * 1.0 / numPixels) * centerCol[2]) + ((1.0 - (px * 1.0 / numPixels)) * borderCol[2]));

      strip.setPixelColor(centerPos + px, strip.Color(mixColorR, mixColorG, mixColorB));
      strip.setPixelColor(centerPos - px, strip.Color(mixColorR, mixColorG, mixColorB));
    }
  }

  strip.show();

}




void wifiConnect()
{
  WiFi.begin(ssid, password);
  int retrys = 0;
  int red[] = {255, 0, 0};
  int green[] = {0, 255, 0};

  paintFromCenter(1, red, red);

  
  Serial.print("Connecting to WiFi " + String(ssid));

  while (WiFi.status() != WL_CONNECTED && retrys < (stripLeftLEDCount + stripCenterLEDCount + stripRightLEDCount) / 2)
  {
    Serial.print(".");
    
    paintFromCenter(retrys, red, red);
    retrys++;

    delay(1000);
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("WiFi connected!");

    wifiClient.setInsecure();

    while (retrys < (stripLeftLEDCount + stripCenterLEDCount + stripRightLEDCount) / 2)
    {
      paintFromCenter(retrys, green, green);
      retrys++;
      delay(20);
    }

    step = 1;
  } else
  {
    Serial.println("WiFi connection failed!");
    
    while (retrys < (stripLeftLEDCount + stripCenterLEDCount + stripRightLEDCount) / 2)
    {
      paintFromCenter(retrys, red, red);
      retrys++;
      delay(20);
    }

    step = 2;
  }

  Serial.println("Entering main loop");
  
}


float getDistancePercent()
{
  if (sender.begin(wifiClient, url))
  {
    int httpCode = sender.GET();
    if (httpCode > 0)
    {
      if (httpCode == HTTP_CODE_OK)
      {
        String payload = sender.getString();
        Serial.println("Got distance: " + payload + "km");
        float dst = payload.toFloat();

        if (dst > largestDistance)
        {
          return 0;
        } else
        {
          if (dst > 0.5)
          {
            return 1.0 - (dst / largestDistance);
          } else
          {
            return 1;
          }
        } 
      }
    } else
    {
      Serial.printf("HTTP-Error: ", sender.errorToString(httpCode).c_str());
    }

    sender.end();
  } else
  {
    Serial.printf("Failed to connect!");
  }

  return 0;
}



void setup()
{
  pinMode(BUTTON_PIN, INPUT);
  Serial.begin(9600);

  strip.begin();
  strip.show();
  strip.setBrightness(200);

  wifiConnect();

  
  centerColor[0] = 0;
  centerColor[1] = 255;
  centerColor[2] = 255;

}


void loop()
{
  centerPercent =  step / (totalSteps * 1.0);

  if (step == 1)
  {
    if (millis() - lastCheck > 2000)
    {
      float dst = getDistancePercent();
      leftPercent = dst;
      rightPercent = dst;

      leftColor[0] = int(255 * (1 - dst));
      leftColor[1] = int(255 * dst);
      leftColor[2] = 0;

      rightColor[0] = int(255 * (1 - dst));
      rightColor[1] = int(255 * dst);
      rightColor[2] = 0;

      lastCheck = millis();

      paint();

    }

  } else
  {
    if (millis() - lastCheck > 2000)
    {
      float dur = stepDurationSec[step - 1] * 1000.0;
      float passed = millis() - stepStartTime;

      float dst = passed / dur;
      if (dst > 1)
      {
        dst = 1;
      }
      leftPercent = dst;
      rightPercent = dst;

      leftColor[0] = int(255 * dst);
      leftColor[1] = int(255 * (1 - dst));
      leftColor[2] = 0;

      rightColor[0] = int(255 * dst);
      rightColor[1] = int(255 * (1 - dst));
      rightColor[2] = 0;

      lastCheck = millis();

      paint();

    }
  }
  



  int newStatus = digitalRead(BUTTON_PIN);

  if (lastBtnStatus != newStatus)
  {
    if (newStatus == LOW)
    {
      Serial.println("BUTTON ON");
      rainbow(4);
      
      if (step < totalSteps)
      {
        stepStartTime = millis();
        step++;
      } else
      {
        if (WiFi.status() == WL_CONNECTED)
        {
          step = 1;
        } else 
        {
          step = 1;
        }
        
      }
    } else
    {
      Serial.println("BUTTON OFF");
    }

    lastBtnStatus = newStatus;
  }

  delay(100);
}


void rainbow(int wait)
{
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256)
  {
    strip.rainbow(firstPixelHue);
    strip.show();
    delay(wait);
  }
}

void colorWipe(uint32_t color, int wait)
{
  for(int i=0; i<strip.numPixels(); i++)
  {
    strip.setPixelColor(i, color);
    strip.show();
    delay(wait);
  }
}
