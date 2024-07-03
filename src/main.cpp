#include "main.h"

void setup() {
    DEBUG_BEGIN(SERIAL_BAUD_RATE);

    delay(INITIAL_STARTUP_DELAY);
    DEBUG_PRINTLN("Starting up");

    if (!initialize_network()) {
        DEBUG_PRINTLN("Failed to initialize network");
    }
 
    if (!initialize_server()) {
        DEBUG_PRINTLN("Failed to initialize web server");
    }

    if (!initialize_force_sensor()) {
        DEBUG_PRINTLN("Failed to initialize force sensor");
    }

    if (!initialize_scale()) {
        DEBUG_PRINTLN("Failed to initialize scale");
    }

    if (!initialize_db_connection()) {
        DEBUG_PRINTLN("Failed to initialize db");
    }
    ArduinoOTA.begin();
    DEBUG_PRINTLN("Done setup");
}

void loop() {
    static uint32_t last_threshold_set_millis = millis();

    if (!sample_force_sensor()) {
        run_network_checks();
        if(millis() - last_threshold_set_millis > COMPARATOR_THRESHOLD_SET_MINUTES * 60 * 1000UL){
            set_comparator_thresholds();
            last_threshold_set_millis = millis();
        }
    }
}
