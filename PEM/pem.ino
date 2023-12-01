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

Adafruit_MPL3115A2 altimeter = Adafruit_MPL3115A2();

void setup() {
  Serial.begin(115200);

  initializeRadio();

  // Set pin modes for new pins
  pinMode(CHECK_PIN, INPUT);
  pinMode(WRITE_PIN, OUTPUT);
  pinMode(ALTITUDE_READY_PIN, OUTPUT);  // Pin to indicate altitude condition met


  // Initialize NeoPixel
  pixels.begin();
  pixels.show();  // Initialize all pixels to 'off'

  // Initialize MPL3115A2
  if (!altimeter.begin()) {
    Serial.println("Could not find a valid MPL3115A2 sensor, check wiring!");
    while (1)
      ;
  }
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
        continueChecking = true;  // Enable continuous checking of CHECK_PIN
        // Set the NeoPixel to green, indicating message received
        setNeoPixelColor(pixels.Color(0, 255, 0));
        Serial.println("Target string received, starting continuous pin check.");
        break;
      } else {
        // If received string is not the TARGET_STRING, continue listening
        received = false;  // Reset the received flag
        message = "";      // Clear the message buffer
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
      // Leave once pin written to hight
      break;
    } else {
      // Set the NeoPixel to purple, indicating CHECK_PIN is not high
      setNeoPixelColor(pixels.Color(128, 0, 128));
    }
  }

  while (1) {
    bool altitudeReached = checkAndSetAltitude();
    if (altitudeReached) {
      Serial.println("Target altitude reached, exiting loop.");
      break;  // Break out of the loop once target altitude is reached
    }
  }

  setNeoPixelColor(pixels.Color(0, 0, 255));  // Change NeoPixel color to blue

  // Transmit that system has fired
  while (1) {

    transmitMessage(TRANSMIT_STRING);
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
