/*
__/\\\\\\\\\\\\\____/\\\\\\\\\\\\\\\__/\\\\____________/\\\\_        
 _\/\\\/////////\\\_\/\\\///////////__\/\\\\\\________/\\\\\\_       
  _\/\\\_______\/\\\_\/\\\_____________\/\\\//\\\____/\\\//\\\_      
   _\/\\\\\\\\\\\\\/__\/\\\\\\\\\\\_____\/\\\\///\\\/\\\/_\/\\\_     
    _\/\\\/////////____\/\\\///////______\/\\\__\///\\\/___\/\\\_    
     _\/\\\_____________\/\\\_____________\/\\\____\///_____\/\\\_   
      _\/\\\_____________\/\\\_____________\/\\\_____________\/\\\_  
       _\/\\\_____________\/\\\\\\\\\\\\\\\_\/\\\_____________\/\\\_ 
        _\///______________\///////////////__\///______________\///__
*/

#include "config.h"

// Define the NeoPixel object
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, 4, NEO_GRB + NEO_KHZ800);

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        delay(1);
    }

    initializeRadio();

    // Set pin modes for new pins
    pinMode(CHECK_PIN, INPUT);
    pinMode(WRITE_PIN, OUTPUT);

    // Initialize NeoPixel
    pixels.begin();
    pixels.show(); // Initialize all pixels to 'off'

}

void loop() {
    String message;
    bool received = false;
    bool continueChecking = false;

    // Continuously check for received messages
    while (!received) {
        // Set the NeoPixel to yellow, indicating waiting for message
        setNeoPixelColor(pixels.Color(255, 255, 0));

        received = checkForReceivedMessage(message);
        if (received) {
            // If TARGET_STRING is received, activate continuous checking
            if (message == TARGET_STRING) {
                continueChecking = true; // Enable continuous checking of CHECK_PIN
                // Set the NeoPixel to green, indicating message received
                setNeoPixelColor(pixels.Color(0, 255, 0));
                Serial.println("Target string received, starting continuous pin check.");
                break;
            }
        }
    }

    // Once a message is received, enter a new loop to continuously check the CHECK_PIN
    while (continueChecking) {
        if (digitalRead(CHECK_PIN) == HIGH) {
            
            // Set the WRITE_PIN to HIGH
            digitalWrite(WRITE_PIN, HIGH);

            // Set the NeoPixel to red, indicating CHECK_PIN is high
            setNeoPixelColor(pixels.Color(255, 0, 0));
        } else {
            // Set the NeoPixel to purple, indicating CHECK_PIN is not high
            setNeoPixelColor(pixels.Color(128, 0, 128));
        }
    }

}

void setNeoPixelColor(uint32_t color) {
    for (int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, color);
    }
    pixels.show();
}

/*
// NeoPixel color indications:
// Blue: System is starting up. This color is set in the setup() function.
setNeoPixelColor(pixels.Color(0, 0, 255)); 

// Yellow: System is waiting for a message. This color is set at the beginning of the loop when checking for messages.
setNeoPixelColor(pixels.Color(255, 255, 0));

// Green: Target message has been received. This color is set once the expected message is detected.
setNeoPixelColor(pixels.Color(0, 255, 0));

// Red: CHECK_PIN is HIGH. This color is set within the continuous checking loop when the CHECK_PIN is read as HIGH.
setNeoPixelColor(pixels.Color(255, 0, 0));

// Purple: CHECK_PIN is LOW or not HIGH. This color is set within the continuous checking loop when the CHECK_PIN is not HIGH.
setNeoPixelColor(pixels.Color(128, 0, 128));
*/
