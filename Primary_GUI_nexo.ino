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

#include <SPI.h>
#include <SuplaDevice.h>
#include <supla/network/esp_wifi.h>
#include <vector>

#include "SuplaConfigManager.h"
#include "SuplaConfigESP.h"
#include "SuplaSensor.h"
#include "SuplaWebServer.h"

SuplaWebServer webServer;
std::vector <DS18B20 *> sensor;

void setup() {
  Serial.begin(74880);

  SuplaConfigManager configManager;
  configManager.begin();

  SuplaConfigESP configESP = SuplaConfigESP(configManager);

  new Supla::ESPWifi(configManager.get(KEY_WIFI_SSID)->getValue(),
                     configManager.get(KEY_WIFI_PASS)->getValue());

  SuplaDevice.addRelay(5, true);           // ﻿44 - ﻿Pin number where the relay is connected

  for (int i = 0; i < configManager.get(KEY_MAX_DS18B20)->getValueInt(); ++i) {
    String ds_key = KEY_DS;
    ds_key += i;
    sensor.push_back(new DS18B20(configManager.get(KEY_GPIO_THERMOMETR)->getValueInt(), configManager.get(ds_key.c_str())->getValueBin(MAX_DS18B20_ADDRESS)));
  }

  SuplaDevice.begin((char*)configManager.get(KEY_SUPLA_GUID)->getValue(),      // Global Unique Identifier
                    (char*)configManager.get(KEY_SUPLA_SERVER)->getValue(),    // SUPLA server address
                    (char*)configManager.get(KEY_SUPLA_EMAIL)->getValue(),     // Email address used to login to Supla Cloud
                    (char*)configManager.get(KEY_SUPLA_AUTHKEY)->getValue());  // Authorization key

  webServer.begin(configManager, configESP);
}

void loop() {
  SuplaDevice.iterate();
  webServer.handleAPClient();
}
