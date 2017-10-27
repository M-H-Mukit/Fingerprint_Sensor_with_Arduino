#include <LiquidCrystal.h>


#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>



#include "Wire.h"
#include "Adafruit_LiquidCrystal.h"

LiquidCrystal lcd(5, 4, 6,7,8,9);

uint8_t id=0;

uint8_t getFingerprintEnroll();


SoftwareSerial mySerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);


String names[5] ;

void setup()  
{
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(500);

  Serial.begin(9600);
  Serial.println("Adafruit Fingerprint sensor enrollment");
  lcd.begin(16, 2);
  lcd.print("WELCOME!");

  // set the data rate for the sensor serial port
  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } 
  else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1);
  }
}



void loop()                     // run over and over again
{
  Serial.println("\n\n\nEnroll or search ( e / s ) ?");
  lcd.clear();
  lcd.print("Enrl/srch(e/s)");

  selectOperation();
}
//// -- editing default code here -----------
boolean searching;
void selectOperation ()
{
  searching = true;
  while (1) {
    while (! Serial.available());
    char c = Serial.read();
    Serial.println(c);
    if ( c== 'e'){
      enroll();
      break;
    }
    if ( c== 's')
    {
      search(); 
      break;
    }
  } 
}
void search()
{
  Serial.println("Waiting for valid finger...");
  
  lcd.clear();
  lcd.print("Put your finger");
  
 
  while (searching)
  {
    getFingerprintIDez();
    delay(50); 
  }
}

void enroll ()
{
  id = readnumber();
  setNames();
  Serial.print("Enrolling ID #  ");

  lcd.clear();
  lcd.print("Enrolling...");

  Serial.println(id);
  Serial.print("Name #  ");
  Serial.println(names[id]);
  while(1){
    while (!  getFingerprintEnroll() );
    
    lcd.clear();
    lcd.print("Stored!");
    lcd.setCursor(0,1);  
    lcd.print("ID:");
    lcd.setCursor(3,1);  
    lcd.print(id);
    lcd.setCursor(5,1);  
    lcd.print("Name: ");
    lcd.setCursor(11,1);  
    lcd.print(names[id]);
    delay (5000);

    break;
  }

}


uint8_t readnumber(void) {
  Serial.println("Enter the ID #");
  lcd.clear();
  lcd.print("ID: ");
  uint8_t num = 0;
  boolean validnum = false; 
  while (1) {
    while (! Serial.available());
    char c = Serial.read();
    if (isdigit(c)) {
      num *= 10;
      num += c - '0';
      validnum = true;

    } 
    else if (validnum) {
      lcd.setCursor(6,0);
      lcd.print(num);
      return num;
    }
  }
}

void setNames()
{

  Serial.println("Input the name");
  lcd.setCursor(0,1);
  lcd.print("Name: ");
  while (1) {
    while (! Serial.available());
    String n = Serial.readString();
    names[id] = n;

    lcd.print(n);
    delay(2000);
    lcd.setCursor(0,0);


    break;
  } 
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
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } 
  else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } 
  else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } 
  else {
    Serial.println("Unknown error");
    return p;
  }   

  // found a match!
  Serial.print("Found ID #"); 
  Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); 
  Serial.println(finger.confidence); 
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
  Serial.print("ID #"); 
  Serial.println(finger.fingerID);
  
  lcd.clear();
  lcd.print("Searching ...");
  delay(2000);
  lcd.clear();
  lcd.print("ID: ");
  lcd.setCursor(5,0);
  lcd.print(finger.fingerID);
  
  Serial.print("Name #"); 
  Serial.println(names[id]);
  
  lcd.setCursor(0,1);
  lcd.print("Name: ");
  lcd.setCursor(7,1);
  lcd.print(names[finger.fingerID]);
  
  delay(10000);

  Serial.print(" with confidence of "); 
  Serial.println(finger.confidence);

  searching = false;


  return finger.fingerID; 
}



uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); 
  Serial.println(id);

  lcd.clear();
  lcd.print("Place Finger");

  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
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

  Serial.println("Remove finger");

  lcd.clear();
  lcd.print("Remove finger");

  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); 
  Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");


  lcd.clear();
  lcd.print("Place Again");

  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
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
  Serial.print("Creating model for #");  
  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } 
  else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } 
  else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } 
  else {
    Serial.println("Unknown error");
    return p;
  }   

  Serial.print("ID "); 
  Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");

    Serial.print("ID: ");
    Serial.println(id);
    Serial.print("Name: ");
    Serial.println(names[id]);


  } 
  else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } 
  else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } 
  else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } 
  else {
    Serial.println("Unknown error");
    return p;
  }   
}






























