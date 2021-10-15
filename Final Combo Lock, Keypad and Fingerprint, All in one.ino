
#include <Adafruit_Fingerprint.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h> 
#include <Keypad.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#define BMP_SCK 52 //SCL 13  52
#define BMP_MISO 50 //SDO 12  50
#define BMP_MOSI 51 //SDA 11   51
#define BMP_CS 53 //CSB 10   53

Adafruit_BMP280 bme; // I2C
//Adafruit_BMP280 bme(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);

#define Password_Length 7 

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

int signalPinBpm = 11;
int signalPinValve = 12;
int signalPin = 13;
int fingerprintID = 0;

uint8_t bell[8]  = {0x4,0xe,0xe,0xe,0x1f,0x0,0x4};
uint8_t note[8]  = {0x2,0x3,0x2,0xe,0x1e,0xc,0x0};
uint8_t clock[8] = {0x0,0xe,0x15,0x17,0x11,0xe,0x0};
uint8_t heart[8] = {0x0,0xa,0x1f,0x1f,0xe,0x4,0x0};
uint8_t duck[8]  = {0x0,0xc,0x1d,0xf,0xf,0x6,0x0};
uint8_t check[8] = {0x0,0x1,0x3,0x16,0x1c,0x8,0x0};
uint8_t cross[8] = {0x0,0x1b,0xe,0x4,0xe,0x1b,0x0};
uint8_t retarrow[8] = {  0x1,0x1,0x5,0x9,0x1f,0x8,0x4};

char Data[Password_Length]; 
char Master[Password_Length] = "123456"; 
byte data_count = 0, master_count = 0, hmls = 9;
bool Pass_is_good;
char customKey;
char customKey2;

const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// Set up the serial port to use softwareserial..
//SoftwareSerial mySerial(11, 12);

#else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
#define mySerial Serial1
      
#endif

LiquidCrystal_I2C lcd(0x27,16,2);


Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setup()
{
  lcd.init(); 
  lcd.backlight();
  pinMode(signalPin, OUTPUT);
  pinMode(signalPinBpm, OUTPUT);
  pinMode(signalPinValve, OUTPUT);
  lcd.createChar(5, check);
  lcd.createChar(6, cross);
  //Fingerprint sensor module setup
  Serial.begin(9600);
  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    //while (1) { delay(1); }
  }
  //Sensor Involvement BMP280
  Serial.println(F("BMP280 test"));
  
  if (!bme.begin(0x76)) {  
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }
}

void loop()                     // run over and over again
{
  //BMP280
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");
    
  Serial.print("Pressure = ");
  Serial.print(bme.readPressure());
  Serial.println(" Pa");
  
  //LCD 
  lcd.setCursor(0,0);
  lcd.print("Password:");
  lcd.print("assword:");
  lcd.print("ssword:");
  lcd.print("sword:");
  lcd.print("word:");
  lcd.print("ord:");
  lcd.print("rd:");
  lcd.print("d:");
  lcd.print(":");
  lcd.setCursor(0,1);
  lcd.print("Fingerprint:");
  lcd.print("ingerprint:");
  lcd.print("ngerprint:");
  lcd.print("gerprint:");
  lcd.print("erprint:");
  lcd.print("rprint:");
  lcd.print("print:");
  lcd.print("rint:");
  lcd.print("int:");
  lcd.print("nt:");
  lcd.print("t:");
  lcd.print(":");

   
  //Pump takes action
  customKey = customKeypad.getKey();
  
  if(customKey == '*'){
    while(bme.readPressure() > 95000){
      customKey2 = customKeypad.getKey();
      if(customKey2 == '#' ){
        digitalWrite(signalPinBpm, LOW);
        break;
      }
      digitalWrite(signalPinBpm, HIGH);
      Serial.print("Temperature = ");
      Serial.print(bme.readTemperature());
      Serial.println(" *C");
      
      Serial.print("Pressure = ");
      Serial.print(bme.readPressure());
      Serial.println(" Pa");
    }
  }
  
  if (customKey){
    switch(customKey){
      case '*':
        break;
      default:
        Data[data_count] = customKey;
        lcd.setCursor(hmls,0); 
        lcd.print(Data[data_count]); 
        data_count++; 
        hmls++;
        break;   
      }
    }

  if(data_count == Password_Length-1){

    if(!strcmp(Data, Master)){
      lcd.setCursor(9,0);
      lcd.print(" ");
      lcd.print(" ");
      lcd.print(" ");
      lcd.print(" ");
      lcd.print(" ");
      lcd.print(" ");
          
      lcd.setCursor(10,0);
      lcd.printByte(5);
      //Valve working
      digitalWrite(signalPinValve, HIGH);
      delay(5000);
      digitalWrite(signalPinValve, LOW);
      //Open locks
      digitalWrite(signalPin, HIGH);
      delay(5000);
      digitalWrite(signalPin, LOW);
      lcd.clear(); 
      }
    else{
      lcd.setCursor(9,0);
      lcd.print(" ");
      lcd.print(" ");
      lcd.print(" ");
      lcd.print(" ");
      lcd.print(" ");
      lcd.print(" ");
      lcd.setCursor(10,0);
      lcd.printByte(6);
      delay(3000);
      lcd.clear();
      }
    
    
    clearData();  
  }
  
  fingerprintID = getFingerprintID();
  delay(50);            //don't ned to run this at full speed.

  if(fingerprintID == 1 || fingerprintID == 5 || fingerprintID == 10){
    lcd.setCursor(14,1);
    lcd.printByte(5);
    //Vlave working
    digitalWrite(signalPinValve, HIGH);
    delay(5000);
    digitalWrite(signalPinValve, LOW);
    //Open Locks
    digitalWrite(signalPin, HIGH);
    delay(5000);
    digitalWrite(signalPin, LOW); 
    fingerprintID = 0;
    lcd.clear();
  }
  Serial.println();
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    lcd.setCursor(14,1);
    lcd.printByte(6);
    delay(3000);
    lcd.clear();
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}

void clearData(){
  while(data_count !=0){
    hmls--;
    Data[data_count--] = 0; 
  }
  return;
}
