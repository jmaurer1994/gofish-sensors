#include "server.h"
#include "conf.h"
#include "scale.h"
#include "sensor.h"
#include "network.h"

AsyncWebServer server(WEBSERVER_PORT);

/******************************************************************
 * Route Handlers
 *
 *
 *
 */

///////////////////////////
// Scale
///////////////////////////
void handleTareScaleRequest(AsyncWebServerRequest *request) {
    tare_scale();
    request->send(200);
}

void handleTareResetRequest(AsyncWebServerRequest *request) {
    reset_tare_value();
    request->send(200);
}

void handleTareZeroRequest(AsyncWebServerRequest *request) {
    zero_tare_value();
    request->send(200);
}

//TODO: convert to a json
void handleReadScaleRequest(AsyncWebServerRequest *request) {
    if (!request->hasParam("samples")) {
        request->send(400); //no default value available
        return;
    }

    AsyncWebParameter *samplesParam = request->getParam("samples");

    long samples = samplesParam->value().toInt();

    if(!(samples > 0 && samples <= LOADCELL_MAX_SAMPLES_PER_REQUEST)){
        request->send(400); //outside of specified range
        return;
    }
    
    double avg_reading = get_avg_reading(samples);

    String returnval = String(avg_reading, 4);

    request->send(200, "text/plain", returnval);
    return;
}

///////////////////////////
// Force Sensor
/////////////////////////// 
void handleGetSensorReading(AsyncWebServerRequest *request){
    float reading = get_sensor_reading();
    request->send(200, "text/plain", String(reading, 5));
    return;
}

void handleGetRawSensorReading(AsyncWebServerRequest *request){
    int16_t raw_reading = get_raw_sensor_reading();
    request->send(200, "text/plain", String(raw_reading));
    return;
}

void handleGetCurrentComparatorThreshold(AsyncWebServerRequest *request){
    int16_t threshold_value = get_current_comparator_threshold();
    request->send(200, "text/plain", String(threshold_value));
    return;
}


///////////////////////////
// Default
///////////////////////////
void handleRoot(AsyncWebServerRequest *request) {
    request->send(200);
    return;
}

void handleHealthCheck(AsyncWebServerRequest *request) {
    request->send(200);
    return;
}
void handleTimeCheck(AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(get_epoch_time()));
    return;
}

void handleNotFound(AsyncWebServerRequest *request) {
    // Handle Unknown Request
    request->send(404);
    return;
}

bool initialize_server() {
    server.on("/", HTTP_GET, handleRoot);
    server.on("/", HTTP_OPTIONS, handleHealthCheck);
    server.on("/time", HTTP_GET, handleTimeCheck);

    server.on("/scale/tare/reset", HTTP_GET, handleTareResetRequest);
    server.on("/scale/tare/zero", HTTP_GET, handleTareZeroRequest);
    server.on("/scale/tare", HTTP_GET, handleTareScaleRequest);
    server.on("/scale/read", HTTP_GET, handleReadScaleRequest);

    server.on("/force_sensor/read", HTTP_GET,
              handleGetSensorReading);
    server.on("/force_sensor/read_raw", HTTP_GET,
              handleGetRawSensorReading);
    server.on("/force_sensor/current_threshold", HTTP_GET,
              handleGetCurrentComparatorThreshold);

    server.onNotFound(handleNotFound);

    server.begin();
    DEBUG_PRINTF1("Server listening on port %d\n", WEBSERVER_PORT);
    return true;
}