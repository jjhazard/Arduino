//Files and classes to include
#include <MKRWAN_v2.h>
#define DISPATCHED(X) (X & !(X & 0xE0))
#define EXPIRED(X)    (X & 0xE0)
#define TRANSLATE(X)  *(uint16_t*)X
#define CODE          data[code]
#define BUFFER        (char*)&buffer[0]
#define ALL_CODES     code = 0; code++; code<10000
#define VALID_CODE    (CODE & CODE<10000)
//Constant variables (unchanging)
LoRaModem lora;
const uint8_t  buzzerPin = 2;
const uint8_t  relayPin  = 3;
const uint64_t interval  = 2700000;

//Variables (changing)
uint8_t  key         = 0;
uint8_t  bufferIndex = 0;
uint8_t  buffer[32]  = {0};
uint8_t  data[10000] = {0};
uint16_t code        = 0;
uint64_t now         = 0;
uint64_t lastCheck   = 0;
uint64_t lastPress   = 0;

//Hardware Setup
void setup() {
  pinMode(buzzerPin, OUTPUT);
  pinMode(relayPin, OUTPUT);
  Serial.begin(9600);
  while (!Serial);
  while (!lora.begin(US915));
}

//Program loop
void loop() {
  if (increment()) ageCodes();
  while (Serial.available()) {
    lastPress = millis();
    key = Serial.read();
    buzz();
    switch(key) {
      case '*':
        process();
        break;
      case '#':
        reject();
        break;
      default:
        buffer[bufferIndex] = key;
        bufferIndex++; 
    }
    while (!Serial.available() || !fiveSeconds());
  }
  if (lora.parsePacket()) {
    while (lora.available()) {
      buffer[bufferIndex] = lora.read();
      bufferIndex++;
    }
    buffer[bufferIndex] = '\0';
    if (TRANSLATE(BUFFER) < 10000) {
      if (data[TRANSLATE(BUFFER)]);
    }
    clear();
  }
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

void buzz() {
  digitalWrite(buzzerPin, HIGH);
  delay(100);
  digitalWrite(buzzerPin, LOW);
}

void process() {
  code = atoi(BUFFER);
  if (VALID_CODE) activate();
  else reject();
}

void activate() {
  digitalWrite(relayPin, HIGH);
  delay(3000);
  digitalWrite(relayPin, LOW);
  clear();
}

void reject() {
  for (int beep=0; beep++; beep<3) {
    buzz();
    delay(70);
  }
  lastPress = 0;
  clear();
}

void clear() {
  memset(BUFFER, 0, 32);
  code = 0;
}

bool fiveSeconds() {
  if (!lastPress) return true;
  if (millis() - lastPress < 5000UL) return false;
  reject();
  return true;
}
