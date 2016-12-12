#include <SoftwareSerial.h>
#include <Arduino.h>
#include <MideaHeatpumpIR.h>

SoftwareSerial esp12(4, 5); //RX,TX
IRSenderPWM irSender(3);
HeatpumpIR *heatpumpIR = new MideaHeatpumpIR();

char c;
boolean gotMessage = false;
String message;
int temp_value = 19, mode_value = 2, fanspeed_value = 0, swing_value = 0, ac_value ;
char AC[1], TP[1], MD[1], SM[1], FS[1];

void setup() {

  Serial.begin(115200);
  esp12.begin(115200);
  pinMode(13, OUTPUT);
}

void loop() {
  message = "";
  while (esp12.available()) {
    gotMessage = true;
    c = esp12.read();
    message += c;
    delay(10);
  }
  if (gotMessage) {
    switch (message[0]) {
      case 'A':
        AC[0] = message[2];
        AC[1] = '\0';
        AC[2] = '\0';
        ac_value = atoi(AC);
        break;
      case 'M':
        MD[0] = message[2];
        MD[1] = '\0';
        MD[2] = '\0';
        mode_value = atoi(MD);
        break;
      case 'F':
        FS[0] = message[2];
        FS[1] = '\0';
        FS[2] = '\0';
        fanspeed_value = atoi(FS);
        break;
      case 'S':
        SM[0] = message[2];
        SM[1] = '\0';
        SM[2] = '\0';
        swing_value = atoi(SM);
        break;
      case 'T':
        TP[0] = message[2];
        TP[1] = message[3];
        temp_value = atoi(TP);
        break;
      default :
        gotMessage=false;
        }
  }
  if (gotMessage) {
    gotMessage = false;
    heatpumpIR->send(irSender, ac_value, mode_value, fanspeed_value , temp_value , swing_value, HDIR_AUTO);
  }
}
