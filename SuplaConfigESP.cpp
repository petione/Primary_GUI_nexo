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
#include "SuplaDevicePrimary.h"
#include "SuplaWebServer.h"

SuplaConfigESP::SuplaConfigESP() {
  Serial.begin(74880);
  configModeESP = NORMAL_MODE;

  if (ConfigManager->isDeviceConfigured()) {
    ConfigManager->setGUIDandAUTHKEY();
    ConfigManager->set(KEY_LOGIN, DEFAULT_LOGIN);
    ConfigManager->set(KEY_LOGIN_PASS, DEFAULT_LOGIN_PASS);
    ConfigManager->save();

    configModeInit();
  }

  SuplaDevice.setStatusFuncImpl(&status_func);
}

void SuplaConfigESP::addConfigESP(int _pinNumberConfig, int _pinLedConfig, int _modeConfigButton) {
  pinNumberConfig = _pinNumberConfig;
  pinLedConfig = _pinLedConfig;
  modeConfigButton = _modeConfigButton;

  if (pinLedConfig <= 0) {
    Serial.println("ESP  - status LED disabled");
  } else {
    pinMode(pinLedConfig, OUTPUT);
    digitalWrite(pinLedConfig, 1);
  }

  Supla::Control::Button *buttonConfig = new Supla::Control::Button(pinNumberConfig, true, true);

  if (modeConfigButton == CONFIG_MODE_10_ON_PRESSES) {
    buttonConfig->willTrigger(*ConfigESP, Supla::ON_PRESS, CONFIG_MODE_10_ON_PRESSES);
  }
  if (modeConfigButton == CONFIG_MODE_5SEK_HOLD) {
    buttonConfig->willTrigger(*ConfigESP, Supla::ON_PRESS, CONFIG_MODE_5SEK_HOLD);
    buttonConfig->willTrigger(*ConfigESP, Supla::ON_RELEASE, CONFIG_MODE_5SEK_HOLD);
  }
}

void SuplaConfigESP::trigger(int event, int action) {
  if (action == CONFIG_MODE_10_ON_PRESSES) {
    if (millis() - cnfigChangeTimeMs > 10000UL) {
      cnfigChangeTimeMs = millis();
      countPresses = 0;
    }
    countPresses++;

    if (countPresses == 10) {
      Serial.println("CONFIG_MODE_10_PRESSES");
      configModeInit();
      countPresses = 0;
    }
  }

  if (action == CONFIG_MODE_5SEK_HOLD) {
    if (event == Supla::ON_PRESS) {
      cnfigChangeTimeMs = millis();
    }
    if (event == Supla::ON_RELEASE) {
      if (millis() - cnfigChangeTimeMs > 5000UL) {
        Serial.println("CONFIG_MODE_5SEK_HOLD");
        configModeInit();
      }
    }
  }
}

void SuplaConfigESP::configModeInit() {
  configModeESP = CONFIG_MODE;
  ledBlinking(100);
  Serial.println("Configuration mode");
  WiFi.softAPdisconnect(true);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_AP_STA);

  Serial.print("Creating Access Point");
  Serial.print("Setting mode ... ");
  Serial.println(WiFi.mode(WIFI_AP_STA) ? "Ready" : "Failed!");

  while (!WiFi.softAP(CONFIG_WIFI_NAME, CONFIG_WIFI_PASS))
  {
    Serial.println(".");
    delay(100);
  }
  Serial.println("Network Created!");
  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());
}

const char *SuplaConfigESP::getLastStatusSupla() {
  return supla_status.msg;
}

void SuplaConfigESP::ledBlinking(int time) {
  os_timer_disarm(&led_timer);
  os_timer_setfn(&led_timer, ledBlinking_func, NULL);
  os_timer_arm (&led_timer, time, true);
}

void SuplaConfigESP::ledBlinkingStop(void) {
  os_timer_disarm(&led_timer);
  digitalWrite(pinLedConfig, 1);
}

int SuplaConfigESP::getPinLedConfig() {
  return pinLedConfig;
}

void ledBlinking_func(void *timer_arg) {
  int val = digitalRead(ConfigESP->getPinLedConfig());
  digitalWrite(ConfigESP->getPinLedConfig(), val == HIGH ? 0 : 1);
}

void status_func(int status, const char *msg) {
  switch (status) {
    case 2:
      ConfigESP->supla_status.msg = "Już zainicjalizowane";
      break;
    case 3:
      ConfigESP->supla_status.msg = "Nie przypisane CB";
      break;
    case 4:
      ConfigESP->supla_status.msg = "Nieprawidłowy identyfikator GUID lub rejestracja urządzeń NIEAKTYWNA";
      break;
    case 5:
      ConfigESP->supla_status.msg = "Nieznany adres serwera";
      break;
    case 6:
      ConfigESP->supla_status.msg = "Nieznany identyfikator ID";
      break;
    case 7:
      ConfigESP->supla_status.msg = "Zainicjowany";
      break;
    case 8:
      ConfigESP->supla_status.msg = "Przekroczono limit kanału";
      break;
    case 9:
      ConfigESP->supla_status.msg = "Rozłączony";
      break;
    case 10:
      ConfigESP->supla_status.msg = "Rejestracja w toku";
      break;
    case 11:
      ConfigESP->supla_status.msg = "Błąd zmiennej";
      break;
    case 12:
      ConfigESP->supla_status.msg = "Błąd wersji protokołu";
      break;
    case 13:
      ConfigESP->supla_status.msg = "Złe poświadczenia";
      break;
    case 14:
      ConfigESP->supla_status.msg = "Tymczasowo niedostępne";
      break;
    case 15:
      ConfigESP->supla_status.msg = "Konflikt lokalizacji";
      break;
    case 16:
      ConfigESP->supla_status.msg = "Konflikt kanałów";
      break;
    case 17:
      ConfigESP->supla_status.msg = "Zarejestrowany i gotowy";
      break;
    case 18:
      ConfigESP->supla_status.msg = "Urządzenie jest rozłączone";
      break;
    case 19:
      ConfigESP->supla_status.msg = "Lokalizacja jest wyłączona";
      break;
    case 20:
      ConfigESP->supla_status.msg = "Przekroczono limit urządzeń";
  }

  static int lock;
  if (status == 17 && ConfigESP->configModeESP == NORMAL_MODE) {
    ConfigESP->ledBlinkingStop();
    lock = 0;
  }
  else if (status != 17 && lock == 0 && ConfigESP->configModeESP == NORMAL_MODE) {
    ConfigESP->ledBlinking(500);
    lock = 1;
  }

  if (ConfigESP->supla_status.old_msg != ConfigESP->supla_status.msg) {
    ConfigESP->supla_status.old_msg = ConfigESP->supla_status.msg;
    ConfigESP->supla_status.status = status;
  }
}

SuplaConfigESP *ConfigESP;
