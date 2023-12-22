#ifndef SENSOR_H
#define SENSOR_H

#include "conf.h"

#include <ADS1X15.h>
#include <vector>

class ForceEvent {
    uint64_t timestamp;
    std::vector<float> samples;

  public:
    void record_sample(float value) { samples.push_back(value); }
    void init_object() { timestamp = micros(); }
    void reset_object() { samples.clear(); }

    int samples_collected() { return samples.size(); }
};

bool initialize_force_sensor();
bool sample_force_sensor();
void clear_events();
#endif