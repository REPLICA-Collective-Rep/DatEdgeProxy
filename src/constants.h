#pragma once
#define DATE_NUM_CHANNELS 8

struct SensorData {
    int device;
    int mscounter;
    float raw[DATE_NUM_CHANNELS];
};