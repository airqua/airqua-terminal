#include <rpcWiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include "TFT_eSPI.h"
#include <Wire.h>
#include "MutichannelGasSensor.h"
#include "Free_Fonts.h"

TFT_eSPI tft;
WiFiClientSecure wifiClient;
PubSubClient pubsubClient(wifiClient);

//-------------------- EDIT THIS DATA --------------------

const bool send_enabled = true; // Enable or disable link to AirQua.
const char* mqtt_token = "INSERT_TOKEN_HERE"; // Replace with your device's token from airqua.uk account
const char* mqtt_device = "INSERT_ID_HERE"; // Replace with your device's id from airqua.uk account

//-------------------- DO NOT EDIT ANYTHING BELOW --------------------

const bool debug_wait = false;

const char* mqtt_host = "mqtt.airqua.uk";
const int mqtt_port = 8883;
const char* mqtt_login = "apikey";
const char* mqtt_ca = "-----BEGIN CERTIFICATE-----\nMIIEVzCCAj+gAwIBAgIRALBXPpFzlydw27SHyzpFKzgwDQYJKoZIhvcNAQELBQAw\nTzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\ncmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMjQwMzEzMDAwMDAw\nWhcNMjcwMzEyMjM1OTU5WjAyMQswCQYDVQQGEwJVUzEWMBQGA1UEChMNTGV0J3Mg\nRW5jcnlwdDELMAkGA1UEAxMCRTYwdjAQBgcqhkjOPQIBBgUrgQQAIgNiAATZ8Z5G\nh/ghcWCoJuuj+rnq2h25EqfUJtlRFLFhfHWWvyILOR/VvtEKRqotPEoJhC6+QJVV\n6RlAN2Z17TJOdwRJ+HB7wxjnzvdxEP6sdNgA1O1tHHMWMxCcOrLqbGL0vbijgfgw\ngfUwDgYDVR0PAQH/BAQDAgGGMB0GA1UdJQQWMBQGCCsGAQUFBwMCBggrBgEFBQcD\nATASBgNVHRMBAf8ECDAGAQH/AgEAMB0GA1UdDgQWBBSTJ0aYA6lRaI6Y1sRCSNsj\nv1iU0jAfBgNVHSMEGDAWgBR5tFnme7bl5AFzgAiIyBpY9umbbjAyBggrBgEFBQcB\nAQQmMCQwIgYIKwYBBQUHMAKGFmh0dHA6Ly94MS5pLmxlbmNyLm9yZy8wEwYDVR0g\nBAwwCjAIBgZngQwBAgEwJwYDVR0fBCAwHjAcoBqgGIYWaHR0cDovL3gxLmMubGVu\nY3Iub3JnLzANBgkqhkiG9w0BAQsFAAOCAgEAfYt7SiA1sgWGCIpunk46r4AExIRc\nMxkKgUhNlrrv1B21hOaXN/5miE+LOTbrcmU/M9yvC6MVY730GNFoL8IhJ8j8vrOL\npMY22OP6baS1k9YMrtDTlwJHoGby04ThTUeBDksS9RiuHvicZqBedQdIF65pZuhp\neDcGBcLiYasQr/EO5gxxtLyTmgsHSOVSBcFOn9lgv7LECPq9i7mfH3mpxgrRKSxH\npOoZ0KXMcB+hHuvlklHntvcI0mMMQ0mhYj6qtMFStkF1RpCG3IPdIwpVCQqu8GV7\ns8ubknRzs+3C/Bm19RFOoiPpDkwvyNfvmQ14XkyqqKK5oZ8zhD32kFRQkxa8uZSu\nh4aTImFxknu39waBxIRXE4jKxlAmQc4QjFZoq1KmQqQg0J/1JF8RlFvJas1VcjLv\nYlvUB2t6npO6oQjB3l+PNf0DpQH7iUx3Wz5AjQCi6L25FjyE06q6BZ/QlmtYdl/8\nZYao4SRqPEs/6cAiF+Qf5zg2UkaWtDphl1LKMuTNLotvsX99HP69V2faNyegodQ0\nLyTApr/vT01YPE46vNsDLgK+4cL6TrzC/a4WcmF5SRJ938zrv/duJHLXQIku5v0+\nEwOy59Hdm0PT/Er/84dDV0CSjdR/2XuZM3kpysSKLgD1cKiDA+IRguODCxfO9cyY\nIg46v9mFmBvyH04=\n-----END CERTIFICATE-----\n";

