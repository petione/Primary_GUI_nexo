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
#include "SuplaWebServer.h"
#include "SuplaConfigManager.h"

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <SuplaDevice.h>

SuplaConfigESP::SuplaConfigESP() {
  configModeESP = normal;

  SuplaDevice.setStatusFuncImpl(&status_func);
}

void SuplaConfigESP::begin() {
  if (ConfigManager.isDeviceConfigured()) {
    Serial.println("Ustawiono dane domyślne");

    configModeESP = mode_1;

    ConfigManager.setGUIDandAUTHKEY();
    ConfigManager.set(KEY_LOGIN, "admin");
    ConfigManager.set(KEY_LOGIN_PASS, "password");

    ConfigManager.save();

    configModeInit();
  }
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

  SuplaConfigESP *configModeESP = new SuplaConfigESP;
  Supla::Control::Button *buttonConfig = new Supla::Control::Button(pinNumberConfig, true, true);

  if (modeConfigButton == CONFIG_MODE_10_ON_PRESSES) {
    buttonConfig->willTrigger(*configModeESP, Supla::ON_PRESS, CONFIG_MODE_10_ON_PRESSES);
  }
  if (modeConfigButton == CONFIG_MODE_5SEK_HOLD) {
    buttonConfig->willTrigger(*configModeESP, Supla::ON_PRESS, CONFIG_MODE_5SEK_HOLD);
    buttonConfig->willTrigger(*configModeESP, Supla::ON_RELEASE, CONFIG_MODE_5SEK_HOLD);
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
      }
    }
  }
}

void SuplaConfigESP::configModeInit() {
  ledBlinking(100);
  Serial.print("Tryb konfiguracji: ");
  Serial.println(configModeESP);

  WiFi.softAPdisconnect(true);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(CONFIG_WIFI_NAME, CONFIG_WIFI_PASS);
  Serial.println("Tryb AP");
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

int SuplaConfigESP::getModeConfigButton() {
  return modeConfigButton;
}

const char *SuplaConfigESP::getLastStatusSupla() {
  return supla_status.msg;
}

void ledBlinking_func(void *timer_arg) {
  int val = digitalRead(ConfigESP.getPinLedConfig());
  digitalWrite(ConfigESP.getPinLedConfig(), val == HIGH ? 0 : 1);
}

void status_func(int status, const char *msg) {
  switch (status) {
    case 2:
      ConfigESP.supla_status.msg = "Już zainicjalizowane";
      break;
    case 3:
      ConfigESP.supla_status.msg = "Nie przypisane CB";
      break;
    case 4:
      ConfigESP.supla_status.msg = "Nieprawidłowy identyfikator GUID lub rejestracja urządzeń NIEAKTYWNA";
      break;
    case 5:
      ConfigESP.supla_status.msg = "Nieznany adres serwera";
      break;
    case 6:
      ConfigESP.supla_status.msg = "Nieznany identyfikator ID";
      break;
    case 7:
      ConfigESP.supla_status.msg = "Zainicjowany";
      break;
    case 8:
      ConfigESP.supla_status.msg = "Przekroczono limit kanału";
      break;
    case 9:
      ConfigESP.supla_status.msg = "Rozłączony";
      break;
    case 10:
      ConfigESP.supla_status.msg = "Rejestracja w toku";
      break;
    case 11:
      ConfigESP.supla_status.msg = "Błąd zmiennej";
      break;
    case 12:
      ConfigESP.supla_status.msg = "Błąd wersji protokołu";
      break;
    case 13:
      ConfigESP.supla_status.msg = "Złe poświadczenia";
      break;
    case 14:
      ConfigESP.supla_status.msg = "Tymczasowo niedostępne";
      break;
    case 15:
      ConfigESP.supla_status.msg = "Konflikt lokalizacji";
      break;
    case 16:
      ConfigESP.supla_status.msg = "Konflikt kanałów";
      break;
    case 17:
      ConfigESP.supla_status.msg = "Zarejestrowany i gotowy";
      break;
    case 18:
      ConfigESP.supla_status.msg = "Urządzenie jest rozłączone";
      break;
    case 19:
      ConfigESP.supla_status.msg = "Lokalizacja jest wyłączona";
      break;
    case 20:
      ConfigESP.supla_status.msg = "Przekroczono limit urządzeń";
  }

  static int lock;
  if (status == 17 && ConfigESP.configModeESP == normal) {
    ConfigESP.ledBlinkingStop();
    lock = 0;
  }
  else if (status != 17 && lock == 0 && ConfigESP.configModeESP == normal) {
    ConfigESP.ledBlinking(500);
    lock = 1;
  }

  if (ConfigESP.supla_status.old_msg != ConfigESP.supla_status.msg) {
    ConfigESP.supla_status.old_msg = ConfigESP.supla_status.msg;
    ConfigESP.supla_status.status = status;
  }
}

SuplaConfigESP ConfigESP;
