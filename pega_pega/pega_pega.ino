/**************************************************************************/
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>


// If using the breakout with SPI, define the pins for SPI communication.

// If using the breakout or shield with I2C, define just the pins connected
// to the IRQ and reset lines.  Use the values below (2, 3) for the shield!
#define PN532_IRQ   (7)
#define PN532_RESET (8)  // Not connected by default on the NFC Shield

// Or use this line for a breakout or shield with an I2C connection:
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

//Declara os pinos do LED
const int greenLEDPin = 5;    // LED connected to digital pin 9
const int redLEDPin = 6;     // LED connected to digital pin 10
const int blueLEDPin = 3;    // LED connected to digital pin 11

int redValue = 0; // value to write to the red LED
int greenValue = 0; // value to write to the green LED
int blueValue = 1; // value to write to the blue LED

int buzzer = 9;

int cor = 0;

int modo_jogo = 0;

long inicial = 0; // Time 1
long atual = 0;   //Time 2

int count = 0;
uint32_t card_antigo = 0;
void mudar_cor(int cor){

  switch(cor){
    case 0:
      azulFuncao();
      
      break;
    case 1:
      verdeFuncao();
      
      break;
    case 2:
      amareloFuncao();
    
      break;
    case 3:
      vermelhoFuncao();
       
      break;
    default:
     azulFuncao();
     
     break;   
  }
}

void vermelhoFuncao(){
  analogWrite(redLEDPin, 250);
  analogWrite(greenLEDPin, 0);
  analogWrite(blueLEDPin, 0);
}
void azulFuncao(){
  analogWrite(redLEDPin, 0);
  analogWrite(greenLEDPin, 0);
  analogWrite(blueLEDPin, 250);
}
void verdeFuncao(){
  analogWrite(redLEDPin, 0);
  analogWrite(greenLEDPin, 250);
  analogWrite(blueLEDPin, 0);
}

void amareloFuncao(){
  analogWrite(redLEDPin,250 );
  analogWrite(greenLEDPin,250);
  analogWrite(blueLEDPin, 0);
}

void setup(void) {
  pinMode(greenLEDPin, OUTPUT);
  pinMode(redLEDPin, OUTPUT);
  pinMode(blueLEDPin, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(buzzer,OUTPUT);
  Serial.begin(9600);
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Modulo PN53x n conectado!");
    while (1); // halt
  }
  // Set the max number of retry attempts to read from a card
  // This prevents us from waiting forever for a card, which is
  // the default behaviour of the PN532.
  nfc.setPassiveActivationRetries(0x32);
  // configure board to read RFID tags
  nfc.SAMConfig();
}


void loop(void) {
  
  Serial.println(modo_jogo);
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  if(modo_jogo == 0){
    mudar_cor(cor);
    
      
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
    
    //Change game type
    if (success) {
      // Display some basic information about the card
      nfc.PrintHex(uid, uidLength);
    
      if (uidLength == 4){
      // We probably have a Mifare Classic card ... 
        uint32_t cardid = uid[0];
        cardid <<= 8;
        cardid |= uid[1];
        cardid <<= 8;
        cardid |= uid[2];  
        cardid <<= 8;
        cardid |= uid[3];
         
        if(cardid == 14649212){
          modo_jogo = 1;
          inicial = millis();
          count = 0;
          card_antigo = 0;
        }
        if(cardid == 2790233019){
          modo_jogo = 0;
          count = 0;
          card_antigo = 0;
        }
        if(card_antigo == 0){
        card_antigo = cardid;
      }

        if(card_antigo!= cardid){
          cor++;
          count++;
          if(cor > 3){
            cor = 0;
          }
          digitalWrite(2, HIGH);   // Vibracall on
          delay(50);               // off
          digitalWrite(2, LOW);    // Vibracall off
          mudar_cor(cor);
          tone(9, 33, 100);
          delay(50);
          noTone(9);
        }
       card_antigo = cardid;
      }
     }
     if(count > 3){
        digitalWrite(2, HIGH);   // Vibracall on
        delay(200);               // off
        digitalWrite(2, LOW);    // Vibracall off
        mudar_cor(0);
        tone(9, 33, 100);
        delay(50);
        noTone(9);
     }
    Serial.print(count);
    }
  else{
    atual = millis();
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

    
    if (success) {
      nfc.PrintHex(uid, uidLength);
      if (uidLength == 4){
      // We probably have a Mifare Classic card ... 
        uint32_t cardid = uid[0];
        cardid <<= 8;
        cardid |= uid[1];
        cardid <<= 8;
        cardid |= uid[2];  
        cardid <<= 8;
        cardid |= uid[3]; 
        
        //Change game type
        if(cardid == 14649212){
          
          modo_jogo = 1;
          inicial = millis();
          count = 0;
          card_antigo = 0;
        }
        if(cardid == 2790233019){
          modo_jogo = 0;
          count = 0;
          card_antigo = 0;
        }
       
       if(card_antigo == 0){
          card_antigo = 1;
       }
       if(card_antigo != cardid){
          count++;
          digitalWrite(2, HIGH);   // Vibracall on
          delay(50);               // off
          digitalWrite(2, LOW);    // Vibracall off
          tone(9, 33, 100);
          delay(50);
          noTone(9);
          inicial = millis();
       }
       card_antigo = cardid;
      }
    
    }
      if(count < 4){
        if((atual - inicial)< 10000){
          mudar_cor(0);
        }
        if((atual - inicial)> 10000 && (atual - inicial)< 20000){
          mudar_cor(0);
        }
        else if((atual - inicial)> 20000 && (atual - inicial)< 30000){
          mudar_cor(1);
        }
        else if((atual - inicial)> 30000 && (atual - inicial)< 40000){
          mudar_cor(2);
        }
        else if((atual - inicial)> 50000){
          mudar_cor(3);

          digitalWrite(2, HIGH);   // Vibracall on
          delay(200);               // off
          digitalWrite(2, LOW);    // Vibracall off
          tone(9, 1047, 100);
          delay(100);
          noTone(9);
        }
      }
      else{
        if(count > 3){
          digitalWrite(2, HIGH);   // Vibracall on
          delay(200);               // off
          digitalWrite(2, LOW);    // Vibracall off
          mudar_cor(1);
          tone(9, 33, 100);
          delay(50);
          noTone(9);
        
        }
    }
    Serial.print(count);
  }
}
