# MQTT-2dev: FireBeetle ESP32 MQTT Client

Αυτό το project υλοποιεί έναν MQTT client χρησιμοποιώντας μια πλακέτα **FireBeetle ESP32**. Το σύστημα συνδέεται σε έναν MQTT Broker, παρακολουθεί τη θερμοκρασία μέσω ενός αισθητήρα TMP36 και επιτρέπει τον απομακρυσμένο έλεγχο ενός εξωτερικού LED.

## 📋 Περιγραφή
Το project έχει σχεδιαστεί για να λειτουργεί ως κόμβος IoT που:
1.  **Διαβάζει δεδομένα** από έναν αναλογικό αισθητήρα θερμοκρασίας (TMP36).
2.  **Δημοσιεύει (Publish)** εντολές σε ένα MQTT topic με βάση το όριο των 7°C.
3.  **Εγγράφεται (Subscribe)** σε ένα MQTT topic για να δέχεται εντολές ελέγχου (LedOn/LedOff) για ένα εξωτερικό LED.

## ✨ Χαρακτηριστικά
-   **Σύνδεση WiFi:** Αυτόματη σύνδεση στο τοπικό δίκτυο.
-   **MQTT Επικοινωνία:** Χρήση της βιβλιοθήκης `PubSubClient` για αμφίδρομη επικοινωνία.
-   **Έξυπνη Λογική:** Αυτόματη αποστολή εντολής "LedOn" αν η θερμοκρασία είναι >= 7°C και "LedOff" αν είναι χαμηλότερη.
-   **Remote Control:** Χειροκίνητος έλεγχος του LED μέσω MQTT μηνυμάτων.
-   **Ασφάλεια:** Διαχωρισμός ευαίσθητων δεδομένων (SSID, Passwords) στο αρχείο `secrets.h`.

## 🛠️ Απαιτούμενο Hardware
-   **Πλακέτα:** DFRobot FireBeetle ESP32.
-   **Αισθητήρας:** TMP36 (Temperature Sensor).
-   **Έξοδος:** 1x LED και 1x Αντίσταση (π.χ. 220Ω).
-   **Breadboard & Καλώδια.**

### Συνδεσμολογία (Pinout)
| Εξάρτημα | Pin ESP32 (FireBeetle) | Περιγραφή |
| :--- | :--- | :--- |
| **TMP36 Signal** | IO34 (A1) | Αναλογική ανάγνωση θερμοκρασίας |
| **External LED** | IO25 (D2) | Ψηφιακή έξοδος ελέγχου LED |

## 💻 Απαιτήσεις Λογισμικού
-   **PlatformIO IDE** (συνιστάται) ή Arduino IDE.
-   **Βιβλιοθήκες:**
    -   `PubSubClient` (by Nick O'Leary)
    -   `WiFi` (ενσωματωμένη στο ESP32 core)

## ⚙️ Ρύθμιση (Configuration)
Πριν το upload, πρέπει να ρυθμίσετε το αρχείο `src/secrets.h`:

```cpp
// WiFi Credentials
const char *ssid = "ΤΟ_WIFI_ΣΑΣ";
const char *password = "ΤΟ_PASSWORD_ΣΑΣ";

// MQTT Broker Details
const char *mqtt_server = "hass.syros.aegean.gr";
const int mqtt_port = 1883;
const char *mqtt_user = "username";
const char *mqtt_password = "password";

// MQTT Topics
const char *mqtt_topic = "homeassistant/sensor/uno_r4/state";        // Για λήψη εντολών
const char *mqtt_publish_topic = "homeassistant/sensor/uno_r4/set";  // Για αποστολή κατάστασης
```

## 🚀 Οδηγίες Χρήσης
1.  **Σύνδεση:** Συνδέστε τα εξαρτήματα σύμφωνα με το Pinout.
2.  **Ρύθμιση:** Ενημερώστε το `secrets.h` με τα δικά σας στοιχεία.
3.  **Compile & Upload:** Χρησιμοποιήστε το PlatformIO για να ανεβάσετε τον κώδικα στην πλακέτα.
4.  **Monitoring:** Ανοίξτε το Serial Monitor (baud rate: 115200) για να δείτε τα logs σύνδεσης και τις μετρήσεις.
5.  **MQTT Control:**
    -   Στείλτε το μήνυμα `ledon` στο topic `homeassistant/sensor/uno_r4/state` για να ανάψετε το LED.
    -   Στείλτε το μήνυμα `ledoff` στο ίδιο topic για να το σβήσετε.

## 🌡️ Λογική Θερμοκρασίας
Το σύστημα διαβάζει τη θερμοκρασία κάθε 5 δευτερόλεπτα.
-   Αν **Temp >= 7°C**: Δημοσιεύει `LedOn` στο topic `homeassistant/sensor/uno_r4/set`.
-   Αν **Temp < 7°C**: Δημοσιεύει `LedOff` στο topic `homeassistant/sensor/uno_r4/set`.

---
*Αναπτύχθηκε για το project MQTT-2dev.*
