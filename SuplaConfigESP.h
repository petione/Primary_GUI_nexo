/*
  Copyright (C) AC SOFTWARE SP. Z O.O.

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef SuplaConfigESP_h
#define SuplaConfigESP_h

#define CONFIG_WIFI_NAME  "SUPLA"
#define CONFIG_WIFI_PASS  ""

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <SuplaDevice.h>
#include <supla/control/button.h>

#include "SuplaConfigManager.h"

enum _configModeESP { NORMAL_MODE, CONFIG_MODE };

typedef struct {
  int status;
  const char *msg;
  const char *old_msg;
} _supla_status;

class SuplaConfigESP : public Supla::Triggerable {
  public:
    SuplaConfigESP();

    void addConfigESP(int _pinNumberConfig, int _pinLedConfig, int _modeConfigButton);
    void trigger(int event, int action);

    const char *getLastStatusSupla();

    void ledBlinking(int time);
    void ledBlinkingStop(void);
    int getPinLedConfig();

    _configModeESP configModeESP;
    _supla_status supla_status;

  private:
    void configModeInit();

    int pinNumberConfig;
    int pinLedConfig;
    int modeConfigButton;
    int countPresses = 0;
    unsigned long cnfigChangeTimeMs = 0;

    ETSTimer led_timer;
};

void ledBlinking_func(void *timer_arg);
void status_func(int status, const char *msg);

extern SuplaConfigESP *ConfigESP;
#endif //SuplaConfigESP_h
