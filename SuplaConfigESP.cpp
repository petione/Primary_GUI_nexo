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

#include "SuplaConfigESP.h"

SuplaConfigESP::SuplaConfigESP() {
  this->configModeESP = normal;
}

SuplaConfigESP::SuplaConfigESP(const SuplaConfigManager& configManager) {
  this->configManager = configManager;

  if (!this->configManager.isDeviceConfigured()) {
    this->configModeESP = mode_1;
    this->configManager.setGUIDandAUTHKEY();
    this->configManager.set(KEY_WIFI_SSID, "xxx");
    this->configManager.set(KEY_WIFI_PASS, "xxx");
    this->configManager.set(KEY_LOGIN, "admin");
    this->configManager.set(KEY_LOGIN_PASS, "password");
    this->configManager.set(KEY_SUPLA_SERVER, "xxx.supla.org");
    this->configManager.set(KEY_SUPLA_EMAIL, "xxx@xxx.xxx");

    this->configManager.save();
  }
}

void SuplaConfigESP::configModeInit() {
  //  SuplaWebServer webServer;

  // supla_led_blinking(LED_CONFIG_PIN, 100);
  // my_mac_adress();
  Serial.print("Tryb konfiguracji: ");
  Serial.println(this->configModeESP);

  WiFi.softAPdisconnect(true);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(CONFIG_WIFI_NAME, CONFIG_WIFI_PASS);
  Serial.println("Tryb AP");
  //webServer.begin();
  Serial.println("Start Serwera");

  if (this->configModeESP == mode_2) {
    while (1) {
      //  webServer.handleAPClient();
    }
  }
}
