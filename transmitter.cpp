#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <QuickEspNow.h>

static const uint8_t led_pin = 2;
#define DEST_ADDR ESPNOW_BROADCAST_ADDRESS
const unsigned int SEND_MSG_MSEC = 500;

// CAR MAC    = 40:22:D8:04:49:98
// ESP-CAM    = 24:D7:EB:0F:E5:7C
// REMOTE MAC = E8:DB:84:11:B6:C8

void dataReceived(uint8_t *address, uint8_t *data, uint8_t len, signed int rssi, bool broadcast)
{
  digitalWrite(led_pin, HIGH);
  Serial.print("Received: ");
  Serial.printf("%.*s\n", len, data);
  Serial.printf("RSSI: %d dBm\n", rssi);
  Serial.printf("From: " MACSTR "\n", MAC2STR(address));
  Serial.printf("%s\n", broadcast ? "Broadcast" : "Unicast");
  digitalWrite(led_pin, LOW);
}

const byte numChars = 32;
boolean newData = false;
char dataToSend[32];
void serialread()
{
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;
  while (Serial.available() > 0 && newData == false)
  {
    rc = Serial.read();
    if (rc != endMarker)
    {
      dataToSend[ndx] = rc;
      ndx++;
      if (ndx >= numChars)
      {
        ndx = numChars - 1;
      }
    }
    else
    {
      dataToSend[ndx] = '\0'; // terminate the string
      ndx = 0;
      newData = true;
    }
  }
}

void setup()
{
  digitalWrite(led_pin, HIGH);
  Serial.begin(115200);
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin("mbotcar1", "12345678");

  // connecting to wifi
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("Connected to %s in channel %d\n", WiFi.SSID().c_str(), WiFi.channel());
  Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
  Serial.printf("MAC address: %s\n", WiFi.macAddress().c_str());
  quickEspNow.onDataRcvd(dataReceived);
  quickEspNow.setWiFiBandwidth(WIFI_IF_STA, WIFI_BW_HT20);
  quickEspNow.begin();
  digitalWrite(led_pin, LOW);
}

void loop()
{
  serialread();
  if (newData == true)
  {
    Serial.println(dataToSend);
    String message = String(dataToSend);
    quickEspNow.send(DEST_ADDR, (uint8_t *)message.c_str(), message.length());
    newData = false;
    for (int i = 0; i < sizeof(dataToSend); ++i)
    {
      dataToSend[i] = (char)0;
    }
  }
}