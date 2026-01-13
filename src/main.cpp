/**
 * @file main.cpp // File name identifier for documentation tools.
 * @brief MQTT Client for FireBeetle ESP32 with TMP36 sensor and External LED
 * control. // Summary of the program's functionality.
 */

#include "secrets.h" // Includes external header file containing sensitive credentials like WiFi SSID and MQTT details.
#include <Arduino.h> // Includes the core Arduino framework for standard functions.
#include <PubSubClient.h> // Includes the MQTT library to handle publishing and subscribing.
#include <WiFi.h> // Includes the ESP32 WiFi library to manage wireless networking.

/**
 * @brief Hardware Pin Definitions
 */
const int TEMP_PIN = 34; // Analog pin A1 (IO34) connected to TMP36 signal pin.
const int EXT_LED_PIN =
    25; // Digital pin D2 (IO25) connected to the external LED anode.

/**
 * @brief MQTT and Networking Objects
 */
WiFiClient espClient; // Creates a WiFiClient object to manage the underlying
                      // TCP/IP connection.
PubSubClient client(espClient); // Initializes the MQTT client by wrapping the
                                // WiFiClient for networked messaging.

long lastMsg = 0; // Global variable to store the timestamp of the last
                  // temperature reading in milliseconds.
char msg[50]; // Character array buffer used to format strings before publishing
              // to MQTT topics.

/**
 * @brief Initializes the WiFi connection.
 */
void setup_wifi() { // Function block to manage the initial connection to the
                    // local wireless network.
  delay(10);        // Waits for 10ms to ensure the hardware is ready for radio
                    // operations.
  Serial.println(); // Prints a blank line to the serial monitor to separate
                    // startup logs.
  Serial.print("Connecting to "); // Prints a status prefix to inform the user
                                  // about the connection attempt.
  Serial.println(ssid); // Prints the SSID name retrieved from the secrets.h
                        // configuration file.

  WiFi.begin(ssid, password); // Initiates the asynchronous connection process
                              // using the stored network credentials.

  while (WiFi.status() !=
         WL_CONNECTED) { // Loops continuously until the WiFi hardware signals a
                         // successful connection.
    delay(500);          // Pauses for 500ms between each status poll to avoid
                         // busy-waiting.
    Serial.print(".");   // Prints a dot as a visual progress indicator for the
                         // user during connection.
  } // End of the blocking WiFi connection loop.

  Serial.println(
      ""); // Moves to a new line on the serial terminal once connected.
  Serial.println("WiFi connected"); // Confirms to the user that the network
                                    // link is successfully established.
  Serial.print(
      "IP address: "); // Label for the following device network address.
  Serial.println(WiFi.localIP()); // Retrieves and prints the IP assigned to the
                                  // ESP32 by the DHCP server.
} // End of the setup_wifi function.

/**
 * @brief MQTT Message Callback function.
 * Handles incoming "LedOn" or "LedOff" commands to control the external LED.
 */
void callback(char *topic, byte *payload,
              unsigned int length) { // Entry point for logic triggered by
                                     // incoming MQTT messages.
  Serial.print("Message arrived ["); // Start of the debug log showing metadata
                                     // of the message.
  Serial.print(topic); // Logs the specific topic string that the message was
                       // published to.
  Serial.print("] ");  // Closing bracket for the debug topic display.

  String message = ""; // Creates a dynamic string object to reconstruct the raw
                       // message payload.
  for (unsigned int i = 0; i < length;
       i++) { // Loops through every byte in the incoming payload buffer.
    message += (char)payload[i]; // Casts each byte to a UTF-8 character and
                                 // appends it to the string object.
  } // End of the payload reconstruction loop.
  Serial.println(message); // Prints the complete received message text to the
                           // Serial monitor.

  message.toLowerCase(); // Normalizes the string to lowercase to allow
                         // case-insensitive command matching.

  if (message == "ledon") { // Selection branch executed if the reconstructed
                            // message matches "ledon".
    digitalWrite(EXT_LED_PIN, HIGH); // Sends a HIGH voltage signal to IO25 to
                                     // illuminate the external LED.
    Serial.println(
        "External LED (IO25) turned ON"); // Logs the physical state change to
                                          // the serial console.
  } else if (message == "ledoff") { // Selection branch executed if the message
                                    // matches "ledoff".
    digitalWrite(
        EXT_LED_PIN,
        LOW); // Sends a LOW (0V) signal to IO25 to extinguish the external LED.
    Serial.println(
        "External LED (IO25) turned OFF"); // Logs the physical state change for
                                           // remote monitoring.
  } // End of the command processing conditional logic.
} // End of the MQTT message callback function.

/**
 * @brief Manages the MQTT connection and subscription.
 */
