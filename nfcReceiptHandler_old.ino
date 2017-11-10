#include "SPI.h"
#include "PN532_SPI.h"
#include "emulatetag.h"
#include "NdefMessage.h"

#define LED_RED D3
#define LED_GREEN D4
#define BUZZER D0

PN532_SPI pn532spi(SPI, 5);
EmulateTag nfc(pn532spi);

uint8_t ndefBuf[2048];
NdefMessage message;

uint8_t uid[3] = { 0x12, 0x34, 0x56 };

void setup()
{
  Serial.begin(9600);
  Serial.println("\n======== KASSABONNEN DING ========");

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(BUZZER, HIGH);
  
  // uid must be 3 bytes!
  nfc.setUid(uid);

  // Init nfc
  nfc.init();

  // deny writing to the tag
  nfc.setTagWriteable(false);
}

int loopNr = 0;
char buffer[1500];

void loop(){
  boolean newTextReceived = false;
  int charNr = 0;
  char c;
  while(Serial.available() > 0 && charNr < sizeof(buffer)-1) {
    digitalWrite(LED_RED, LOW);
    newTextReceived = true;
    Serial.print(".");
    if((charNr+1) % 80 == 0) Serial.println();
    c = Serial.read();
    buffer[charNr++] = c;
    buffer[charNr] = 0;

    int maxDelay = 1000;
    while(maxDelay-- > 0) {
      if(Serial.available() > 0) {
        break;
      }
      delayMicroseconds(1000);
    }
  }
  digitalWrite(LED_RED, HIGH);

  if(newTextReceived) {
    digitalWrite(BUZZER, LOW);
    delay(200);
    digitalWrite(BUZZER, HIGH);
    Serial.print("\nSetting new text: '");
    Serial.print(buffer);
    Serial.println("'");
    sendToNFC(buffer);
  }
  
  digitalWrite(LED_GREEN, LOW);
  // start emulation (blocks)
  //nfc.emulate();
        
  // or start emulation with timeout
  boolean success = nfc.emulate(500);
  if(success) {
    digitalWrite(BUZZER, LOW);
    delay(100);
    digitalWrite(BUZZER, HIGH);
    delay(100);
    digitalWrite(BUZZER, LOW);
    delay(100);
    digitalWrite(BUZZER, HIGH);
  }
  Serial.println();
  Serial.print(success);
  Serial.println(" - Looping...");
  digitalWrite(LED_GREEN, HIGH);

  delay(100);
  loopNr++;
}

void sendToNFC(char* buffer) {
  message = NdefMessage();
  message.addTextRecord(buffer);
  
  int messageSize = message.getEncodedSize();
  Serial.print("Ndef encoded message size: ");
  Serial.println(messageSize);
  if (messageSize > sizeof(ndefBuf)) {
      Serial.println("ndefBuf is too small.");
      message = NdefMessage();
      message.addTextRecord("ndefBuf is too small.");
  }

  message.encode(ndefBuf);
  
  // comment out this command for no ndef message
  nfc.setNdefFile(ndefBuf, messageSize);
}

void serialEvent() {
  Serial.println("Serial event.");
}

