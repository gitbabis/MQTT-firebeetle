#ifndef SECRETS_H
#define SECRETS_H

// WiFi Credentials
const char *ssid = "babis";
const char *password = "babishome!";

// MQTT Broker Details
const char *mqtt_server = "hass.syros.aegean.gr";
const int mqtt_port = 1883;
const char *mqtt_user = "dpsd26000";      // Leave blank if not needed
const char *mqtt_password = "dpsd26000!"; // Leave blank if not needed

// MQTT Topic
const char *mqtt_topic = "homeassistant/sensor/uno_r4/state";
const char *mqtt_publish_topic = "homeassistant/sensor/uno_r4/set";

#endif
