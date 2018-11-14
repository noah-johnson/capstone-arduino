//NFC Reader includes
#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>
#include <NfcTag.h>

PN532_SPI pn532spi(SPI, 7);
NfcAdapter nfc = NfcAdapter(pn532spi);

//LoRa Tx Reader includes
#include <RH_RF95.h>
 
#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0
 
// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

int loopcount = 0;


void setup() {
  //LoRaInit();

  //Serial.println("setup complete, hopefully...");
}

void NFCInit(){
    // put your setup code here, to run once:
  //NFC Init
  Serial.begin(9600);
  Serial.println("NDEF Writer");
  nfc.begin();
}

void LoRaInit(){
  
  //LoRa Init
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
 
  while (!Serial);
  Serial.begin(9600);
  delay(100);
 
  Serial.println("Arduino LoRa TX Test!");
 
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
 
  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");
 
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
 
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}
 
int16_t packetnum = 0;  // packet counter, we increment per xmission

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("\nThis is loop #"+ String(loopcount));
  //NFC Loop
  //used to keep track of number of cycles
  loopcount++;
  //NFCWriteRead();
  LoRaInit();
  LoRaTx();
  delay(50);
  NFCInit();
  NFCWriteRead();
  delay(5000);
  
    
}

void NFCWriteRead(){
      //Write to Tag
    Serial.println("\nPlace a formatted Mifare Classic NFC tag on the reader.");
    if (nfc.tagPresent()) {
        Serial.println("\nWriting to NFC tag.");
        NdefMessage message = NdefMessage();
        /*NfcTag tag = nfc.read();
        NdefMessage storedMessage = tag.getNdefMessage();
        message.addUriRecord(storedMessage.getRecord());*/
        message.addTextRecord("Hello, Arduino!");
        message.addUriRecord("This is loop #" + String(loopcount));
        
        bool success = nfc.write(message);
        if (success) {
          Serial.println("\nSuccess. Try reading this tag with your phone or any tag reader.");        
        } else {
          Serial.println("\nWrite failed.");
        }
    } else {
      Serial.println("\nTag not present, check again.");
    }
    
    //Read from Tag
    if (nfc.tagPresent())
    {
        Serial.println("\nReading from tag");
        NfcTag tag = nfc.read();
        tag.print();
    }

    //get data from tag
  //
  //
  //
    if (nfc.tagPresent())
    {
      Serial.println("\nGetting data from tag");
      NfcTag tag = nfc.read();
      String data[] = tag.getData();
    }

}

void LoRaTx(){
      Serial.println("Sending to rf95_server");
  // Send a message to rf95_server
  
  char radiopacket[20] = "Hello World #      ";
  itoa(packetnum++, radiopacket+13, 10);
  Serial.print("Sending "); Serial.println(radiopacket);
  radiopacket[19] = 0;
  
  Serial.println("Sending..."); delay(10);
  rf95.send((uint8_t *)radiopacket, 20);
 
  Serial.println("Waiting for packet to complete..."); delay(10);
  rf95.waitPacketSent();
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
 
  Serial.println("Waiting for reply..."); delay(10);
  if (rf95.waitAvailableTimeout(1000))
  {
    // Should be a reply message for us now   
    if (rf95.recv(buf, &len))
   {
      Serial.print("Got reply: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);    
    }
    else
    {
      Serial.println("Receive failed");
    }
  }
  else
  {
    Serial.println("No reply, is there a listener around?");
  }
  
}
