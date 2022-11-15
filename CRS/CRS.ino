//Files and classes to include
#include <MKRWAN_v2.h>
#define SMALL_LEFT     &0x1b401d10111b6100
#define BIG_CENTER     0x1b401d21111b6101
#define FEED_LINES     0X0a0a0a0a
#define NEXT_LINE      0x0d0a
#define DISPATCHED(X) (X & !(X & 0xE0))
#define EXPIRED(X)    (X & 0xE0)
#define RANDOM_CODE   (uint16_t)random(0, 10000)
#define NEXT_CODE     (++code%10000)
#define ALL_CODES     code = 0; code++; code<10000
#define CODE          data[code]
#define BUFFER        (char*)&buffer[0]

//Constant variables (unchanging)
LoRaModem lora;
const uint8_t  buttonPin = 2;
const uint8_t  smallLeft = 2;
const uint64_t interval  = 2700000;

//Variables (changing)
uint8_t  buffer[32]  = {0};
uint8_t  data[10000] = {0};
uint16_t code        = 0;
uint64_t now         = 0;
uint64_t lastCheck   = 0;

//Hardware Setup
void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), request, LOW);
  Serial.begin(9600);
  while (!Serial);
  while (!lora.begin(US915));
}

//Program loop
void loop() {
  if (increment()) ageCodes();
}

bool increment() {
  now = millis();
  if (now - lastCheck > interval) {
    lastCheck = now;
    return true;
  }
  return false;
}

void ageCodes() {
  for (ALL_CODES) if (CODE) CODE++;
}

void request() {
  getCode();
  printCode();
  sendCode();
  reset();
}

void getCode() {
  code = RANDOM_CODE;
  while DISPATCHED(CODE) NEXT_CODE;
  CODE++;
  itoa(code, BUFFER, 10);
}

void printCode() {
  Serial.write("Your free air code is:");
  Serial.write(NEXT_LINE);
  Serial.write(BIG_CENTER);
  Serial.write(BUFFER);
  Serial.write(NEXT_LINE);
  Serial.write(SMALL_LEFT);
  Serial.write(FEED_LINES);
}

void sendCode() {
  lora.beginPacket();
  lora.print(BUFFER);
  lora.endPacket();
}

void reset() {
  memset(BUFFER, 0, 32);
  code = 0;
  lastCheck += 1000;
  delay(1000);
}