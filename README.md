# Evening-Progressbar
Arduino-powered progressbar for evening routine


## Main idea
This project enables an optical support for the evening routine (for the kids..). With the help of a RGB LED strip, this shows
- (as first step) how far away from home daddy is
- (for the other steps) how much time we already spent on this step (meaning, you get an idea, how much time is left)
- and at which step we currently are

To continue to the next step, we will use a button.

The three progress bars are realized by a single LED strip that is cut in three pieces of a length you define based on the size of your deployment. Main point is, you connect only connect the main piece to the Arduino.


## Required hardware & other things
- We used an ESP. Important is "WiFi", if you like to use the "how far away is daddy" feature
- LED Strip. We used one of AZ Delivery on Amazon, which you can cut in smaller strips and wire manually
- Something to put the LED strip and graphics of the steps on. We used frosted plexiglass, where you could put the LEDs on the back and pictograms for the steps on the front
- A button. We used a buzzer, because BUZZER!

For tracking the distance, you need an app on your phone along with a webserver for the php script.
- As phone app, I'd recommend [GPSLogger](https://github.com/mendhak/gpslogger/) on Android, where you can enter an url with custom parameters and basic auth that matches this script here
- As webserver, you may use a public one that can do php, as this one uses a token-parameter to prevent unauthorized access, BUT: note, that this Arduino implementation does not validate the https-certificate (unless you want to import certificates to your device), which is vulnerable to man in the middle attacks (but I don't think that this is too private information here).

### Layout for the LED-Strip

The LED-strip will be connected on the upper left side, go from there down to the bottom (cut here) then from left to right for the center bar, then (cut) from bottom to top on the right side. Something like this:
```
X  < connect Arduino cables
|                              |
|                              |
|  < Left bar  |  Right bar >  |
|                              |
|          Center bar          |
|              |               |
  ----------------------------
```

## How to use

PHP:
- Copy `credentials.json.example` (in the subdirectory) to `credentials.json` and insert your credentials and home coordinates
- Upload all files to your webserver (make sure you include `.htaccess`!)

Arduino:
- Uses the following libraries:
  - ESP8266WiFi (should be included in [ESP8266 Core](https://github.com/esp8266/Arduino))
  - ESP8266HTTPClient (should be included in [ESP8266 Core](https://github.com/esp8266/Arduino))
  - [Adafruit_NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel)
- Copy `config.h.example` to `config.h` and configure all your parameters
- Build!