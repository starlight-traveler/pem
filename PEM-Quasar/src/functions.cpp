extern "C" {
  #include <hardware/sync.h>
  #include <hardware/flash.h>
};

// Set the target offset to the last sector of flash
#define FLASH_TARGET_OFFSET (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE)

char buf[FLASH_PAGE_SIZE];  // One page buffer for the string
int *p, addr;
unsigned int page; // To prevent comparison of unsigned and signed int
int first_empty_page = -1;

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
  rf95.setSignalBandwidth(BANDWIDTH * 1000);  // Convert kHz to Hz
  rf95.setSpreadingFactor(SPREADING_FACTOR);
  rf95.setCodingRate4(CODING_RATE);
  rf95.setPreambleLength(PREAMBLE_LENGTH);
  rf95.setPromiscuous(true);

}

bool checkForReceivedMessage(String &message, float &triggerAltitude) {
  if (rf95.available()) {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len)) {
      digitalWrite(LED_BUILTIN, HIGH);
      message = String((char *)buf);

      if (message.startsWith("OPCODE:")) {
                uint8_t opcode = strtol(message.substring(7).c_str(), NULL, 16);

                switch (opcode) {
                    case 0x01:
                        transmitMessage("altitudeTargetSet: 1");
                        break;
                    case 0x02:
                        transmitMessage("Trigger Altitude: " + String(triggerAltitude));
                        break;
                    case 0x03:
                        transmitMessage("Eggtimer Check Pin State: " + String(digitalRead(CHECK_PIN)));
                        break;
                    case 0x04:
                        transmitMessage("Eggtimer Write Pin State: " + String(digitalRead(WRITE_PIN)));
                        break;
                    case 0x05:
                        transmitMessage("Altimeter Write Pin State: " + String(digitalRead(ALTITUDE_READY_PIN)));
                        break;
                    case 0x06:
                        transmitMessage("Received: 0");
                        break;
                    case 0x07:
                        digitalWrite(WRITE_PIN, LOW);
                        transmitMessage("Eggtimer Write Pin Low");
                        break;
                    case 0x08:
                        digitalWrite(ALTITUDE_READY_PIN, LOW);
                        transmitMessage("Altimeter Write Pin Low");
                        break;
                    case 0x09:
                        static int heartbeatCount = 0;
                        heartbeatCount++;
                        transmitMessage("Heartbeat: " + String(heartbeatCount));
                        break;
                    case 0x0A:
                        // Implementation for opcode 0x0A
                        break;
                    case 0x0B:
                        // Implementation for resetting RP2040
                        rp2040.reboot(); // This will reset the RP2040
                        break;

                }
            }

      Serial.print("Got: ");
      Serial.println(message);
      digitalWrite(LED_BUILTIN, LOW);
      return true;
    } else {
      Serial.println("Receive Failed.");
    }
  }
  return false;
}


bool checkAndSetAltitude(float triggerAltitude) {
  float altitude = altimeter.getAltitude();  // Read current altitude
  Serial.print("Current Altitude: ");
  Serial.println(altitude);

  if (altitude >= triggerAltitude) {
    digitalWrite(ALTITUDE_READY_PIN, HIGH);  // Set the pin high
    Serial.println("Altitude condition met, setting pin high.");
    return true;  // Return true when target altitude is reached
  }

  return false;  // Return false otherwise
}

void transmitMessage(const String &message) {
  Serial.print("Transmitting: ");
  Serial.println(message);

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

// Pizeo Checking

void beepPiezo() {
  digitalWrite(PIEZO_PIN, HIGH);
}

// OP Code Checking, Will Return Value
void checkForOpcode(bool &received, bool &continueChecking, bool &altitudeTargetSet, float &triggerAltitude) {
    if (rf95.available()) {
        uint8_t buffer[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t length = sizeof(buffer);

        if (rf95.recv(buffer, &length)) {
            String message = String((char *)buffer);

            Serial.println(message);

            if (message.startsWith("OPCODE:")) {
                uint8_t opcode = strtol(message.substring(7).c_str(), NULL, 16);

                switch (opcode) {
                    case 0x01:
                        transmitMessage("altitudeTargetSet: " + String(altitudeTargetSet));
                        break;
                    case 0x02:
                        transmitMessage("Trigger Altitude: " + String(triggerAltitude));
                        break;
                    case 0x03:
                        transmitMessage("Eggtimer Check Pin State: " + String(digitalRead(CHECK_PIN)));
                        break;
                    case 0x04:
                        transmitMessage("Eggtimer Write Pin State: " + String(digitalRead(WRITE_PIN)));
                        break;
                    case 0x05:
                        transmitMessage("Altimeter Write Pin State: " + String(digitalRead(ALTITUDE_READY_PIN)));
                        break;
                    case 0x06:
                        transmitMessage("Received: " + String(received));
                        break;
                    case 0x07:
                        digitalWrite(WRITE_PIN, LOW);
                        transmitMessage("Eggtimer Write Pin Low");
                        break;
                    case 0x08:
                        digitalWrite(ALTITUDE_READY_PIN, LOW);
                        transmitMessage("Altimeter Write Pin Low");
                        break;
                    case 0x09:
                        static int heartbeatCount = 0;
                        heartbeatCount++;
                        transmitMessage("Heartbeat: " + String(heartbeatCount));
                        break;
                    case 0x0A:
                        // Implementation for opcode 0x0A
                        break;
                    case 0x0B:
                        // Implementation for resetting RP2040
                        rp2040.reboot(); // This will reset the RP2040
                        break;

                }
            }
        }
    }
}

// Flash stuff, do not touch.

void writeToFlash(const char* message) {
  // Check if the message is too long to fit in a page
  if (strlen(message) >= FLASH_PAGE_SIZE) {
    Serial.println("Error: Message too long for a single flash page.");
    return;
  }

  // Find the first empty page again in case it has changed
  int new_empty_page = -1;
  for (page = 0; page < FLASH_SECTOR_SIZE / FLASH_PAGE_SIZE; page++) {
    addr = XIP_BASE + FLASH_TARGET_OFFSET + (page * FLASH_PAGE_SIZE);
    p = (int *)addr;
    if (*p == -1) {
      new_empty_page = page;
      break;
    }
  }

  if (new_empty_page < 0) {
    Serial.println("No empty page found, erasing sector...");
    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
    new_empty_page = 0;
    restore_interrupts(ints);
  }

  // Clear the buffer and copy the message into it
  memset(buf, 0xFF, sizeof(buf));  // Fill buffer with 0xFF
  strncpy(buf, message, FLASH_PAGE_SIZE - 1);  // Copy message to buffer
  buf[FLASH_PAGE_SIZE - 1] = '\0';  // Ensure null termination

  // Write the buffer to the flash memory
  Serial.println("Writing message to flash...");
  uint32_t ints = save_and_disable_interrupts();
  flash_range_program(FLASH_TARGET_OFFSET + (new_empty_page * FLASH_PAGE_SIZE), (const uint8_t *)buf, FLASH_PAGE_SIZE);
  restore_interrupts(ints);

  Serial.println("Message written to flash successfully.");
}
