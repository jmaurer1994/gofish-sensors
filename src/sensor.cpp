#include "sensor.h"
#include "conf.h"
ADS1115 ADS(0x48);

float peak_force = 0;
float total_force = 0;
int samples_collected = 0;

volatile bool sensor_activity = false;
volatile bool sampling_active = false;

std::vector<ForceEvent> forceEvents;

void IRAM_ATTR SAMPLE_START_ISR() {
    if (!sampling_active) {
        sensor_activity = true;
    }
}



bool sample_force_sensor() {
    if (!ADS.isConnected()) {
        DEBUG_PRINTLN("ADS not connected!");
        return false;
    }
    
    if (!sensor_activity && !sampling_active) {
        return false; // no sensor activity & not sampling
    }

    if (sensor_activity && !sampling_active) {
        DEBUG_PRINTLN("Started sampling");
        sampling_active = true;
    }

    // do sampling
    float force = ADS.getValue();
    if (force < 100) {
        // stop sampling
        if(samples_collected > 0){
        DEBUG_PRINTF3("Event recorded:\n\tAvg: %f\tpeak: %f\tsamples: %d\n",
                      total_force / samples_collected, peak_force,
                      samples_collected);
        }
        total_force = 0;
        peak_force = 0;
        samples_collected = 0;
        sensor_activity = false; // reset int
        sampling_active = false;
        return false;
    }

    if (force > peak_force) {
        peak_force = force;
    }

    total_force += force;
    samples_collected++;

    return true;
}

bool initialize_force_sensor() {
    Wire.begin();
    ADS.begin();

    ADS.setMode(0);
    ADS.setGain(1);

    float f = ADS.toVoltage(1);
    DEBUG_PRINTLN("Setting up comparator");

    ADS.setComparatorMode(0);  // TRADITIONAL
    ADS.setComparatorLatch(0); // NON-LATCH

    ADS.setComparatorThresholdHigh(0x0250);
    ADS.setComparatorThresholdLow(0x0100);

    ADS.setComparatorQueConvert(0); // Trigger after 1 conversion?

    ADS.requestADC(0);
    DEBUG_PRINTLN("Intialized ADC"); // Sends above settings to ADC

    pinMode(EADC_ALERT_PIN, INPUT_PULLUP);
    DEBUG_PRINTLN("Configured alert pin");

    attachInterrupt(digitalPinToInterrupt(EADC_ALERT_PIN), SAMPLE_START_ISR,
                    RISING);
   
    DEBUG_PRINTLN("Attached interrupt");

    return true;
}