void reconnect() { // Logic to recover the connection if the MQTT link is broken
                   // or not yet established.
  while (!client.connected()) { // Loop that persists until the client object
                                // confirms connection to the broker.
    Serial.print(
        "Attempting MQTT connection..."); // Status log indicating the start of
                                          // a broker handshake.

    String clientId =
        "FireBeetleClient-"; // Prefix for the MQTT client identifier required
                             // by the protocol.
    clientId +=
        String(random(0xffff), HEX); // Appends a random hex value to ensure the
                                     // ID is unique on the broker.

    if (client.connect(
            clientId.c_str(), mqtt_user,
            mqtt_password)) { // Attempts to connect using the generated ID and
                              // optional credentials.
      Serial.println("connected");  // Confirms successful authentication and
                                    // handshake with the MQTT broker.
      client.subscribe(mqtt_topic); // Re-registers interest in the state topic
                                    // to receive incoming commands.
    } else { // Execution path if the connection attempt fails (e.g., wrong IP
             // or credentials).
      Serial.print(
          "failed, rc="); // Starts the error report for the failed connection.
      Serial.print(client.state()); // Prints the PubSubClient return code to
                                    // diagnose why the connection failed.
      Serial.println(" try again in 5 seconds"); // Notifies the user of the
                                                 // mandatory retry interval.
      delay(5000); // Blocks for 5 seconds to avoid spamming the broker with
                   // rapid connection attempts.
    } // End of connection success/failure check.
  } // End of the reconnection persistence loop.
} // End of the reconnect function.

/**
 * @brief Reads temperature from TMP36 and publishes to MQTT if threshold is
 * met.
 */
void handleTemperature() { // Handles sensor data acquisition and outbound MQTT
                           // logic.
  long now = millis();     // Gets the current system uptime in milliseconds for
                           // timing logic.
  if (now - lastMsg >
      5000) { // Checks if 5000ms (5 seconds) have passed since the last report.
    lastMsg =
        now; // Updates the timestamp of the last report to reset the timer.

    int analogValue =
        analogRead(TEMP_PIN); // Samples the 12-bit ADC on IO34 to get the raw
                              // sensor voltage reading.

    float voltage =
        analogValue * (3.3 / 4095.0); // Maps the raw ADC value (0-4095) to the
                                      // actual voltage (0-3.3V).

    float temperatureC =
        (voltage - 0.5) * 100.0; // Converts millivolts to Celsius based on
                                 // TMP36 characteristic (10mV/C, 500mV offset).

    Serial.print("Temperature: "); // Serial label for the measured temperature.
    Serial.print(temperatureC); // Prints the floating-point temperature value.
    Serial.println(" C");       // Append the units (Celsius) and a newline.

    if (temperatureC >= 7.0) { // Compares the calculated temperature against
                               // the 7-degree threshold.
      client.publish(mqtt_publish_topic,
                     "LedOn"); // Sends the "LedOn" command to the broker if it
                               // is warm enough.
      Serial.println(
          "Published: LedOn (Temp >= 7)"); // Logs the outbound MQTT action for
                                           // the threshold trigger.
    } else { // Logic for when the temperature is strictly below 7 degrees.
      client.publish(mqtt_publish_topic,
                     "LedOff"); // Sends the "LedOff" command to the broker to
                                // signal low temp.
      Serial.println(
          "Published: LedOff (Temp < 7)"); // Logs the outbound MQTT action for
                                           // the lower threshold.
    } // End of temperature threshold logic.
  } // End of the 5-second interval check.
} // End of the handleTemperature function.

/**
 * @brief Setup function.
 */
void setup() { // Standard Arduino entry point for hardware and service
               // initialization.
  pinMode(EXT_LED_PIN, OUTPUT); // Sets the data direction for pin IO25 to
                                // output mode for driving an LED.
  digitalWrite(EXT_LED_PIN,
               LOW); // Initializes the LED to a known state (OFF) upon startup.

  Serial.begin(115200); // Opens the Universal Asynchronous Receiver/Transmitter
                        // at 115.2 kbps for debugging.
  setup_wifi(); // Executes the WiFi connection routine defined earlier in the
                // script.

  client.setServer(mqtt_server,
                   mqtt_port);  // Configures the MQTT client with the address
                                // and port of the target broker.
  client.setCallback(callback); // Tells the MQTT library which function to run
                                // when messages arrive.
} // End of the hardware and software setup process.

/**
 * @brief Main loop.
 */
void loop() { // Infinite execution loop that keeps the microcontroller running
              // active tasks.
  if (!client.connected()) { // Polling check to monitor if the broker
                             // connection is still alive.
    reconnect(); // Triggers the reconnection logic if the link to the broker is
                 // lost.
  } // End of connection health check.
  client.loop(); // Essential library call to handle heartbeat packets and
                 // process incoming data buffers.

  handleTemperature(); // Executes the sensor monitoring and automated MQTT
                       // reporting logic.
} // End of the main program loop..
