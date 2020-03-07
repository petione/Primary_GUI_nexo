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
#include "SuplaConfigManager.h"
#include "SuplaWebServer.h"

SuplaConfigESP::SuplaConfigESP() {
  this->configModeESP = normal;
}

SuplaConfigESP::SuplaConfigESP(SuplaConfigManager* configManager) {
  _configManager = configManager;
  this->configModeESP = normal;

  if (!_configManager->isDeviceConfigured()) {
    this->configModeESP = mode_1;
    _configManager->setGUIDandAUTHKEY();
    _configManager->set(KEY_WIFI_SSID, "Gargulec");
    _configManager->set(KEY_WIFI_PASS, "renaultmegane");
    _configManager->set(KEY_LOGIN, "admin");
    _configManager->set(KEY_LOGIN_PASS, "password");
    _configManager->set(KEY_SUPLA_SERVER, "svr19.supla.org");
    _configManager->set(KEY_SUPLA_EMAIL, "krystian.franieczek@gmail.com");

    _configManager->save();

    this->configModeInit();
  }
}

void SuplaConfigESP::configModeInit() {
  // supla_led_blinking(LED_CONFIG_PIN, 100);
  // my_mac_adress();
  Serial.print("Tryb konfiguracji: ");
  Serial.println(this->configModeESP);

  WiFi.softAPdisconnect(true);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(CONFIG_WIFI_NAME, CONFIG_WIFI_PASS);
  Serial.println("Tryb AP");
  //webServer->begin();
  Serial.println("Start Serwera");
}

SuplaConfigESP ConfigESP;