const char* test_request_url = "https://mqtt.airqua.uk/get200";

const int preheat_msec = 300000;
const int preheat_pb_sections = 10;

const int update_msec = 60000; // 1 min
const int send_msec = 3600000; // 60 min


const int width = 320;
const int height = 240;

void redrawWiFiScreen() {
  Serial.println("redrawing wifi screen");
  tft.fillScreen(TFT_BLACK);

  tft.setFreeFont(FF7);
  tft.drawString("AirQua Terminal", (width - tft.textWidth("AirQua Terminal"))/2, height/2 - 30);

  tft.setFreeFont(FF2);
  tft.drawString("Connecting to WiFi...", (width - tft.textWidth("Connecting to WiFi..."))/2, height/2 + 30);

  tft.setFreeFont(FF1);
  tft.drawString("SSID: AirQuaTerminal", (width - tft.textWidth("SSID: AirQuaTerminal"))/2, height/2 + 60);
  tft.drawString("Wait a bit, then connect", (width - tft.textWidth("Wait a bit, then connect,"))/2, height/2 + 80);
  tft.drawString("if network appeared", (width - tft.textWidth("if network appeared"))/2, height/2 + 100);
}

void redrawPreheat() {
  Serial.println("redrawing preheat");
  tft.fillScreen(TFT_BLACK);

  tft.setFreeFont(FF7);
  tft.drawString("AirQua Terminal", (width - tft.textWidth("AirQua Terminal"))/2, height/2 - 30);

  tft.setFreeFont(FF2);
  tft.drawString("Preheating sensors...", (width - tft.textWidth("Preheating sensors..."))/2, height/2 + 30);

  tft.setFreeFont(FF1);
  tft.drawString("^", (width - tft.textWidth("^"))/2 + 15, 5);
  tft.drawString("Cancel", (width - tft.textWidth("Cancel"))/2 + 10, 15);
  tft.drawString("preheat", (width - tft.textWidth("preheat"))/2 + 10, 35);
}

int preheat_pb_last_drawn = -1;
void drawPreheatProgressbar(const int preheat_start) {
  int cusec = (millis() - preheat_start) / (preheat_msec / preheat_pb_sections); //section to draw
  if(cusec == preheat_pb_last_drawn) return;
  tft.fillRect(10 + 30 * cusec, 200, 30, 30, TFT_WHITE);
  preheat_pb_last_drawn = cusec;
}

unsigned long sendt = 0;
unsigned long updatet = 0;
void redrawInterface() {
  Serial.println("redrawing interface");
  tft.fillScreen(TFT_BLACK);

  //Title
  tft.setFreeFont(FF5);
  tft.drawString("AirQua Terminal", 1, 1);
  tft.drawLine(0, 20, width, 20, TFT_WHITE);

  //Footer
  tft.drawLine(0, height - 30, width, height - 30, TFT_WHITE);
  tft.setFreeFont(FF1);
  int tm = (int)((send_msec - (millis() - sendt)) / 60000);
  
  char* buf = new char[25];
  sprintf(buf, "Next upload: %d min", tm);
  tft.drawString(buf, (width - tft.textWidth(buf)) - 1, height - 20);
  delete[] buf;

  //Data // TODO that's awful needs rewrite from ground up or smth
  tft.setFreeFont(FF5);

  const char* sensors[] = {
    "CO", "C2H5OH", "NULL", "NULL", "NO2", "NULL", "NULL", "NULL"
  };
  float data[] = {
    0, 0, 0, 0, 0, 0, 0, 0
  };

  data[0] = gas.measure_CO();
  data[1] = gas.measure_C2H5OH();
  data[4] = gas.measure_NO2();

  Serial.print("CO2: ");
  Serial.println(data[0]);
  Serial.print("C2H5OH: ");
  Serial.println(data[1]);
  Serial.print("NO2: ");
  Serial.println(data[4]);
  
  int i; //columns
  int j; //rows
  int si = 0;
  for(i = 1; i < 4; i+=2)
  {
    for(j = 1; j <= 4; j++)
    {
      if(sensors[si] != "NULL") {
        char* str = new char[25];
        sprintf(str, "%s: %.2f ppm", sensors[si], data[si]);
        tft.drawString(str, 
          (si + 4 <= 7 && sensors[si + 4] == "NULL" && j % 2 == 0) || (si >= 4 && sensors[si - 4] == "NULL" && j % 2 == 1) ? 
          (width - tft.textWidth(str))/2 :
          (width / 4 * i) - tft.textWidth(str)/2, 
        height / 7 * j + 15);
        delete[] str;
      }
      si += 1;
    }
  }
}

