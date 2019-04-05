#include <FS.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>
#include <SoftwareSerial.h>
#include <DNSServer.h>
#include <IotWebConf.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include "Definitions.h"
#include "Atlas_ezo.h"

//Use WiFiClientSecure class to create TLS connection
//WiFiClient client;
WiFiClientSecure client;
WiFiClient espClient;

// -- Initial name of the Thing. Used e.g. as SSID of the own Access Point.
const char thingName[] = "EZO_SENSORS_AP";

// -- Initial password to connect to the Thing, when it creates an own Access Point.
const char wifiInitialApPassword[] = "ezo_sensors_password";

DNSServer dnsServer;
WebServer server(80);

IotWebConf iotWebConf(thingName, &dnsServer, &server, wifiInitialApPassword);
String line;


Atlas_ezo ezo_rtd(EZO_RTD_ADDR);
Atlas_ezo ezo_ec(EZO_EC_ADDR);
Atlas_ezo ezo_ph(EZO_PH_ADDR);
Atlas_ezo ezo_orp(EZO_ORP_ADDR);
Atlas_ezo ezo_do(EZO_DO_ADDR);

const char* fingerprint = "BB 37 30 98 AD 72 39 3F 0C 25 7E 49 E5 F9 85 CA 00 05 C7 AC";
char data_to_send[350] = {0};
float ph_value  = 0;
float orp_value = 0;
float ec_value  = 0;
float do_value  = 0;
float rtd_value = 0;


void setup() {
  Serial.begin(9600);
  Serial.println("Starting...");
  Wire.begin(12, 14);
  Wifi_init_v2();
  Serial.println("Waiting for network connection...");
  Wait_until_connected_to_network();
  Serial.println("Connection Established");
  
  ezo_rtd.begin();
  ezo_ec.begin();
  ezo_ph.begin();
  ezo_orp.begin();
  ezo_do.begin();
  
}

void loop() {
  iotWebConf.doLoop();
  
  ezo_ph.send_cmd('r', &ph_value);
  ezo_orp.send_cmd('r', &orp_value);
  ezo_ec.send_cmd('r', &ec_value);
  ezo_do.send_cmd('r', &do_value);
  ezo_rtd.send_cmd('r', &rtd_value);
  
  Get_data_in_json();
  Http_post_request(data_to_send);

  uint32_t start_time = millis();
  while((millis() - start_time) < TIME_TO_WAIT_MS){
      iotWebConf.doLoop();
      delay(500); 
  }
  
}

void Get_data_in_json(void){

  StaticJsonBuffer<350> dataJSONBuffer;
  JsonObject& data = dataJSONBuffer.createObject();

  uint32_t timestamp = Wifi_get_time_stamp();
  
  while (timestamp == 0){
    timestamp = Wifi_get_time_stamp();
    iotWebConf.doLoop();
  }

  
  data["id"]            = "EZO_SENSORS_ID";
  data["timestamp"]     = String(timestamp);
  data["ph"]            = String(ph_value);
  data["orp"]           = String(orp_value);
  data["conductivity"]  = String(ec_value);
  data["do"]            = String(do_value);
  data["temp"]          = String(rtd_value);  

  Serial.println("Data: ");
  data.prettyPrintTo(Serial);
  data.prettyPrintTo(data_to_send, sizeof(data_to_send));
}

/**
 * Handle web requests to "/" path.
 */
void handleRoot(void){
  
  // -- Let IotWebConf test and handle captive portal requests.
  if (iotWebConf.handleCaptivePortal())
  {
    // -- Captive portal request were already served.
    return;
  }
  String s = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/>";
  s += "<title>ECM AP - Home page</title></head><body>Hello!";
  s += "Go to <a href='config'>configure page</a> to change settings.";
  s += "</body></html>\n";

  server.send(200, "text/html", s);
}
