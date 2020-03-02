#ifndef SuplaSensor_h
#define SuplaSensor_h

#include <Arduino.h>
#include <DallasTemperature.h>
#include <OneWire.h>

#include <supla-common/log.h>
#include <supla/sensor/thermometer.h>

class OneWireBus {
  public:
    OneWireBus(uint8_t pinNumber);
    int8_t getIndex(uint8_t *deviceAddress);

    uint8_t pin;
    OneWireBus * nextBus;
    unsigned long lastReadTime;
    DallasTemperature sensors;

  protected:
    OneWire oneWire;
};

class DS18B20 : public Thermometer() {
  public:
    DS18B20(uint8_t pin, uint8_t *deviceAddress = nullptr);
    void iterateAlways();
    double getValue();
    void onInit();

  protected:
    static OneWireBus *oneWireBus;
    OneWireBus *myBus;
    DeviceAddress address;
    int8_t retryCounter;
    double lastValidValue;
};

//OneWireBus *DS18B20::oneWireBus = nullptr;
#endif //SuplaSensor_h
