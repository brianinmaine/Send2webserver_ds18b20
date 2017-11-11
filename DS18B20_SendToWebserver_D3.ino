// Wemos Mini, DS18B20 temp sensor, and a SSD1306 display.
// this program uploads the temperature every 10 minutes to my
// own Ubuntu webserver running Apache, PHP7 and MySQL.
//
const char * ssid = "BT";                // access point
const char * password = "harmless";      // access point password
const char * ssid2 = "Trauger";                // second access point
const char * password2 = "harmless";      // second access point password
const char * host = "trauger.ddns.net";  // webserver to upload to
const uint16_t port = 80;                // webserver port, usually 80
int uploadInterval = 10;       // how many minutes between uploads

// DS18B20 temp sensor:
// red=5V black=GND blue=GPIO0, D3(digital OneWire)
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS D3
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
// this is for my probe only
DeviceAddress probe = {0x28, 0xFF, 0x55, 0xDB, 0xB3, 0x16, 0x03, 0x66 };

// SSD1306 display:
// GND=gnd VDD=5v SCK=GPIO5,D1 SDA=GPIO4,D2
#include "SSD1306.h"
SSD1306  display(0x3c, D2, D1);

// wifi:
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti WiFiMulti;

// turn on the builtin LED when uploading the temp
const int ledPin = 2; //gpio2 is D4, the builtin LED
int ledState = HIGH; // HIGH is off, LOW is on.

// multitasking loop without delay()
long previousMillis = 0;
long interval = 1000 * 60 * uploadInterval;     // X minutes

int lanConnected = FALSE;

void setup(void)
{
  Serial.begin(9600);

  pinMode(ledPin, OUTPUT);     // prepare the LED
  digitalWrite(ledPin, HIGH);

  display.init();         // prepare the display
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);

  sensors.setResolution(probe, 12); // can be 9(fast),10,11,or 12(best)
  sensors.begin();

  Serial.println();
  Serial.println();
  Serial.print("waiting ");
  Serial.print(uploadInterval);
  Serial.println(" minutes.");
}

void loop(void)
{
  showTemp();                                   // display the temp
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > interval) {  // loop to wait 10 minutes
    previousMillis = currentMillis;
    connectLan(); // just the wifi part
    if (lanConnected) {
      uploadToHost(); // send temp to webserver
    }
  }
}

float temp() {
  sensors.requestTemperatures();
  // getTempF with resolution 12 is 4 digits of resolution -65F - 255F
  return (sensors.getTempF(probe));
}

void showTemp() {
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_24);
  String curTemp = String(temp(), 4);    // 4 digits of resolution
  display.drawString(2, 10, curTemp + " *F");    // 2 over, 10 down
  display.display();
}

void connectLan() {
  WiFiMulti.addAP(ssid, password);
  WiFiMulti.addAP(ssid2, password2);   // connect to either host
  Serial.print("Waiting for WiFi: ");

  if (WiFiMulti.run() == WL_CONNECTED) {
    Serial.println();
    Serial.print("connected, my IP: ");
    Serial.println(WiFi.localIP());
    lanConnected = TRUE;
  } else {
    Serial.println("* not connected *");
  }
}

void uploadToHost() {
  WiFiClient client;
  if (client.connect(host, port)) {
    Serial.println();
    Serial.print("connected to ");
    Serial.println(host);
    String curTemp = String(temp(), 2);       // convert the float value to a string
    Serial.println("Sending:" + curTemp);
    digitalWrite(ledPin, LOW);                 // turn on the LED
    client.print("GET /write_data.php?");
    client.print("value=");
    client.print(curTemp);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(host);
    client.println("");                       // MANDATORY BLANK LINE HERE!
    Serial.println("Data sent");
  }
  while (client.available()) {               // any response from the server?
    char c = client.read();
    Serial.write(c);
  }
  Serial.println();
  Serial.println("closing connection");
  client.stop();
  digitalWrite(ledPin, HIGH);                // turn LED off again
  lanConnected = FALSE;                      // turn off connected flag
  Serial.print("waiting ");
  Serial.print(uploadInterval);
  Serial.println(" minutes.");
}