void makeTestRequest() {
  HTTPClient http;
  Serial.println("Making test request...");

  if(http.begin(wifiClient, test_request_url)) {
    int httpCode = http.GET();
    if(httpCode > 0) {
        Serial.println("Test request made. Data:");
        if(httpCode == HTTP_CODE_OK) {
            Serial.println(http.getString());
        } else {
            Serial.println("http code not ok");
        }
    } else {
        Serial.printf("Test request failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  } else {
    Serial.println("Unable to connect to the server");
  }
}

void sendMqttData() {
  if (!pubsubClient.connected()) {
    Serial.println("MQTT not connected. Connecting...");
    if(pubsubClient.connect(mqtt_device, mqtt_login, mqtt_token)) {
      Serial.println("MQTT connected successfully.");
    } else {
      Serial.printf("MQTT connection error. State: %d", pubsubClient.state());
      return;
    }
  }
  char* buf = new char[45];
  sprintf(buf, "{\"co\":%d,\"no2\":%d,\"c2h5oh\":%d}", 
    (int)gas.measure_CO(), 
    (int)gas.measure_NO2(), 
    (int)gas.measure_C2H5OH()
  );
  pubsubClient.publish(mqtt_device, buf);
  Serial.println("Published data to MQTT:");
  Serial.println(buf);
  delete[] buf;
}

int preheat = -1;
// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  while(debug_wait && !Serial); //wait for serial
  
  pinMode(WIO_BUZZER, OUTPUT);
  pinMode(WIO_KEY_A, INPUT_PULLUP);
  pinMode(WIO_KEY_C, INPUT_PULLUP);

  Serial.printf("RTL8720 Firmware Version: %s\n", rpc_system_version());
  
  tft.begin();
  tft.setRotation(3);

  gas.begin(0x04);

  Serial.print("Gas Firmware Version = ");
  Serial.println(gas.getVersion());

  if(send_enabled) {
      redrawWiFiScreen();

      WiFiManager wifiManager;
      wifiManager.autoConnect("AirQuaTerminal");
      
      delay(1000);
      makeTestRequest();

      wifiClient.setCACert(mqtt_ca);
      pubsubClient.setServer(mqtt_host, mqtt_port);    
  }

  analogWrite(WIO_BUZZER, 128);
  delay(1000);
  analogWrite(WIO_BUZZER, 0);

  // Preheat screen
  redrawPreheat();
  preheat = millis();
}

void loop() {  
  if(preheat != -1) { // handle preheat screen
    if(digitalRead(WIO_KEY_A) == LOW or millis() >= preheat + preheat_msec) { //end preheat
      analogWrite(WIO_BUZZER, 128);
      delay(1000);
      analogWrite(WIO_BUZZER, 0);
      preheat = -1;
      redrawInterface();
      return;
    }
    drawPreheatProgressbar(preheat);
  } else {
    if (digitalRead(WIO_KEY_C) == LOW) {  // handle force update via C btn
      analogWrite(WIO_BUZZER, 128);
      delay(200);
      analogWrite(WIO_BUZZER, 0);
      redrawInterface();
    }
    if (digitalRead(WIO_KEY_A) == LOW) { // DEBUG
      analogWrite(WIO_BUZZER, 128);
      delay(200);
      analogWrite(WIO_BUZZER, 0);
      sendMqttData();
    }
    if(send_enabled) { // handle sending
      if(millis() - sendt > send_msec) {
        sendt = millis();
        sendMqttData();
      }
    }
    if(millis() - updatet > update_msec) { // handle update once in update_msec mseconds
      updatet = millis();
      redrawInterface();
    } 
  }
}