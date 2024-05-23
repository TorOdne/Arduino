// C++ code
//
#include <MKRWAN.h>
#include <Servo.h>
Servo myServo1;
Servo myServo2;
const int MOTOR1 = 2;
const int MOTOR2 = 3;
const int LED = 4;
const int POT1 = A0;
const int POT2 = A1;
const int PHOTOS = A2;
int angle1 = 90;
int angle2 = 90;
int reset = 0;
int anglemoved1 = 0;
int anglemoved2 = 0;
int time = 0;
int moved = 0;
LoRaModem modem;

#include "arduino_secrets.h"
String appEui = SECRET_APP_EUI;
String appKey = SECRET_APP_KEY;

void setup()

{
  if (!modem.begin(EU868)) {
    Serial.println("Failed to start module");
    while (1) {}
  };
  Serial.print("Your module version is: ");
  Serial.println(modem.version());
  Serial.print("Your device EUI is: ");
  Serial.println(modem.deviceEUI());
  int Reset = 0;
  int connected = modem.joinOTAA(appEui, appKey);
  if (!connected) {
    Serial.println("Something went wrong; are you indoor? Move near a window and retry");
    while (1) {}
  }
  
  pinMode(POT1, INPUT);
  pinMode(POT2, INPUT);
  pinMode(PHOTOS, INPUT);
  pinMode(LED, OUTPUT);
  myServo1.attach(MOTOR1);
  myServo2.attach(MOTOR2);
  myServo1.write(90);
  myServo2.write(90);
  Serial.begin(9600);
  delay(500);
  modem.minPollInterval(60);
  
}

void loop()
{
    if (reset == 1) {
    delay(60000);
    myServo1.write(90);
    myServo2.write(90);
    angle1 = 90;
    angle2 = 90;
    anglemoved1 = 0;
    anglemoved2 = 0;
    moved = 0;
	  digitalWrite(LED, LOW);
    Serial.println("Reset complete");
    delay(1000);
    reset = 0;
  }
  

  int analP1 = analogRead(POT1);
  int analP2 = analogRead(POT2);
  int analPS = analogRead(PHOTOS);
  int AP1 = map(analP1, 0, 1023, 0, 100);
  int AP2 = map(analP2, 0, 1023, 0, 100);
  int PS = map(analPS, 0, 1023, 0, 100);
  if (moved == 1) {
    time += 1;
  } 
  if (AP1 < 40 && angle1 != 0) {
    angle1 -= 1;
    myServo1.write(angle1);
    anglemoved1 += 1;
    if (moved == 0) {
      moved = 1;
    }
  } else if (AP1 > 60 && angle1 != 180) {
    angle1 += 1;
    myServo1.write(angle1);
    anglemoved1 += 1;
    if (moved == 0) {
      moved = 1;
    }
  }
  
    
  if (AP2 < 40 && angle2 != 0) {
    angle2 -= 1;
    myServo2.write(angle2);
    anglemoved2 += 1;
    if (moved == 0) {
      moved = 1;
    }
  } else if (AP2 > 60 && angle2 != 180) {
    angle2 += 1;
    myServo2.write(angle2);
    anglemoved2 += 1;
    if (moved == 0) {
      moved = 1;
    }
  }


  
  if (PS < 30) {
    int timemoved = time / 100;
    String Space = " ";
    String msgTemp = String(timemoved);
    String msgTempm = String(anglemoved2);
    String msg = String(anglemoved1); // Convert integer to string and assign it to msg
//      std::string str = std::to_string(anglemoved);
      msg =anglemoved1; //send value
      msg += Space;
      msg += msgTempm;
      msg += Space;
      msg += msgTemp;
  reset = 1;
  digitalWrite(LED, HIGH);
  Serial.println("Resetting!");
  Serial.println(PS);
  Serial.println();
  Serial.print("Sending: " + msg + " - ");
  for (unsigned int i = 0; i < msg.length(); i++) {
    Serial.print(msg[i] >> 4, HEX);
    Serial.print(msg[i] & 0xF, HEX);
    Serial.print(" ");
  }
  Serial.println();

  int err;
  modem.beginPacket();
  modem.print(msg);
  err = modem.endPacket(true);
  if (err > 0) {
    Serial.println("Message sent correctly!");
  } else {
    Serial.println("Error sending message :(");
    Serial.println("(you may send a limited amount of messages per minute, depending on the signal strength");
    Serial.println("it may vary from 1 message every couple of seconds to 1 message every minute)");
  }
  delay(10000);
  if (!modem.available()) {
    Serial.println("No downlink message received at this time.");
    return;
  }
  char rcv[64];
  int i = 0;
  while (modem.available()) {
    rcv[i++] = (char)modem.read();
  }
  Serial.print("Received: ");
  for (unsigned int j = 0; j < i; j++) {
    Serial.print(rcv[j] >> 4, HEX);
    Serial.print(rcv[j] & 0xF, HEX);
    Serial.print(" ");
  }
  Serial.println();
  }
  delay(10);

  }