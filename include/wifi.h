#ifndef WIFI_H
#define WIFI_H

/**
 * dependencies
 */
#include "conf.h"

#include "AsyncTCP.h"
#include "ESPAsyncWebSrv.h"
#include <ArduinoOTA.h>
#include <NTPClient.h>
#include <WString.h>
#include <WiFi.h>
#include <WiFiUdp.h>

bool initialize_wifi();
void check_wifi_status();

uint64_t get_epoch_time();

#endif