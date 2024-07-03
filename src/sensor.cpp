#include "sensor.h"
#include "conf.h"
#include "db.h"
#include "network.h"

ADS1115 ADS(0x48);

volatile bool sensor_activity = false;
volatile bool sampling_active = false;

int16_t current_comparator_threshold = 1000;
ForceEvent current_event;

void IRAM_ATTR SAMPLE_START_ISR() {
  if (!sampling_active) {
    sensor_activity = true;
  }
}

bool sample_force_sensor() {
  static int16_t last_reading = 0;

  // if there's no activity and no sampling happening return false
  if (!sensor_activity && !sampling_active) {
    return false; // no sensor activity & not sampling
  }

  if (sensor_activity && !sampling_active) {
    sampling_active = true;
    last_reading = 0;
    current_event.init_object(); // set timestamp and clear list
    DEBUG_PRINTF2("Started sampling at %d\tsamples: %d\n", current_event.get_timestamp(), current_event.samples_collected());
  }

  int16_t raw_reading = get_raw_sensor_reading();

  // if sensor is under the comparator threshold or sample cap, stop sampling
  if ((raw_reading < current_comparator_threshold) ||
      !(current_event.samples_collected() < 50)) {
    DEBUG_PRINTLN("DONE SAMPLING");
    sensor_activity = false; // reset int
    sampling_active = false;
    // if the numer of samples collected was over 0 send the data
    if (current_event.samples_collected() > 0) {
      DEBUG_PRINTLN("SENDING INSERT REQUEST");
      sendSensorEventInsertRequest(current_event);
    } // don't save phantom events (happens on wifi dc?)
    DEBUG_PRINTLN("NO LONGER SAMPLING");
    return false;
  }

  if (raw_reading != last_reading) {
    current_event.record_sample(raw_reading);
  }

  last_reading = raw_reading;

  return true;
}

int16_t get_raw_sensor_reading() { return ADS.getValue(); }

float get_sensor_reading() {
  int16_t raw = ADS.getValue();

  return ADS.toVoltage(raw);
}

void set_comparator_thresholds() {
  int16_t current_reading = get_raw_sensor_reading();
  int16_t new_threshold = (int16_t)((float)current_reading * 1.05) + 100;

  ADS.setComparatorThresholdHigh(new_threshold);
  ADS.setComparatorThresholdLow(new_threshold);

  ADS.requestADC(0);

  current_comparator_threshold = new_threshold;

  DEBUG_PRINTF2("Set ADC comparator threshold\n\tCurrent "
                "Reading:\t%d\n\tThreshold:\t%d\n",
                current_reading, new_threshold);
}

int16_t get_current_comparator_threshold() {
  return current_comparator_threshold;
}

bool initialize_force_sensor() {
  DEBUG_PRINTLN("Initializing external ADC");
  Wire.begin();
  ADS.begin();

  ADS.setMode(0);
  ADS.setGain(1);

  ADS.requestADC(0);

  DEBUG_PRINTLN("Configuring comparator");
  ADS.setComparatorMode(0);       // TRADITIONAL
  ADS.setComparatorLatch(0);      // NON-LATCH
  ADS.setComparatorQueConvert(0); // Trigger after 1 conversion?

  set_comparator_thresholds();

  pinMode(EADC_ALERT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(EADC_ALERT_PIN), SAMPLE_START_ISR,
                  RISING);

  if (!ADS.isConnected()) {
    DEBUG_PRINTLN("ADS not connected!");
  }

  DEBUG_PRINTLN("External ADC ready"); // Sends above settings to ADC

  return true;
}

void ForceEvent::record_sample(int16_t value) { samples.push_back(value); }

void ForceEvent::init_object() {
  timestamp = get_epoch_time();
  samples.clear();
}

uint64_t ForceEvent::get_timestamp() const { return timestamp; }
std::vector<int16_t> ForceEvent::get_samples() { return samples; }
uint16_t ForceEvent::samples_collected() { return samples.size(); }