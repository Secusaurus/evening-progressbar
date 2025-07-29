#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "config.h"

/* Control pin for strip */
#define STRIP_CONTROL D8
/* Pin for listening for the button */
#define BUTTON_PIN D3

/* Recent captured button pin status */
int lastBtnStatus = HIGH;

/* Color for the left progress bar */
int leftColor[] = {0, 0, 255};
/* Color for the center progress bar */
int centerColor[] = {0, 0, 255};
/* Color for the right progress bar */
int rightColor[] = {0, 0, 255};

/* Percentage for the left progress bar */
float leftPercent = 1;
/* Percentage for the center progress bar */
float centerPercent = 1;
/* Percentage for the right progress bar */
float rightPercent = 1;

/* Most recent percentage for distance */
float currentDistancePercent = 0.0;

/* Last check time (to prevent checking on every few milliseconds) */
float lastCheck = -100;
/* Time the current step was started */
float stepStartTime = 0;
/* Current step number */
int step = 1;

/* Count of all the LEDs of this strip */
const int LED_COUNT = stripLeftLEDCount + stripCenterLEDCount + stripRightLEDCount;

/* Alternating bool for making bee boop */
bool beeBoop = false;

/* Main strip */
Adafruit_NeoPixel strip(LED_COUNT, STRIP_CONTROL, NEO_GRB + NEO_KHZ800);
/* HTTP sender */
HTTPClient sender;
/* WiFi connector */
WiFiClientSecure wifiClient;

/* Red color */
const int red[] = { 255, 0, 0 };
/* Yellow color */
const int yellow[] = { 255, 255, 0 };
/* Green color */
const int green[] = { 0, 255, 0 };
/* Cyan color */
const int cyan[] = { 0, 255, 255 };
/* Blue color */
const int blue[] = { 0, 0, 255 };
/* Pink color */
const int pink[] = { 255, 0, 255 };
/* white color */
const int white[] = { 255, 255, 255 };
/* Black color */
const int black[] = { 0, 0, 0 };


