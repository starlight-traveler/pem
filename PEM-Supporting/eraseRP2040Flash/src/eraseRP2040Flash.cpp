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

void eraseFlash() {
  Serial.println("Erasing flash memory...");

  uint32_t ints = save_and_disable_interrupts();
  // Erase the last sector of the flash memory
  flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
  restore_interrupts(ints);

  Serial.println("Flash memory erased.");
}

void setup() {
  Serial.begin(9600);
  while (!Serial); // Wait for the serial port to connect

  // Call the erase function
  eraseFlash();

  // Reading flash memory after erasing to verify
  Serial.println("Verifying flash memory after erase...");

  for (unsigned int page = 0; page < FLASH_SECTOR_SIZE / FLASH_PAGE_SIZE; page++) {
    unsigned int addr = XIP_BASE + FLASH_TARGET_OFFSET + (page * FLASH_PAGE_SIZE);
    const char* p = (const char*)addr;

    if (*p != (char)0xFF) {
      Serial.print("Data found on page ");
      Serial.print(page);
      Serial.print(" (at 0x");
      Serial.print(addr, HEX);
      Serial.print("): ");
      Serial.println(p);
    }
  }

  Serial.println("Verification complete.");
}

void loop() {
  // Empty loop
}
