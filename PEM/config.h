#ifndef CONFIG_H
#define CONFIG_H

//*******************************************//
//                Libraries                  //
//*******************************************//
#include <Arduino.h>
#include <RH_RF95.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <Adafruit_MPL3115A2.h>

//*******************************************//
//             LoRa Settings                 //
//*******************************************//
#define RF95_FREQ 915.0                // LoRa Frequency (MHz)
#define RFM95_CS 16                    // Chip Select pin
#define RFM95_INT 21                   // Interrupt pin
#define RFM95_RST 17                   // Reset pin
#define TX_POWER 23                    // Transmit Power (LoRa, dBm)
#define BANDWIDTH 125                  // Bandwidth (LoRa, kHz)
#define SPREADING_FACTOR 12            // Spreading Factor (LoRa)
#define CODING_RATE 8                  // Coding Rate (LoRa, 4/x)
#define PREAMBLE_LENGTH 12             // Preamble Length (LoRa)
#define TARGET_STRING "EXIT_STRING"    // The string upon which to exit the loop
#define TRANSMIT_STRING "YourMessage"  // Replace with your desired default transmit message

//*******************************************//
//             Ignition Settings             //
//*******************************************//
#define CHECK_PIN 2  // Define the pin number to check
#define WRITE_PIN 10  // Define the pin number to write high
#define ALTITUDE_READY_PIN 11  // Change to a suitable pin number

//*******************************************//
//             Neopixel Settings             //
//*******************************************//
#define NUMPIXELS 1  // Number of NeoPixels
#define PIEZO_PIN 13 // Define the pin number for the piezo buzzer


//*******************************************//
//             Function Declarations         //
//*******************************************//
void initializeRadio();
bool checkForReceivedMessage(String &message);
void transmitMessage(const String &message);
bool checkAndSetAltitude(float triggerAltitude);

//*******************************************//
//                Extern                     //
//*******************************************//
extern RH_RF95 rf95;
extern Adafruit_NeoPixel pixels;
extern Adafruit_MPL3115A2 altimeter;


#endif  // CONFIG_H
