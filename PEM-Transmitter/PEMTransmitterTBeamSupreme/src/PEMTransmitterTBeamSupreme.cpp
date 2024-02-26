#include <SPI.h>
#include <RH_RF95.h>
#include <U8g2lib.h> // Include U8g2 library for the OLED

// Updated Pin Definitions for LILYGO_TBeamS3_SUPREME_V3_0
#define RADIO_SCLK_PIN 12
#define RADIO_MISO_PIN 13
#define RADIO_MOSI_PIN 11
#define RADIO_CS_PIN 10
#define RADIO_DIO0_PIN -1 // Set to -1 if not used
#define RADIO_RST_PIN 5
#define RADIO_DIO1_PIN 1
#define RADIO_BUSY_PIN 4
#define BUTTON_PIN 0 // Button pin

// I2C for OLED Display
#define I2C_SDA 17
#define I2C_SCL 18

// Frequency
#define RF95_FREQ 905.0

// Define LoRa Parameters
#define TX_POWER 23         // Transmit Power (LoRa, dBm)
#define BANDWIDTH 125       // Bandwidth (LoRa, kHz)
#define SPREADING_FACTOR 12 // Spreading Factor (LoRa)
#define CODING_RATE 8       // Coding Rate (LoRa, 4/x)
#define PREAMBLE_LENGTH 12  // Preamble Length (LoRa)

// Singleton instance of the radio driver
RH_RF95 rf95(RADIO_CS_PIN, RADIO_DIO1_PIN);

// OLED Display instance using hardware I2C
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, I2C_SCL, I2C_SDA);

// State Definitions
enum State
{
  TRANSMIT,
  RECEIVE
};
State currentState = RECEIVE;
State previousState = RECEIVE; // To track state changes

void setup()
{
  pinMode(RADIO_RST_PIN, OUTPUT);
  digitalWrite(RADIO_RST_PIN, HIGH);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Setup button with internal pull-up

  Serial.begin(115200);
  Serial.println("T-Beam LoRa Mode Toggle!");

  // Manual reset of LoRa module
  digitalWrite(RADIO_RST_PIN, LOW);
  delay(10);
  digitalWrite(RADIO_RST_PIN, HIGH);
  delay(10);

  if (!rf95.init())
  {
    Serial.println("LoRa radio init failed");
    while (1)
      ;
  }
  Serial.println("LoRa radio init OK!");

  if (!rf95.setFrequency(RF95_FREQ))
  {
    Serial.println("setFrequency failed");
    while (1)
      ;
  }
  Serial.print("Set Freq to: ");
  Serial.println(RF95_FREQ);

  // Set LoRa parameters
  rf95.setTxPower(TX_POWER, false);
  rf95.setSignalBandwidth(BANDWIDTH * 1000); // Convert kHz to Hz
  rf95.setSpreadingFactor(SPREADING_FACTOR);
  rf95.setCodingRate4(CODING_RATE);
  rf95.setPreambleLength(PREAMBLE_LENGTH);

  // Initialize the OLED display
  u8g2.begin();
  u8g2.setFont(u8g2_font_ncenB08_tr); // Choose a suitable font
  updateDisplay();                    // Initial display update
}

void loop()
{
  // Receive Mode
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (rf95.waitAvailableTimeout(3000) && rf95.recv(buf, &len))
  {
    buf[len] = 0; // Ensure null termination
    Serial.println((char *)buf);
    displayMessage((char *)buf);
  }
}

void displayMessage(const char *message)
{
  u8g2.clearBuffer();
  int textWidth = u8g2.getStrWidth(message);
  int x = (128 - textWidth) / 2; // Center horizontally
  int y = 32;                    // Center vertically (for an 8-pixel high font on a 64-pixel high screen)
  u8g2.setCursor(x, y);
  u8g2.print(message);
  u8g2.sendBuffer();
}

void updateDisplay()
{
  const char *message = (currentState == TRANSMIT) ? "Transmitting" : "Receiving";
  displayMessage(message);
}
