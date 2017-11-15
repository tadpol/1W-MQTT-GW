# 1W MQTT Gateway

A simple example of sending multiple 1W temperuature sensors to Exosite's IoT cloud.


## Setup

1. Setup the project space
   `murano init`

2. Create a new product
   `murano product create --save owmqttgw`

3. Setup Device Authentication method.
   To keep things simple, this example uses device passwords.
   `murano setting write Gateway.provisioning auth_type password`

4. Turn on MQTT
   `murano setting write Gateway.protocol name mqtt`

5. Switch to port 8883
   `murano setting write Gateway.protocol port --num 8883`

4. Whitelist a test device
   `murano device enable 123456 --auth password --cred 123456789123456789123456789`
   You may wish to pick a better password, it must be at least 24 characters long.

5. Get the server domain for your product
   `murano domain --type product --brief`
   Copy this domain name into `AIO_SERVER` in `1w-mqtt-gw.cpp`.

6. Set the SSID and password for your Network
   Edit the lines for `WLAN_SSID` and `WLAN_PASS` in `1w-mqtt-gw.cpp`.

7. Upload to an ESP8266 Thing
