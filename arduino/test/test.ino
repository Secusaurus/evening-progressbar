#include <Adafruit_NeoPixel.h>

#define LED_PIN D8
#define LED_COUNT 60

const int buttonPin = D3;
//const int stripeControl = D8;

int lastStatus = HIGH;


Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);


void setup()
{
  pinMode(buttonPin, INPUT);
  Serial.begin(9600);

  strip.begin();
  strip.show();
  strip.setBrightness(200);

}

void loop()
{
  int newStatus = digitalRead(buttonPin);

  if (lastStatus != newStatus)
  {
    if (newStatus == LOW)
    {
      Serial.println("ON");
      rainbow(5);
    } else
    {
      Serial.println("OFF");
      colorWipe(strip.Color(0, 0, 0), 10);
    }

    lastStatus = newStatus;
  }

  delay(200);
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
