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

#include "SuplaSensor.h"
#include "SuplaConfigManager.h"
#include "SuplaWebServer.h"

SuplaWebServer webServer;
SuplaConfigManager configManager;
std::vector <DS18B20 *> sensor;

void setup() {

  Serial.begin(74880);

  configManager.ConfigMode = mode_1;

  Serial.print("Tryb konfiguracji: "); Serial.println(configManager.ConfigMode);

  configManager.addKey(KEY_SUPLA_GUID, MAX_GUID);
  configManager.addKey(KEY_SUPLA_AUTHKEY, MAX_AUTHKEY);
  configManager.addKey(KEY_WIFI_SSID, MAX_SSID);
  configManager.addKey(KEY_WIFI_PASS, MAX_PASSWORD);
  configManager.addKey(KEY_LOGIN, MAX_MLOGIN);
  configManager.addKey(KEY_LOGIN_PASS, MAX_MPASSWORD);
  configManager.addKey(KEY_HOST_NAME, MAX_HOSTNAME);
  configManager.addKey(KEY_SUPLA_SERVER, MAX_SUPLA_SERVER);
  configManager.addKey(KEY_SUPLA_EMAIL, MAX_EMAIL);
  configManager.addKeyAndRead(KEY_MAX_DS18B20, "1", sizeof(int));

  for (int i = 0; i < configManager.get(KEY_MAX_DS18B20)->getValueInt(); ++i)
  {
    String ds_key = KEY_DS;
    String ds_name_key = KEY_DS_NAME;
    ds_key += i;
    ds_name_key += i;

    configManager.addKey(ds_key.c_str(), MAX_DS18B20_ADDRESS_HEX);
    configManager.addKey(ds_name_key.c_str(), MAX_DS18B20_NAME);
  }

  configManager.load();

  if (!configManager.isDeviceConfigured()) {
    configManager.setGUIDandAUTHKEY();
    configManager.set(KEY_WIFI_SSID, "wifi");
    configManager.set(KEY_WIFI_PASS, "password");
    configManager.set(KEY_LOGIN, "admin");
    configManager.set(KEY_LOGIN_PASS, "password");
    configManager.set(KEY_SUPLA_SERVER, "xxx.supla.org");
    configManager.set(KEY_SUPLA_EMAIL, "xxxk@xx.xx);

    configManager.save();
  }

  new Supla::ESPWifi(configManager.get(KEY_WIFI_SSID)->getValue(),
                     configManager.get(KEY_WIFI_PASS)->getValue());

  SuplaDevice.addRelay(5, true);           // ﻿44 - ﻿Pin number where the relay is connected

  for (int i = 0; i < configManager.get(KEY_MAX_DS18B20)->getValueInt(); ++i)
  {
    String ds_key = KEY_DS;
    ds_key += i;
    sensor.push_back(new DS18B20(2, configManager.get(ds_key.c_str())->getValueBin(MAX_DS18B20_ADDRESS)));
  }

  SuplaDevice.begin((char*)configManager.get(KEY_SUPLA_GUID)->getValue(),      // Global Unique Identifier
                    (char*)configManager.get(KEY_SUPLA_SERVER)->getValue(),    // SUPLA server address
                    (char*)configManager.get(KEY_SUPLA_EMAIL)->getValue(),     // Email address used to login to Supla Cloud
                    (char*)configManager.get(KEY_SUPLA_AUTHKEY)->getValue());  // Authorization key

  webServer.begin();
}

void loop() {
  SuplaDevice.iterate();
  webServer.handleAPClient();
}
