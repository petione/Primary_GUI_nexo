#ifndef SuplaSensor_h
#define SuplaSensor_h

#include <supla/sensor/DS18B20.h>

class DS18B20 : public Supla::Sensor::DS18B20 {
  public:
    DS18B20(uint8_t pin, uint8_t *deviceAddress = nullptr)
      : Supla::Sensor::DS18B20(pin, deviceAddress) {};
      
    uint8_t  getPin() {
      return myBus->pin;
    }

};

#endif //SuplaSensor_h
