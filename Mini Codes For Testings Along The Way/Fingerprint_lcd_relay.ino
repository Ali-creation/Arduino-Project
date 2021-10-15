
#include <Adafruit_Fingerprint.h>
#include <LiquidCrystal_I2C.h>

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

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

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// Set up the serial port to use softwareserial..
SoftwareSerial mySerial(11, 12);

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
    while (1) { delay(1); }
  }

}

void loop()                     // run over and over again
{
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
  
  fingerprintID = getFingerprintID();
  delay(50);            //don't ned to run this at full speed.

  if(fingerprintID == 1){
    lcd.setCursor(14,1);
    lcd.printByte(5);
    /*lcd.print("Correct");
    lcd.print("orrect");
    lcd.print("rrect");
    lcd.print("rect");
    lcd.print("ect");
    lcd.print("ct");
    lcd.print("t");*/
    digitalWrite(signalPin, HIGH);
    delay(5000);
    digitalWrite(signalPin, LOW); 
    fingerprintID = 0;
    lcd.clear();
  }
  /*else{
    lcd.clear();
    lcd.print("Incorrect");
    lcd.print("ncorrect");
    lcd.print("correct");
    lcd.print("orrect");
    lcd.print("rrect");
    lcd.print("rect");
    lcd.print("ect");
    lcd.print("ct");
    lcd.print("t");
    delay(1000);
  }*/
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
    /*lcd.clear();
    lcd.print("Incorrect");
    lcd.print("ncorrect");
    lcd.print("correct");
    lcd.print("orrect");
    lcd.print("rrect");
    lcd.print("rect");
    lcd.print("ect");
    lcd.print("ct");
    lcd.print("t");*/
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
