#ifndef SECRETS_H
#define SECRETS_H

// WiFi Credentials
const char *ssid = "ΤΟ_WIFI_ΣΑΣ";
const char *password = "ΤΟ_PASSWORD_ΣΑΣ";

// MQTT Broker Details
const char *mqtt_server = "hass.syros.aegean.gr";
const int mqtt_port = 1883;
const char *mqtt_user = "mqtt_username";      // Leave blank if not needed
const char *mqtt_password = "mqtt_password"; // Leave blank if not needed

// MQTT Topic
const char *mqtt_topic = "homeassistant/sensor/uno_r4/state";
const char *mqtt_publish_topic = "homeassistant/sensor/uno_r4/set";

#endif
