#ifndef SENSOR_H
#define SENSOR_H

#include <ADS1X15.h>
#include <vector>

class ForceEvent {
  uint64_t timestamp;
  std::vector<int16_t> samples;

public:
  void record_sample(int16_t value);
  void init_object();
  void reset_object();
  uint64_t get_timestamp() const;
  std::vector<int16_t> get_samples();
  uint16_t samples_collected();
};

bool initialize_force_sensor();
bool sample_force_sensor();
int16_t get_raw_sensor_reading();
float get_sensor_reading();
void set_comparator_thresholds();
int16_t get_current_comparator_threshold();
#endif