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
String data;
char nfcPacket[120];

void setup() {
}

void NFCInit(){
  // put your setup code here, to run once:
  //NFC Init
  Serial.begin(9500);
  Serial.println("NDEF Writer");
  nfc.begin();
}

void LoRaInit(){
  
  //LoRa Init
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
 
  while (!Serial);
  Serial.begin(9500);
  delay(100);
 
  Serial.println("LoRa TX:");
 
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
 
  while (!rf95.init()) {
    Serial.println("failed");
    while (1);
  }
  Serial.println("OK!");
 
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("Freq. Fail");
    while (1);
  }
  //Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
 
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}
 
int16_t packetnum = 0;  // packet counter, we increment per xmission

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("\nLoop #" + String(loopcount));
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
    Serial.println("Place tag");
    if (nfc.tagPresent()) {
        Serial.println("Writing to tag.");
        NdefMessage message = NdefMessage();

        message.addUriRecord("Hello, Arduino!");
        message.addUriRecord("Loop no." + String(loopcount));
        message.addUriRecord("Random comment to take up some space");
        
        bool success = nfc.write(message);
        if (success) {
          Serial.println("Success.");        
        } else {
          Serial.println("Write Fail");
        }
    } else {
      Serial.println("\nTag not present");
    }
    
    //Read from Tag
    if (nfc.tagPresent())
    {
        Serial.println("Reading tag");
        NfcTag tag = nfc.read();
        tag.print();
    }

    //get data from tag
  //
  //
  //
    if (nfc.tagPresent())
    {
      Serial.println("\nGetting data");
      NfcTag tag = nfc.read();
      data = tag.getData();
//      while (data.length()<120)
//      {
//        data +=  " "; 
//      }
      Serial.println("Data: ");
      Serial.println(data);
      data.toCharArray(nfcPacket, 120);
      //nfcPacket = data;
    }
    
}

void LoRaTx(){
      Serial.println("Sending to rf95_server");
  // Send a message to rf95_server
  //char radiopacket[20] = "Hello World #      ";
  //char nfcPacket[256] = data;
  //nfcPacket = data;

  //char nfcPacket[];

  
  itoa(packetnum++, nfcPacket+13, 10);
  Serial.print("packet to send:"); 
  for (int i = 0; i < 120; i++)
  {
  Serial.print(nfcPacket[i]);
  }
//  radiopacket[19] = 0;
  
  Serial.println("Sending..."); delay(10);
  rf95.send((uint8_t *)nfcPacket, 120);
 
  Serial.println("Waiting for packet to complete..."); delay(10);
  rf95.waitPacketSent();
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
 
  Serial.println("Waiting..."); delay(10);
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
      Serial.println("Receive Fail");
    }
  }
  else
  {
    Serial.println("No reply");
  }
  
}
