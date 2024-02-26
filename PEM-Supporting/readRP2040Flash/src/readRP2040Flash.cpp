#include <Arduino.h>

extern "C" {
  #include <hardware/sync.h>
  #include <hardware/flash.h>
};

// Flash memory parameters
#define FLASH_PAGE_SIZE 256
#define FLASH_SECTOR_SIZE 4096
#define FLASH_BLOCK_SIZE 65536
#define PICO_FLASH_SIZE_BYTES 8388608
#define XIP_BASE 0x10000000
#define FLASH_TARGET_OFFSET (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE)

void setup() {
  Serial.begin(9600);
  while(!Serial); // Wait for the serial port to connect

  Serial.println("Reading flash memory...");

  // Read the flash using memory-mapped addresses
  for (unsigned int page = 0; page < FLASH_SECTOR_SIZE / FLASH_PAGE_SIZE; page++) {
    unsigned int addr = XIP_BASE + FLASH_TARGET_OFFSET + (page * FLASH_PAGE_SIZE);
    const char* p = (const char*)addr;

    // Check if the first byte is not 0xFF, indicating possible data
    if (*p != (char)0xFF) {
      Serial.print("String on page ");
      Serial.print(page);
      Serial.print(" (at 0x");
      Serial.print(addr, HEX);
      Serial.print("): ");
      Serial.println(p); // p points to a null-terminated string
    }
  }
}

void loop() {
  // Empty loop
}
