//Use SPI Bus
#if 1
#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>

PN532_SPI pn532spi(SPI, 7);
NfcAdapter nfc = NfcAdapter(pn532spi);
#else
// Use I2C Bus
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>

PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);
#endif

int loopcount = 0;
void setup() {
      Serial.begin(9600);
      Serial.println("NDEF Writer");
      nfc.begin();
}

void loop() {
    loopcount++;
    //Write to Tag
    Serial.println("\nPlace a formatted Mifare Classic NFC tag on the reader.");
    if (nfc.tagPresent()) {
        Serial.println("\nWriting to NFC tag.");
        NdefMessage message = NdefMessage();
        message.addTextRecord("Hello, Arduino!");
        message.addUriRecord("This is loop #" + String(loopcount));

        bool success = nfc.write(message);
        if (success) {
          Serial.println("\nSuccess. Try reading this tag with your phone or any tag reader.");        
        } else {
          Serial.println("\nWrite failed.");
        }
    }
    
    //Read from Tag
    if (nfc.tagPresent())
    {
        Serial.println("\nReading from tag");
        NfcTag tag = nfc.read();
        tag.print();
    }
/*
    //Erase Tag
    if (nfc.tagPresent()&&(loopcount%3==0)) {
        Serial.println("\nErasing tag.");
        bool success = nfc.erase();
        if (success) {
            Serial.println("\nSuccess, tag contains an empty record.");        
        } else {
            Serial.println("\nUnable to erase tag.");
        }

    }*/
    delay(5000);
}
