# Evening-Progressbar
Arduino-powered progressbar for evening routine


## Main idea
This project enables an optical support for the evening routine (for the kids..). With the help of a RGB LED strip, this shows
- (as first step) how far away from home daddy is
- (for the other steps) how much time we already spent on this step (meaning, you get an idea, how much time is left)
- and at which step we currently are

To continue to the next step, we will use a button.

The two progress bars are realized by a single LED strip that is cut in two pieces of a length you define based on the size of your deployment. Main point is, you connect only connect one of them to the Arduino.


## Required hardware & other things
- We used an Arduino Nano WiFi. Important is "WiFi", if you like to use the "how far away is daddy" feature
- LED Strip. We used one of AZ Delivery on Amazon, which you can cut in smaller strips and wire manually
- Something to put the LED strip and graphics of the steps on. We used frosted plexiglass, where you could put the LEDs on the back and pictograms for the steps on the front
- A button. We used a buzzer, because BUZZER!

For tracking the distance, you need an app on your phone along with a webserver for the php script.
- As phone app, I'd recommend [GPSLogger](https://github.com/mendhak/gpslogger/) on Android, where you can enter an url with custom parameters and basic auth that matches this script here
- As webserver, you may use a public one, as this one uses basic auth to prevent unauthorized access, BUT: note, that the Arduino requires plain http, which is unencrypted (vulnerable to man in the middle attacks).


## How to use

PHP:
- Copy `credentials.json.example` (in the subdirectory) to `credentials.json` and insert your credentials and home coordinates
- Upload all files to your webserver (make sure you include `.htaccess`!)

Arduino:
- In the first few lines, you need to configure all required parameters