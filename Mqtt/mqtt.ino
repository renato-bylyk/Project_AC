#include <ESP8266WiFi.h>
#include <DHT.h>                               //  Humidity & Temperature sensor library
#include "Adafruit_MQTT.h"                     // 
#include "Adafruit_MQTT_Client.h"              // Mqtt library from adafruit


#define WLAN_SSID  "Your ssid"
#define WLAN_PASS  "Your password"


#define arduino_status " "                                       // STATUS  subscribe/publish
#define arduino_status_response " "                     //


#define arduino_pubsub " "       //   Arduino  subscribe/publish
#define arduino_response " "     //
#define arduino_temp " "         //  
#define arduino_hum " "          //
#define arduino_watt " "         //

#define MQTT_SERVER "test.mosca.io"                                            // MQTT Server
#define mqtt_port  1883

/************ Global State (you don't need to change this!) ******************/
WiFiClient client;
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, mqtt_port );
/****************************** Feeds ***************************************/
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, arduino_pubsub, 1);
Adafruit_MQTT_Subscribe statusreport = Adafruit_MQTT_Subscribe(&mqtt, arduino_status, 1);
/*************************** Sketch Code ************************************/


void MQTT_connect();
DHT dht(2  , DHT11, 15);                   // pin 2 on esp8266-01 (GPIO2)
char c;                                    // for the messages from the arduino
int i, isConnected = 0;                    


void setup() {
  Serial.begin(115200);
  delay(10);
  dht.begin();
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  // Setup MQTT subscription for onoff & slider feed.
  mqtt.subscribe(&onoffbutton);
  mqtt.subscribe(&statusreport);
}


// LOOP ----------------
void loop() {
  if (!mqtt.connected()) {
    MQTT_connect();
  }
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(3000))) {
    if (subscription == &onoffbutton) {
      if (strcmp((char *)onoffbutton.lastread, "connect") == 0) {
        isConnected = 1;
        mqtt.publish(arduino_response, "connected", 1);
      } else if (strcmp((char *)onoffbutton.lastread, "disconnect") == 0) {
        isConnected = 0;
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
    if (subscription == &statusreport) {
      if (strcmp((char *)statusreport.lastread, "statusreport") == 0) {
        calcWatt(4);
      }
    }
  }



  if (isConnected) {

    float h = dht.readHumidity();
    char hum[1];
    dtostrf(h, 2, 0, hum);
    mqtt.publish(arduino_hum, hum, 1);
    
    float t = dht.readTemperature();
    char temp[1];
    dtostrf(t, 2, 0, temp);
    mqtt.publish(arduino_temp, temp, 1);

    calcWatt(5);

    isConnected = 0;
  }



  // ping the server to keep the mqtt connection alive
  if (! mqtt.ping()) {
    mqtt.disconnect();
    isConnected = 0;
  }
}

void calcWatt(int mode) {

  char message[8] = "";
  i = 0;
  Serial.print("Watt");
  while (!Serial.available()) {}
  while (Serial.available()) {
    message[i++] = Serial.read();
    delay(10);
  }
  if (atof(message) >= 120 && mode == 1) {
    mqtt.publish(arduino_response, "done", 1);
  } else if (atof(message) < 120 && atof(message) > 0 && mode == 0) {
    mqtt.publish(arduino_response, "done", 1);
  } else if (atof(message) >= 120 && mode == 4) {
    mqtt.publish(arduino_status_response, "enpeton", 1);
  } else if (atof(message) < 120 && mode == 4) {
    mqtt.publish(arduino_status_response, "enpetoff", 1);
  }
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
