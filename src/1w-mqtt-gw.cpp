#include <Arduino.h>

#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

const int ONE_WIRE_BUS = 0;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

/************************* WiFi Access Point *********************************/
const char WLAN_SSID[] = "<SSID>";
const char WLAN_PASS[] = "<PASSWORD>";

/************************* Murano Setup *********************************/
const char AIO_SERVER[]   = "<Product Domain Name>";
const int  AIO_SERVERPORT = 8883;
const char AIO_USERNAME[] = "123456"; // Device ID (often serial number)
const char AIO_KEY[]      = "123456789123456789xxx"; // The password you enabled the device with

WiFiClientSecure client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

void setup(void)
{
  byte led_state = LOW;
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // LED is On.

  Serial.begin(115200);
  Serial.println("Hello");
  for(int i=0; i < 5; i++) {
    delay(200);
    Serial.print("#");
  }
  Serial.println("");
  digitalWrite(LED_BUILTIN, HIGH); // LED is off.
  
  sensors.begin();

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, led_state);
    led_state = (led_state==HIGH)?LOW:HIGH;
    delay(500);
    Serial.print(("."));
  }
  Serial.println(" WiFi connected.");

  digitalWrite(LED_BUILTIN, HIGH); // HIGH turns LED off.
}

void MQTT_connect();
void loop(void)
{
  uint8_t addr[8];
  char tpname[13+16+1]; // "$resource/ow/xxxxxxxxxxxxxxxx"
  char value[12];

  MQTT_connect();

  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.

  Serial.print("Sensors : "); Serial.println(sensors.getDeviceCount());
  for(int i=0; i < sensors.getDeviceCount();i++) {
    sensors.getAddress(addr, i);
    Serial.print("Temperature for the device (");
    sprintf(tpname, "$resource/ow/%0x%0x%0x%0x%0x%0x%0x%0x", 
      addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], addr[6], addr[7]);
    Serial.print(tpname);
    Serial.print(") ");
    Serial.print(i, DEC);
    Serial.print(" is: ");
    float tmc = sensors.getTempCByIndex(i);
    dtostrf(tmc, 0, 2, value);
    Serial.println(value);

    mqtt.publish(tpname, value);
  }

  mqtt.processPackets(10000);
  mqtt.ping();
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  byte led_state = HIGH;
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  digitalWrite(LED_BUILTIN, led_state);
  led_state = (led_state==HIGH)?LOW:HIGH;
  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 30;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    digitalWrite(LED_BUILTIN, led_state);
    led_state = (led_state==HIGH)?LOW:HIGH;
    Serial.print(ret); Serial.print(" ");
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      digitalWrite(LED_BUILTIN, HIGH);
      // basically die and wait for WDT to reset me
      while (1);
    }
  }

  Serial.println("MQTT Connected!");
  digitalWrite(LED_BUILTIN, HIGH);
}


