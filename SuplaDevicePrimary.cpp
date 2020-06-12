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
#include <supla/network/esp_wifi.h>

#include "SuplaDevicePrimary.h"
#include "SuplaSensorDS18B20.h"
#include "SuplaConfigESP.h"
#include "SuplaConfigManager.h"
#include "SuplaWebServer.h"

SuplaDevicePrimaryClass::SuplaDevicePrimaryClass() {
  ConfigManager = new SuplaConfigManager();
  ConfigESP = new SuplaConfigESP();
  WebServer = new SuplaWebServer();
}

SuplaDevicePrimaryClass::~SuplaDevicePrimaryClass() {
  delete ConfigManager;
  delete ConfigESP;
  delete WebServer;
  delete this;
}

void SuplaDevicePrimaryClass::begin() {
  Supla::ESPWifi *wifi = new Supla::ESPWifi(ConfigManager->get(KEY_WIFI_SSID)->getValue(),
      ConfigManager->get(KEY_WIFI_PASS)->getValue());
  // wifi->setBufferSizes(1024, 256);

  //wifi->enableSSL(false);

  //String supla_hostname = ConfigManager->get(KEY_HOST_NAME)->getValue();
  //supla_hostname.replace(" ", "-");
  //wifi->setHostName(supla_hostname.c_str());

  SuplaDevice.setName((char*)ConfigManager->get(KEY_HOST_NAME)->getValue());

  SuplaDevice.begin((char*)ConfigManager->get(KEY_SUPLA_GUID)->getValue(),      // Global Unique Identifier
                    (char*)ConfigManager->get(KEY_SUPLA_SERVER)->getValue(),    // SUPLA server address
                    (char*)ConfigManager->get(KEY_SUPLA_EMAIL)->getValue(),     // Email address used to login to Supla Cloud
                    (char*)ConfigManager->get(KEY_SUPLA_AUTHKEY)->getValue());  // Authorization key

  ConfigManager->showAllValue();
  WebServer->begin();
}

void SuplaDevicePrimaryClass::addRelayButton(int pinRelay, int pinButton, bool highIsOn) {
  relay.push_back(new Supla::Control::Relay(pinRelay, highIsOn));
  button.push_back(new Supla::Control::Button(pinButton, true));
  int size = relay.size() - 1;

  if (ConfigManager->get(KEY_TYPE_BUTTON)->getValueElement(size)) {
    //BISTABILNY
    button[size]->willTrigger(*relay[size], Supla::ON_CHANGE, Supla::TOGGLE);
  } else {
    //MONOSTABILNY
    button[size]->willTrigger(*relay[size], ConfigManager->get(KEY_MONOSTABLE_TRIGGER)->getValueInt(), Supla::TOGGLE);
  }
}

void SuplaDevicePrimaryClass::addDS18B20MultiThermometer(int pinNumber) {
  for (int i = 0; i < ConfigManager->get(KEY_MAX_DS18B20)->getValueInt(); ++i) {
    String ds_key = KEY_DS;
    ds_key += i;
    sensorDS.push_back(new DS18B20(pinNumber, ConfigManager->get(ds_key.c_str())->getValueBin(MAX_DS18B20_ADDRESS)));
  }
}

void  SuplaDevicePrimaryClass::addConfigESP(int pinNumberConfig, int pinLedConfig, int modeConfigButton) {
  ConfigESP->addConfigESP(pinNumberConfig, pinLedConfig, modeConfigButton);
}

std::vector <Supla::Control::Relay *> relay;
std::vector <Supla::Control::Button *> button;

SuplaDevicePrimaryClass SuplaDevicePrimary;
