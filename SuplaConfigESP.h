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

enum ConfigMode { CONFIG_MODE_10_ON_PRESSES, CONFIG_MODE_5SEK_HOLD };

typedef enum _configModeESP {
  normal,
  mode_1,
  mode_2
};

typedef struct {
  int status;
  const char *msg;
  const char *old_msg;
} _supla_status;

class SuplaConfigESP : public Supla::Triggerable {
  public:
    SuplaConfigESP();

    void begin();
    void addConfigESP(int _pinNumberConfig, int _pinLedConfig, int _modeConfigButton);
    void trigger(int event, int action);

    void ledBlinking(int time);
    void ledBlinkingStop(void);
    
    int getPinLedConfig();
    int getModeConfigButton();
    const char *getLastStatusSupla();

    _configModeESP configModeESP;
    _supla_status supla_status;

  private:
    int pinNumberConfig;
    int pinLedConfig;
    int modeConfigButton;
    int countPresses = 0;
    unsigned long cnfigChangeTimeMs = 0;
    void configModeInit();
    ETSTimer led_timer;
};

void ledBlinking_func(void *timer_arg);
void status_func(int status, const char *msg);

extern SuplaConfigESP ConfigESP;
#endif //SuplaConfigESP_h
