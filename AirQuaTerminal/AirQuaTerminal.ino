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
const char* mqtt_ca = \
                      "-----BEGIN CERTIFICATE-----\n"
                      "MIIFBTCCAu2gAwIBAgIQS6hSk/eaL6JzBkuoBI110DANBgkqhkiG9w0BAQsFADBP\n"
                      "MQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJuZXQgU2VjdXJpdHkgUmVzZWFy\n"
                      "Y2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBYMTAeFw0yNDAzMTMwMDAwMDBa\n"
                      "Fw0yNzAzMTIyMzU5NTlaMDMxCzAJBgNVBAYTAlVTMRYwFAYDVQQKEw1MZXQncyBF\n"
                      "bmNyeXB0MQwwCgYDVQQDEwNSMTAwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEK\n"
                      "AoIBAQDPV+XmxFQS7bRH/sknWHZGUCiMHT6I3wWd1bUYKb3dtVq/+vbOo76vACFL\n"
                      "YlpaPAEvxVgD9on/jhFD68G14BQHlo9vH9fnuoE5CXVlt8KvGFs3Jijno/QHK20a\n"
                      "/6tYvJWuQP/py1fEtVt/eA0YYbwX51TGu0mRzW4Y0YCF7qZlNrx06rxQTOr8IfM4\n"
                      "FpOUurDTazgGzRYSespSdcitdrLCnF2YRVxvYXvGLe48E1KGAdlX5jgc3421H5KR\n"
                      "mudKHMxFqHJV8LDmowfs/acbZp4/SItxhHFYyTr6717yW0QrPHTnj7JHwQdqzZq3\n"
                      "DZb3EoEmUVQK7GH29/Xi8orIlQ2NAgMBAAGjgfgwgfUwDgYDVR0PAQH/BAQDAgGG\n"
                      "MB0GA1UdJQQWMBQGCCsGAQUFBwMCBggrBgEFBQcDATASBgNVHRMBAf8ECDAGAQH/\n"
                      "AgEAMB0GA1UdDgQWBBS7vMNHpeS8qcbDpHIMEI2iNeHI6DAfBgNVHSMEGDAWgBR5\n"
                      "tFnme7bl5AFzgAiIyBpY9umbbjAyBggrBgEFBQcBAQQmMCQwIgYIKwYBBQUHMAKG\n"
                      "Fmh0dHA6Ly94MS5pLmxlbmNyLm9yZy8wEwYDVR0gBAwwCjAIBgZngQwBAgEwJwYD\n"
                      "VR0fBCAwHjAcoBqgGIYWaHR0cDovL3gxLmMubGVuY3Iub3JnLzANBgkqhkiG9w0B\n"
                      "AQsFAAOCAgEAkrHnQTfreZ2B5s3iJeE6IOmQRJWjgVzPw139vaBw1bGWKCIL0vIo\n"
                      "zwzn1OZDjCQiHcFCktEJr59L9MhwTyAWsVrdAfYf+B9haxQnsHKNY67u4s5Lzzfd\n"
                      "u6PUzeetUK29v+PsPmI2cJkxp+iN3epi4hKu9ZzUPSwMqtCceb7qPVxEbpYxY1p9\n"
                      "1n5PJKBLBX9eb9LU6l8zSxPWV7bK3lG4XaMJgnT9x3ies7msFtpKK5bDtotij/l0\n"
                      "GaKeA97pb5uwD9KgWvaFXMIEt8jVTjLEvwRdvCn294GPDF08U8lAkIv7tghluaQh\n"
                      "1QnlE4SEN4LOECj8dsIGJXpGUk3aU3KkJz9icKy+aUgA+2cP21uh6NcDIS3XyfaZ\n"
                      "QjmDQ993ChII8SXWupQZVBiIpcWO4RqZk3lr7Bz5MUCwzDIA359e57SSq5CCkY0N\n"
                      "4B6Vulk7LktfwrdGNVI5BsC9qqxSwSKgRJeZ9wygIaehbHFHFhcBaMDKpiZlBHyz\n"
                      "rsnnlFXCb5s8HKn5LsUgGvB24L7sGNZP2CX7dhHov+YhD+jozLW2p9W4959Bz2Ei\n"
                      "RmqDtmiXLnzqTpXbI+suyCsohKRg6Un0RC47+cpiVwHiXZAW+cn8eiNIjqbVgXLx\n"
                      "KPpdzvvtTnOPlC7SQZSYmdunr3Bf9b77AiC/ZidstK36dRILKz7OA54=\n"
                      "-----END CERTIFICATE-----\n";

const char* test_request_url = "https://mqtt.airqua.uk/get200/";

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
  IPAddress ip;
  if (WiFi.hostByName("mqtt.airqua.uk", ip)) {
    Serial.print("Resolved IP: ");
    Serial.println(ip);
  } else {
    Serial.println("DNS resolution failed.");
  }
  
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
      WiFiManager wifiManager;
      redrawWiFiScreen();

      // wifiManager.resetSettings();
      wifiManager.autoConnect("AirQuaTerminal");
      
      delay(1000);

      wifiClient.setCACert(mqtt_ca);
      pubsubClient.setServer(mqtt_host, mqtt_port);    
      makeTestRequest();
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