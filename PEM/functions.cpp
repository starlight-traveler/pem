#include "config.h"

// Declare the singleton instance of the radio driver
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
