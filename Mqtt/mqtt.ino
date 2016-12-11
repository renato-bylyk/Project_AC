#include <ESP8266WiFi.h>
#include <DHT.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"


#define WLAN_SSID  ""
#define WLAN_PASS  ""

#define arduino_pubsub " #1 SUBSCRIPTION"         //
#define arduino_response " #1 RESPONSE"           //
#define arduino_temp "#2 PUBLISH "                //  PUB-SUB
#define arduino_hum "#2 PUBLISH"                  //
#define arduino_watt "#2 PUBLISH"                 //

#define MQTT_SERVER "MQTT"                                            // MQTT Server
#define mqtt_port  1883


void MQTT_connect();
/************ Global State (you don't need to change this!) ******************/
WiFiClient client;
DHT dht(2  , DHT11, 15);


const char* host = "";
String urlon = "STRING FOR ON";
String urloff = "STRING FOR OFF";
long previousMillis = 0;
long interval = 60000*5;   // 5 minutes delay 
boolean gotMessage = false;
char c;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, mqtt_port );
/****************************** Feeds ***************************************/
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, arduino_pubsub, 1);
//Adafruit_MQTT_Subscribe statusreport = Adafruit_MQTT_Subscribe(&mqtt, arduino_status, 1);
/*************************** Sketch Code ************************************/


void setup() {
  Serial.begin(115200);
  delay(10);
  dht.begin();                                            // Humidity-Temp Sensor
  WiFi.begin(WLAN_SSID, WLAN_PASS);                       // Start wifi
  while (WiFi.status() != WL_CONNECTED) {                 // Connect
    delay(500);
  }
  // Setup MQTT subscription for onoff & slider feed.
  mqtt.subscribe(&onoffbutton);                           // Subscription #1
  //mqtt.subscribe(&statusreport);                        // Subscription #2
}


// LOOP ----------------
void loop() {

  if (!mqtt.connected()) {                                // connect to broker
    MQTT_connect();
  }

  Adafruit_MQTT_Subscribe *subscription;                  // Subscribe again

  while ((subscription = mqtt.readSubscription(3000))) {  // if message arrived
    if (subscription == &onoffbutton) {
      if (strcmp((char *)onoffbutton.lastread, "connect") == 0) {
        mqtt.publish(arduino_response, "connected", 1);
        iscon(5);
      } else if (strcmp((char *)onoffbutton.lastread, "disconnect") == 0) {
      } else if (strcmp((char *)onoffbutton.lastread, "AC1") == 0) {
        Serial.print("AC1");
        delay(50);
        calcWatt(1);
      } else if (strcmp((char *)onoffbutton.lastread, "AC0") == 0) {
        Serial.print("AC0");
        delay(50);
        calcWatt(0);
      } else if (!(strcmp((char *)onoffbutton.lastread, "done") == 0)) {
        Serial.print((char *)onoffbutton.lastread);
        mqtt.publish(arduino_response, "done", 1);
      }
    }
  }

  // ping the server to keep the mqtt connection alive
  if (! mqtt.ping()) {
    mqtt.disconnect();
  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    Serial.println("Success");
    previousMillis = currentMillis;
    calcWatt(4);
  }
}




void iscon(int mode) {
  float h = dht.readHumidity();
  char hum[1];
  dtostrf(h, 2, 0, hum);
  mqtt.publish(arduino_hum, hum, 1);

  float t = dht.readTemperature();
  char temp[1];
  dtostrf(t, 2, 0, temp);
  mqtt.publish(arduino_temp, temp, 1);

  calcWatt(mode);

}

// 5 - genika
// 4 - bash update
// 1,0 control panel

void calcWatt(int mode) {
  int i = 0;
  char message[8] = "";
  Serial.print("Watt");
  //while (!Serial.available()) {}
  while (Serial.available()) {
    message[i++] = Serial.read();
    delay(10);
  }
  if (atof(message) >= 120 && mode == 1) {
    mqtt.publish(arduino_response, "done", 1);
  } else if (atof(message) < 120 && atof(message) > 0 && mode == 0) {
    mqtt.publish(arduino_response, "done", 1);
  } else if (atof(message) >= 120 && mode == 4) {                                     //
    client.connect(host, 80);                                                         //
    client.print(String("GET ") + urlon + " HTTP/1.1\r\n" +                           //                    
               "Host: " + host + "\r\n" +                                             //
               "Connection: close\r\n\r\n");                                          //
  } else if (atof(message) < 120 && mode == 4) {                                      //  UPDATE THE DATABASE SO WE CAN CHECK WHICH AC
    client.connect(host, 80);                                                         //   IS ON AND WHICH ONE IS CLOSED.
    client.print(String("GET ") + urloff + " HTTP/1.1\r\n" +                          //
               "Host: " + host + "\r\n" +                                             //
               "Connection: close\r\n\r\n");                                          //
  }                                                                                   //
  if ( mode != 4) {
    delay(10);
    mqtt.publish(arduino_watt, message, 1);
  }
   
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;
  uint8_t retries = 5;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
}
