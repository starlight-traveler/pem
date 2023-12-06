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

#include "config.h"  // Include configuration header file

Adafruit_MPL3115A2 altimeter = Adafruit_MPL3115A2();  // Initialize altimeter object

void setup() {

  Serial.begin(115200);  // Start serial communication at 115200 baud rate

  initializeRadio();  // Initialize the radio module

  // Set pin modes for various functionalities
  pinMode(CHECK_PIN, INPUT);            // Set CHECK_PIN as input
  pinMode(WRITE_PIN, OUTPUT);           // Set WRITE_PIN as output
  pinMode(ALTITUDE_READY_PIN, OUTPUT);  // Set ALTITUDE_READY_PIN as output, used to indicate altitude condition met
  pinMode(PIEZO_PIN, OUTPUT);           // Set PIEZO_PIN as output for piezo buzzer

  // Initialize NeoPixel LED
  pixels.begin();  // Initialize NeoPixel
  pixels.show();   // Turn off all pixels (initial state)

  delay(5000);

  transmitMessage("PEM Module Live.");

  // Initialize MPL3115A2 altimeter sensor
  if (!altimeter.begin()) {
    Serial.println("Could not find a valid MPL3115A2 sensor, check wiring!");
    while (1)
      ;  // Infinite loop if sensor not found
  }
}

void loop() {
  String message;                  // To store received message
  bool received = false;           // Flag to indicate message receipt
  bool continueChecking = false;   // Flag for continuous checking of CHECK_PIN
  bool altitudeTargetSet = false;  // Flag to indicate if target altitude is set
  float triggerAltitude = 0.0;     // Variable to store target altitude


  // Set target altitude if not already set
  if (!altitudeTargetSet) {
    triggerAltitude = altimeter.getAltitude() + 200.0;  // Set target altitude 200 feet above initial altitude
    altitudeTargetSet = true;                           // Mark target altitude as set
    String messageAlt = String(millis()) + " - Altitude target set to " + String(triggerAltitude) + "!";
    const char* messageAltChar = messageAlt.c_str();
    writeToFlash(messageAltChar);
    Serial.print("Target Altitude Set: ");
    Serial.println(triggerAltitude);
  }

  // Continuously check for received messages
  while (!received) {
    setNeoPixelColor(pixels.Color(255, 255, 0));                   // Set NeoPixel to yellow, indicating waiting for message
    received = checkForReceivedMessage(message, triggerAltitude);  // Check for received message
    if (received) {
      // If specific target string is received
      if (message == TARGET_STRING) {
        continueChecking = true;                    // Enable continuous checking
        setNeoPixelColor(pixels.Color(0, 255, 0));  // Set NeoPixel to green, indicating target string received
        Serial.println("Target string received, starting continuous pin check.");
        String messageTarget = String(millis()) + " - Target string recieved!";
        const char* messageTargetChar = messageTarget.c_str();
        writeToFlash(messageTargetChar);
        break;  // Break the loop as target string is received
      } else {
        // Reset if received string is not target string
        received = false;
        message = "";
      }
    }
  }

  // Continuous checking of CHECK_PIN after receiving target message
  while (continueChecking) {
    if (digitalRead(CHECK_PIN) == HIGH) {
      digitalWrite(WRITE_PIN, HIGH);  // Set WRITE_PIN high
      String messageEggTimer = String(millis()) + " - Eggtimer Activated!";
      const char* messageEggTimerChar = messageEggTimer.c_str();
      writeToFlash(messageEggTimerChar);
      setNeoPixelColor(pixels.Color(255, 0, 0));  // Set NeoPixel to red, indicating CHECK_PIN is high
      break;                                      // Exit loop once pin is written high
    } else {
      setNeoPixelColor(pixels.Color(128, 0, 128));  // Set NeoPixel to purple, indicating CHECK_PIN is not high
    }
    checkForOpcode(received, continueChecking, altitudeTargetSet, triggerAltitude);  // Check for opcodes
  }

  // Loop until target altitude is reached
  while (1) {
    bool altitudeReached = checkAndSetAltitude(triggerAltitude);  // Check if target altitude is reached
    checkForOpcode(received, continueChecking, altitudeTargetSet, triggerAltitude);
    if (altitudeReached) {
      Serial.println("Target altitude reached, exiting loop.");
      String messageAltReached = String(millis()) + " - Target Altitude Reached!";
      const char* messageAltReachedChar = messageAltReached.c_str();
      writeToFlash(messageAltReachedChar);
      break;  // Exit loop once target altitude is reached
    }
  }

  setNeoPixelColor(pixels.Color(0, 0, 255));  // Set NeoPixel color to blue

  String messageFired = String(millis()) + " - Fired!";
  const char* messageFiredChar = messageFired.c_str();
  writeToFlash(messageFiredChar);

  // Transmit that system has fired
  while (1) {
    checkForOpcode(received, continueChecking, altitudeTargetSet, triggerAltitude);
    transmitMessage(TRANSMIT_STRING);  // Transmit a predefined message
  }
}

void setNeoPixelColor(uint32_t color) {
  // Function to set color of NeoPixel
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, color);  // Set color for each pixel
  }
  pixels.show();  // Apply the color change
}

/*
  NeoPixel color indications:
  - Blue: System is starting up. Set in the setup() function.
  - Yellow: System waiting for a message. Set at the beginning of the loop.
  - Green: Target message received. Set when expected message detected.
  - Red: CHECK_PIN is HIGH. Set in the continuous checking loop.
  - Purple: CHECK_PIN is LOW or not HIGH. Set in the continuous checking loop.
*/
