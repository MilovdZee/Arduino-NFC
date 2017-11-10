#include "SPI.h"
#include "PN532_SPI.h"
#include "emulatetag.h"
#include "NdefMessage.h"

PN532_SPI pn532spi(SPI, 10);
EmulateTag nfc(pn532spi);

uint8_t ndefBuf[120];
NdefMessage message;
int messageSize;

uint8_t uid[3] = { 0x12, 0x34, 0x56 };

void setup()
{
  Serial.begin(115200);
  Serial.println("------- Emulate Tag --------");
  
  message = NdefMessage();
  //message.addUriRecord("http://www.seeedstudio.com");
  //message.addTextRecord("{\"data\":{\"verif\":{},\"trip\":{\"000000000031\":{\"nasa\":\"38039\",\"deposit\":390,\"wqty\":\"0\",\"itemprice\":\"13,39\",\"price\":1339,\"seq\":2,\"desc\":\"HEINEKEN\",\"qty\":\"1\"},\"000000000003\":{\"wqty\":\"0\",\"itemprice\":\"1,99\",\"nasa\":\"20960\",\"seq\":1,\"qty\":\"1\",\"desc\":\"BLOEMKOOL\",\"price\":199}},\"totals\":{\"promostamps\":\"1 ruimtevaart\",\"pitemcnt\":2,\"subtotal\":\"1928\",\"totaldue\":\"1928\"},\"receipt\":{\"header\":{\"line3\":\"       Tel.: 023 - 5260170      \",\"line1\":\"        Albert Heijn 1148       \",\"line2\":\"    Soendaplein 18 - 2022 BA    \"},\"change\":\"70\",\"tender\":{\"CONTANT\":\"20,00\"},\"vat\":{\"0\":{\"amount\":\"0,00\",\"turnover\":\"0,00\"},\"TOT\":{\"amount\":\"2,43\",\"turnover\":\"12,95\"},\"210\":{\"turnover\":\"11,07\",\"amount\":\"2,32\"},\"60\":{\"amount\":\"0,11\",\"turnover\":\"1,88\"}},\"footer\":{\"line1\":\"         Wij denken mee         \",\"line2\":\"       met uw portemonee.       \",\"line3\":\"           Dat helpt.           \"},\"trx\":{\"trx\":\"80\",\"date\":\"01-11-2017\",\"dt\":\"23:44\",\"oper\":\"1\",\"lane\":\"1\",\"store\":\"1148\"}},\"seq\":3},\"cmd\":\"RECEIPT\"}");
  message.addTextRecord("{\"data\":{\"verif\":{},\"trip\":{\"000000000031\":{\"nasa\":\"38039\",\"deposit\":390,\"wqty\":\"0\",\"itemprice");
  messageSize = message.getEncodedSize();
  if (messageSize > sizeof(ndefBuf)) {
      Serial.println("ndefBuf is too small");
      while (1) { }
  }
  
  Serial.print("Ndef encoded message size: ");
  Serial.println(messageSize);

  message.encode(ndefBuf);
  
  // comment out this command for no ndef message
  nfc.setNdefFile(ndefBuf, messageSize);
  
  // uid must be 3 bytes!
  nfc.setUid(uid);
  
  nfc.init();
}

void loop(){
    // uncomment for overriding ndef in case a write to this tag occured
    //nfc.setNdefFile(ndefBuf, messageSize); 
    
    // start emulation (blocks)
    Serial.println("Emulating...");
    nfc.emulate();
    Serial.println("Emulate finished?");
        
    // or start emulation with timeout
//    if(!nfc.emulate(1000)){ // timeout 1 second
//      Serial.println("timed out");
//    }
    
    // deny writing to the tag
    // nfc.setTagWriteable(false);
    
    if(nfc.writeOccured()){
       Serial.println("\nWrite occured !");
       uint8_t* tag_buf;
       uint16_t length;
       
       nfc.getContent(&tag_buf, &length);
       NdefMessage msg = NdefMessage(tag_buf, length);
       msg.print();
    }

    delay(100);
}
