#include "config.h"

// Define the NeoPixel and RF95 instances in the global scope
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void initializeRadio() {
    pinMode(RFM95_RST, OUTPUT);
    digitalWrite(RFM95_RST, HIGH);

    // Manual reset
    digitalWrite(RFM95_RST, LOW);
    delay(10);
    digitalWrite(RFM95_RST, HIGH);
    delay(10);

    while (!rf95.init()) {
        Serial.println("LoRa failed to initialize!");
        while (1);
    }
    Serial.println("LoRa radio init OK!");

    if (!rf95.setFrequency(RF95_FREQ)) {
        Serial.println("setFrequency failed!");
        while (1);
    }

    // Configure LoRa transmitter settings
    rf95.setTxPower(TX_POWER, false);
    rf95.setSignalBandwidth(BANDWIDTH * 1000); // Convert kHz to Hz
    rf95.setSpreadingFactor(SPREADING_FACTOR);
    rf95.setCodingRate4(CODING_RATE);
    rf95.setPreambleLength(PREAMBLE_LENGTH);
}

bool checkForReceivedMessage(String &message) {
    if (rf95.available()) {
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);

        if (rf95.recv(buf, &len)) {
            digitalWrite(LED_BUILTIN, HIGH);
            message = String((char*)buf);
            Serial.print("Got: "); Serial.println(message);
            digitalWrite(LED_BUILTIN, LOW);
            return true;
        } else {
            Serial.println("Receive Failed.");
        }
    }
    return false;
}

void transmitMessage(const String &message) {
    Serial.print("Transmitting: "); Serial.println(message);
    
    // Convert the message to a byte array for transmission
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    message.getBytes(buf, RH_RF95_MAX_MESSAGE_LEN);
    size_t len = message.length();

    // Send the message
    if (rf95.send(buf, len)) {
        Serial.println("Message sent successfully");
    } else {
        Serial.println("Message failed to send");
    }
}
