/*
* Author: Mohamad Ariffin Zulkifli
* Organization: Myinvent Technologies Sdn Bhd
*
* This sketch has 3 execution steps:
* 1. Initialized WiFi connectivity
* 2. Sensors reading for data acquisition
* 3. Favoriot HTTP API request
* 
* Please select ESP32/ESP8266 Boards before compiling the sketch
* (example) Go to menu, Tools > Board > ESP32 Arduino or ESP8266 Boards
*
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char ssid[] = "YOUR_WIFI_SSID";
const char pass[] = "YOUR_WIFI_PASSWORD";

#include <SimpleDHT.h>

int pinDHT11 = D1;
SimpleDHT11 dht11(pinDHT11);

unsigned long lastMillis = 0;

void setup() {
  Serial.begin(9600);

  // STEP #1 - Connect to WiFi router/hotspot
  WiFi.begin(ssid, pass);

  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(250);
  }

  Serial.println();
  Serial.println("WiFi Connected!");

}

void loop() {
  // STEP #2 - Data acquisition
  Serial.println("=================================");
  Serial.println("Sample DHT11...");
  
  byte temperature = 0;
  byte humidity = 0;
  
  int err = SimpleDHTErrSuccess;
  
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err=");
    Serial.print(SimpleDHTErrCode(err));
    Serial.print(",");
    Serial.println(SimpleDHTErrDuration(err));
    delay(1000);
    
    return;
  }
  
  Serial.print("Sample OK: ");
  Serial.print((int)temperature); Serial.print(" *C, "); 
  Serial.print((int)humidity); Serial.println(" H");

  delay(1500);

  if(millis() - lastMillis > 15000){
    lastMillis = millis();
    
    // #STEP 3 - Favoriot HTTP API Request - Send data to Favoriot data stream
    HTTPClient http;
  
    http.begin("http://apiv2.favoriot.com/v2/streams");
  
    http.addHeader("Content-Type", "application/json");
    http.addHeader("apikey", "YOUR_DEVICE_ACCESS_TOKEN");
  
    String body = "{";
    body += "\"device_developer_id\": \"YOUR_DEVICE_DEVELOPER_ID\",";
    body += "\"data\": {";
    body += "\"temperature\": " + (String)temperature + ",";
    body += "\"humidity\": " + (String)humidity;
    body += "}";
    body += "}";
    
    int httpCode = http.POST(body);
  
    if(httpCode > 0){
      Serial.print("HTTP Request: ");
      
      httpCode == 201 ? Serial.print("Success, ") : Serial.print("Error, ");
      Serial.println(http.getString());
    }
    else{
      Serial.println("HTTP Request Connection Error!");
    }
  
    http.end();
  }
}
