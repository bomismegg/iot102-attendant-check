#include <SPI.h>
#include <Wire.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_Fingerprint.h>
#include <LiquidCrystal.h>
#define Finger_Rx D5
#define Finger_Tx D6

SoftwareSerial mySerial(Finger_Rx, Finger_Tx);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
const char *ssid = "BomUisme";
const char *password = "minhtrig8ntp";
String postData ;
String link = "http://192.168.0.29/biometricattendance/getdata.php";
int FingerID = 0;
uint8_t id;
LiquidCrystal lcd(D8, D7, D4, D3, D2, D1);
// 12, 11, 5, 4, 3, 2

void setup() {
  lcd.begin(16,2);
  Serial.begin(115200);

  connectToWiFi();
  finger.begin(57600);
  Serial.println("\n\nAdafruit finger detect test");

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) {
      delay(1);
    }
  }

  finger.getTemplateCount();
  Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  Serial.println("Waiting for valid finger...");
}
void loop() {

  //check if there's a connection to WiFi or not
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }
  FingerID = getFingerprintID();
  delay(50);
  if (FingerID > 0) {
    SendFingerprintID( FingerID );
  }
  ChecktoAddID();
  ChecktoDeleteID();
}
void SendFingerprintID( int finger ) {

  WiFiClient client;
  HTTPClient http;
  postData = "FingerID=" + String(finger);
  http.begin(client, link);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int httpCode = http.POST(postData);   //Send the request
  String payload = http.getString();    //Get the response payload

  Serial.println(httpCode);   //Print HTTP return code
  Serial.println(payload);    //Print request response payload
  Serial.println(postData);   //Post Data
  Serial.println(finger);     //Print fingerprint ID

  if (payload.substring(0, 5) == "login") {
    String user_name = payload.substring(5);
    Serial.println(user_name);
    lcd.clear();
//    lcd.setCursor(0, 0);
    lcd.print("Welcome");
    lcd.setCursor(0, 1);
    lcd.print(user_name);
  }
  else if (payload.substring(0, 6) == "logout") {
    String user_name = payload.substring(6);
    Serial.println(user_name);
    lcd.clear();
//    lcd.setCursor(0, 0);
    lcd.print("Good bye");
    lcd.setCursor(0,1);
    lcd.print(user_name);
  }
  delay(2000);
  lcd.clear();
  postData = "";
  http.end();  //Close connection
}
//********************Get the Fingerprint ID******************
int getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return 0;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return -2;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return -2;
    default:
      Serial.println("Unknown error");
      return -2;
  }
  // OK success!
  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return -1;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return -2;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return -2;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return -2;
    default:
      Serial.println("Unknown error");
      return -2;
  }
  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return -2;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return -1;
  } else {
    Serial.println("Unknown error");
    return -2;
  }
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

  return finger.fingerID;
}
//******************Check if there a Fingerprint ID to delete******************
void ChecktoDeleteID() {

  WiFiClient client;
  HTTPClient http;    //Declare object of class HTTPClient
  //Post Data
  postData = "DeleteID=check"; // Add the Fingerprint ID to the Post array in order to send it
  // Post methode

  http.begin(client, link); //initiate HTTP request, put your Website URL or Your Computer IP
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header

  int httpCode = http.POST(postData);   //Send the request
  String payload = http.getString();    //Get the response payload

  if (payload.substring(0, 6) == "del-id") {
    String del_id = payload.substring(6);
    Serial.println(del_id);
    deleteFingerprint( del_id.toInt() );
  }

  http.end();  //Close connection
}
//******************Delete Finpgerprint ID*****************
uint8_t deleteFingerprint( int id) {
  uint8_t p = -1;

  p = finger.deleteModel(id);

  if (p == FINGERPRINT_OK) {
    Serial.println("Deleted!");
    lcd.clear();
//    lcd.setCursor(0, 0);
    lcd.print("Deleted");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    lcd.clear();
//    lcd.setCursor(0, 0);
    lcd.print("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not delete in that location");
    lcd.clear();
//    lcd.setCursor(0, 0);
    lcd.print("Could not delete");
    lcd.setCursor(0,1);
    lcd.print("in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    lcd.clear();
//    lcd.setCursor(0, 0);
    lcd.print("Error writing");
    lcd.setCursor(0,1);
    lcd.print("to flash");
    return p;
  } else {
    Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Unknown error: 0x");
    return p;
  }
  return p;

}
//******************Check if there a Fingerprint ID to add******************
void ChecktoAddID() {

  WiFiClient client;
  HTTPClient http;    //Declare object of class HTTPClient
  //Post Data
  postData = "Get_Fingerid=get_id"; // Add the Fingerprint ID to the Post array in order to send it
  // Post methode

  http.begin(client, link); //initiate HTTP request, put your Website URL or Your Computer IP
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header

  int httpCode = http.POST(postData);   //Send the request
  String payload = http.getString();    //Get the response payload

  if (payload.substring(0, 6) == "add-id") {
    String add_id = payload.substring(6);
    Serial.println(add_id);
    id = add_id.toInt();
    getFingerprintEnroll();
  }
  http.end();  //Close connection
}
//******************Enroll a Finpgerprint ID*****************
uint8_t getFingerprintEnroll() {

  int p = -1;
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        lcd.clear();
        lcd.print("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.println(".");
        lcd.clear();
        lcd.print("No finger");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
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
      lcd.clear();
//      lcd.setCursor(0, 0);
      lcd.print("OK");
      break;
    case FINGERPRINT_IMAGEMESS:
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
//  lcd.setCursor(0, 0);
  lcd.print("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        lcd.clear();
//        lcd.setCursor(0, 0);
        lcd.print("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.println(".");
        lcd.clear();
//        lcd.setCursor(0, 0);
        lcd.print("No finger");
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
      lcd.clear();
//      lcd.setCursor(0, 0);
      lcd.print("Image converted");
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
  Serial.print("Creating model for #");  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
    lcd.clear();
//    lcd.setCursor(0, 0);
    lcd.print("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
    lcd.clear();
//    lcd.setCursor(0, 0);
    lcd.print("Stored");
    confirmAdding();
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }
  return p;
}
//******************Check if there a Fingerprint ID to add******************
void confirmAdding() {

  WiFiClient client;
  HTTPClient http;    //Declare object of class HTTPClient
  //Post Data
  postData = "confirm_id=" + String(id); // Add the Fingerprint ID to the Post array in order to send it
  // Post methode

  http.begin(client, link); //initiate HTTP request, put your Website URL or Your Computer IP
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header

  int httpCode = http.POST(postData);   //Send the request
  String payload = http.getString();    //Get the response payload
  lcd.clear();
//  lcd.setCursor(0, 0);
  lcd.print(payload);
  delay(1000);
  Serial.println(payload);


  http.end();  //Close connection
  return;
}
//********************connect to the WiFi******************
void connectToWiFi() {
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  lcd.clear();
//  lcd.setCursor(0, 0);
  lcd.print("Connecting to ");
  lcd.setCursor(1, 0);
  lcd.print(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected");
  lcd.clear();
//  lcd.setCursor(0, 0);
  lcd.print("Connected, IP: ");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP

}