/**
* Prepares painting the right and left progress bars as defined in percentage and color
* Note that you might want to clear the unpainted fields first and must call strip.show() after painting
*/
void preparePaintSides()
{
  // Set left side (note, that this needs to be inverted)
  for (int l = 0.0; l < stripLeftLEDCount; l += 1)
  {
    if (l * 1.0 / stripLeftLEDCount * 1.0 <= leftPercent)
    {
      strip.setPixelColor(stripLeftLEDCount - l - 1, strip.Color(leftColor[0], leftColor[1], leftColor[2]));
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
}


/**
* Prepares painting the right and left progress bars in a bee bop alarm
* Note that you might must call strip.show() after painting
*/
void preparePaintSidesBeeBoop()
{
  // Set left side (note, that this needs to be inverted)
  for (int l = 0.0; l < stripLeftLEDCount; l += 1)
  {
    if (l % 2 == 0)
    {
      if (beeBoop)
      {
        strip.setPixelColor(stripLeftLEDCount - l - 1, strip.Color(red[0], red[1], red[2]));
      } else
      {
        strip.setPixelColor(stripLeftLEDCount - l - 1, strip.Color(black[0], black[1], black[2]));
      }
    } else
    {
      if (!beeBoop)
      {
        strip.setPixelColor(stripLeftLEDCount - l - 1, strip.Color(red[0], red[1], red[2]));
      } else
      {
        strip.setPixelColor(stripLeftLEDCount - l - 1, strip.Color(black[0], black[1], black[2]));
      }
    }
  }
  
  // Set right
  for (int r = 0.0; r < stripRightLEDCount; r += 1)
  {
    
    if (r % 2 == 0)
    {
      if (beeBoop)
      {
        strip.setPixelColor(stripLeftLEDCount + stripCenterLEDCount + r, strip.Color(red[0], red[1], red[2]));
      } else
      {
        strip.setPixelColor(stripLeftLEDCount + stripCenterLEDCount + r, strip.Color(black[0], black[1], red[2]));
      }
    } else
    {
      if (!beeBoop)
      {
        strip.setPixelColor(stripLeftLEDCount + stripCenterLEDCount + r, strip.Color(red[0], red[1], red[2]));
      } else
      {
        strip.setPixelColor(stripLeftLEDCount + stripCenterLEDCount + r, strip.Color(black[0], black[1], red[2]));
      }
    }
  }
}

/**
* Prepares painting the center progress bar as defined in percentage and color
* Note that you might want to clear the unpainted fields first and must call strip.show() after painting
*/
void preparePaintCenter()
{ 
  // Set center
  for (int c = 0.0; c < stripCenterLEDCount; c += 1)
  {
    if (c * 1.0 / stripCenterLEDCount * 1.0 <= centerPercent)
    {
      strip.setPixelColor(stripLeftLEDCount + c, strip.Color(centerColor[0], centerColor[1], centerColor[2]));
    }
  }

}

/**
* Paints the progress bars as defined per single progress percentages and colors
*/
void paint()
{
  // start by switching all off
  strip.clear();

  // paint elements
  preparePaintSides();
  preparePaintCenter();

  // Show
  strip.show();
}


/**
* Paints the progress bars in a bee boop alarm style
*/
void paintBeeBoop()
{
  // start by switching all off
  strip.clear();

  // paint elements
  preparePaintSidesBeeBoop();
  preparePaintCenter();

  // Show
  strip.show();
}

/**
* Paint everything from the center
*/
void paintFromCenter(int numPixels, int centerCol[], int borderCol[])
{
  strip.clear();

  int centerPos = (LED_COUNT) / 2;

  for (int px = 0; px < numPixels; px++)
  {
    if (centerPos + px < LED_COUNT)
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

/**
* Generates a dynamic color and applies to center bar
*/
void colorizeCenter()
{
  uint32_t col = strip.ColorHSV((millis() / 3) % 65535, 255, 255);

  centerColor[0] = (col >> 16) & 0xFF;
  centerColor[1] = (col >> 8) & 0xFF;
  centerColor[2] = col & 0xFF;
}


/*
* Shows a neat reainbow animation
*/
void rainbow(int wait)
{
  int comingInPx = LED_COUNT - 1;
  int comingOutPx = LED_COUNT - 1;
  int maxVal = 2 * 256;


  for(long firstPixelHue = 0; firstPixelHue < maxVal; firstPixelHue++)
  {
    strip.rainbow(firstPixelHue * 256);
    
    if (comingInPx > 0)
    {
      for (int px = 0; px < LED_COUNT; px++)
      {
        if (px < comingInPx)
        {
          strip.setPixelColor(px, strip.Color(0, 0, 0));
        }
      }

      comingInPx--;
    }
    
    if (firstPixelHue + LED_COUNT >= maxVal)
    {
      for (int px = 0; px < LED_COUNT; px++)
      {
        if (px >= comingOutPx)
        {
          strip.setPixelColor(px, strip.Color(0, 0, 0));
        }
      }

      comingOutPx--;
    }

    strip.show();
    delay(wait);
  }
}


/**
* Wipes out all current colors from center
*/
void wipeOut(int wait)
{

  int centerPos = (LED_COUNT) / 2;

  for (int px = 0; px + centerPos < LED_COUNT; px++)
  {
    strip.setPixelColor(centerPos + px, strip.Color(0, 0, 0));
    strip.setPixelColor(centerPos - px, strip.Color(0, 0, 0));
    strip.show();
    delay(wait);
  }
}

void wipeInCenter(int wait)
{
  for (int i = 0; i <= 1.0 * stripCenterLEDCount * centerPercent; i++)
  {
    for (int c = 0; c < i; c++)
    {
      colorizeCenter();
      strip.setPixelColor(stripLeftLEDCount + c, strip.Color(centerColor[0], centerColor[1], centerColor[2]));
    }
      
    strip.show();
    delay(wait);
  }
}



/**
* Establishes wifi connection and shows succes or failure via LEDs
*/
void wifiConnect()
{
  WiFi.begin(ssid, password);
  int retrys = 0;
  int red[] = {255, 0, 0};
  int green[] = {0, 255, 0};

  paintFromCenter(1, red, red);

  
  Serial.print("Connecting to WiFi " + String(ssid));

  while (WiFi.status() != WL_CONNECTED && retrys < (LED_COUNT) / 2)
  {
    Serial.print(".");
    
    paintFromCenter(retrys, red, red);
    retrys++;

    delay(1500);
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("WiFi connected!");

    wifiClient.setInsecure();

    while (retrys < (LED_COUNT) / 2)
    {
      paintFromCenter(retrys, green, green);
      retrys++;
      delay(15);
    }

    step = 1;
  } else
  {
    Serial.println("WiFi connection failed!");
    
    while (retrys < (LED_COUNT) / 2)
    {
      paintFromCenter(retrys, red, red);
      retrys++;
      delay(15);
    }

    step = 2;
  }

  Serial.println("Entering main loop");
  
}


/**
* Gets the current distance in percent from url for printing on the progress bars
*/
void getDistancePercent()
{
  if (sender.begin(wifiClient, url))
  {
    int httpCode = sender.GET();
    if (httpCode > 0)
    {
      if (httpCode == HTTP_CODE_OK)
      {
        String payload = sender.getString();
        float dst = payload.toFloat();
        Serial.println("Got distance: " + String(dst) + "km");

        if (dst < 0)
        {
          currentDistancePercent = -1;
        } else
        {
          if (dst > largestDistance)
          {
            currentDistancePercent = 0;
          } else
          {
            if (dst > 0.5)
            {
              currentDistancePercent = 1.0 - (dst / largestDistance);
            } else
            {
              currentDistancePercent = 1;
            }
          }
        }
      }
    } else
    {
      Serial.printf("HTTP-Error: ", sender.errorToString(httpCode).c_str());
      currentDistancePercent = -1;
    }

    sender.end();
  } else
  {
    Serial.printf("Failed to connect!");
    currentDistancePercent = -1;
  }

}


/**
* Paints the current distance or error if it fails
*/
void paintDistance()
{
  if (currentDistancePercent < 0)
  {
    paintBeeBoop();
  } else
  {
    leftPercent = currentDistancePercent;
    rightPercent = currentDistancePercent;

    if (currentDistancePercent > 0.75)
    {
      float per = (currentDistancePercent - 0.75) * 4.0;

      leftColor[0] = int(255 * (1 - per));
      leftColor[1] = int(255 * per);
      leftColor[2] = 0;

      rightColor[0] = int(255 * (1 - per));
      rightColor[1] = int(255 * (per));
      rightColor[2] = 0;
    } else
    {
      leftColor[0] = 255;
      leftColor[1] = int(128 * (currentDistancePercent * 1.333));
      leftColor[2] = 0;

      rightColor[0] = 255;
      rightColor[1] = int(255 * (currentDistancePercent * 1.333));
      rightColor[2] = 0;
    }
    
    paint();
  }
}

/**
* Paints the current progress in the current step
*/
void paintProgressStep()
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

  paint();
}



/**
* Goes to the next step
*/
void nextStep()
{
  Serial.println("Proceeding to next step!");
  wipeOut(20);
  strip.setBrightness(255);
  rainbow(5);

  stepStartTime = millis();
      
  if (step < totalSteps)
  {
    step++;
  } else
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      step = 1;
    } else 
    {
      step = 2;
    }
  }
  
  strip.setBrightness(brightness);

  centerPercent =  (step - 1) / ((totalSteps - 1) * 1.0);
  wipeInCenter(50);

  Serial.println("Now we are on step " + String(step));
}


/**
* Starting routine
*/
void setup()
{
  pinMode(BUTTON_PIN, INPUT);
  Serial.begin(9600);

  strip.begin();
  strip.show();
  strip.setBrightness(255);

  wifiConnect();
}


/**
* Main Loop
*/
void loop()
{
  centerPercent =  (step - 1) / ((totalSteps - 1) * 1.0);
  colorizeCenter();

  if (millis() - lastCheck > 1234)
  {
    beeBoop = !beeBoop;
    

    if (step == 1)
    {
      getDistancePercent();
    } else
    {
      // Nothing to fetch here
    }

    lastCheck = millis();
  }



  // Draw constantly
  strip.setBrightness(brightness);
  if (step == 1)
  {
    paintDistance();
  } else
  {
    paintProgressStep();
  }


  // Check for button
  int newStatus = digitalRead(BUTTON_PIN);

  if (lastBtnStatus != newStatus)
  {
    if (newStatus == LOW)
    {
      Serial.println("BUTTON ON");
      nextStep();
    } else
    {
      Serial.println("BUTTON OFF");
    }

    lastBtnStatus = newStatus;
  }

  delay(50);
}


