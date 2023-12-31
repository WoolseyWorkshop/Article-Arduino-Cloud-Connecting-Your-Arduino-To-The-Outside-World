/**
 * @mainpage Arduino Cloud
 *
 * @section description_main Description
 * An Arduino sketch that demonstrates how to use the Arduino Cloud service.
 *
 * The sketch configures and connects the cloud variables representing the
 * built-in LED and the connected potentiometer to the service over WiFi.
 *
 * The LED and potentiometer can be controlled and monitored from a dashboard.
 *
 * The following cloud variables (linked local variables) are used within the
 * sketch.
 * - LEDState (ledState)
 * - PotentiometerValue (potValue)
 * - DebugMessage (debugMessage)
 *
 * @section circuit_main Circuit
 * - A 10 KΩ linear potentiometer is connected to pin *A1*.
 *
 * @section notes_main Notes
 * - Replace the secret placeholders (*secret_ssid*, *secret_optional_pass*, and
 *   *secret_device_key*) within the *arduino_secrets.h* header file with the
 *   appropriate values for your setup before running this sketch.
 * - Replace the *device_login_name* placeholder within the *thingProperties.h*
 *   header file with the appropriate value for your setup before running this
 *   sketch.
 * - Debug messages are printed in the Serial Monitor and sent to the Arduino
 *   Cloud dashboard when the *DEBUG* flag is enabled.
 *
 * Copyright (c) 2023 Woolsey Workshop.  All rights reserved.
 */


/**
 * @file Arduino_Cloud_LED_Pot.ino
 *
 * @brief The primary Arduino sketch implementation file.
 *
 * @section description_arduino_cloud_led_pot_ino Description
 * The primary Arduino sketch implementation file.
 *
 * @section libraries_arduino_cloud_led_pot_ino Libraries
 * - *arduino_secrets.h* Local Header File
 *   - Contains the secrets (usernames, passwords, and other parameters) used
 *     for accessing the local WiFi network and the Arduino Cloud service.
 * - *thingProperties.h* Local Header File
 *   - Contains the Arduino Cloud configuration and connection details.
 *   - Generated by Arduino and should not be edited by the user.
 *   - Requires the *ArduinoIoTCloud*, *Arduino_ConnectionHandler*,
 *     *Arduino_DebugUtils*, and *ArduinoMqttClient* libraries.
 *
 * @section notes_arduino_cloud_led_pot_ino Notes
 * - Comments are Doxygen compatible.
 *
 * @section todo_arduino_cloud_led_pot_ino TODO
 * - None.
 *
 * @section author_arduino_cloud_led_pot_ino Author(s)
 * - Created by John Woolsey on 09/09/2023.
 * - Modified by John Woolsey on 12/07/2023.
 *
 * Copyright (c) 2023 Woolsey Workshop.  All rights reserved.
 */


// Includes
#include "arduino_secrets.h"  // confidential authentication secrets
#include "thingProperties.h"  // Arduino Cloud configuration


// Defines
#define DEBUG 1                 ///< The mode of operation; 0 = normal, 1 = debug.
#define POT_SAMPLE_PERIOD 1000  ///< The potentiometer sampling period in milliseconds.


// Pin Mapping
const uint8_t Pot = A1;  ///< The pin connected to the 10 KΩ potentiometer.


/**
 * The standard Arduino setup function used for setup and configuration tasks.
 */
void setup() {
   // Serial Monitor
   Serial.begin(9600);
   delay(1500);  // wait for serial connection (recommended method by Arduino)

   // Pin configuration
   pinMode(LED_BUILTIN, OUTPUT);

   // Configure Arduino Cloud
   initProperties();

   // Connect Arduino Cloud
   ArduinoCloud.begin(ArduinoIoTPreferredConnection);

   // Arduino Cloud debug messages
   setDebugMessageLevel(2);  // 0 (errors only) - 4 (verbose messages)
   ArduinoCloud.printDebugInfo();

   // Show debug status
   if (DEBUG) {
      printDebugMessage("DEBUG mode is enabled.");  // does not show in dashboard (timing conflict?)
   } else {
      printDebugMessage("DEBUG mode is disabled.");  // shows in dashboard
   }
}


/**
 * The standard Arduino loop function used for repeating tasks.
 */
void loop() {
   ArduinoCloud.update();  // synchronize Arduino Cloud data

   // Process potentiometer
   static unsigned long previousPotReadTime = 0;  // retain the previous time the potentiometer was read
   unsigned long currentTime = millis();
   if (currentTime - previousPotReadTime >= POT_SAMPLE_PERIOD) {
      potValue = map(analogRead(Pot), 0, 1023, 0, 100);  // 10-bit ADC to percentage
      if (DEBUG) {
         printDebugMessage("Read potentiometer value of " + String(potValue) + ".");
      }
      previousPotReadTime = currentTime;
   }
}


/**
 * The callback function that executes each time the *LEDState* cloud variable
 * is updated in the dashboard.
 */
void onLedStateChange() {
   if (ledState) {
      digitalWrite(LED_BUILTIN, HIGH);
      if (DEBUG) {
         printDebugMessage("Turned on LED.");
      }
   } else {
      digitalWrite(LED_BUILTIN, LOW);
      if (DEBUG) {
         printDebugMessage("Turned off LED.");
      }
   }
}


/**
 * Prints the specified debug message to the Serial Monitor and updates the
 * *DebugMessage* cloud variable with the message.
 *
 * @param message  The debug message.
 */
void printDebugMessage(String message) {
   Serial.println(message);
   debugMessage = message;
   ArduinoCloud.update();
}
