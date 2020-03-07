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

#ifndef SuplaWebServer_h
#define SuplaWebServer_h

#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <SuplaDevice.h>
#include <vector>

#include "SuplaConfigESP.h"
#include "SuplaConfigManager.h"
#include "SuplaSensor.h"

#define  GUI_BLUE               "#005c96"
#define  GUI_GREEN              "#00D151"

#define  DEFAULT_HOSTNAME      "Primary GUI"
#define  SUPLA_DEVICE_NAME     "SUPLA TESTY"
#define  CONFIG_WIFI_LOGIN     "MODUL SUPLA"
#define  CONFIG_WIFI_PASSWORD  ""

#define  DEFAULT_LOGIN         "admin"
#define  DEFAULT_PASSWORD      "password"

#define  UPDATE_PATH           "/firmware"

class  SuplaConfigManager;
class  SuplaConfigESP;

class SuplaWebServer {
  public:
    SuplaWebServer();
    void begin(SuplaConfigManager& configManager, SuplaConfigESP& configESP);
    void handleAPClient();

  private:
    SuplaConfigManager _configManager;
    SuplaConfigESP _configESP;
    
    char* gui_color;
    char* gui_box_shadow;
    const char * Supported_Gpio[18] = {
      "GPIO0 (D3)",
      "GPIO1 (TX)",
      "GPIO2 (D4)",
      "GPIO3 (RX)",
      "GPIO4 (D2)",
      "GPIO5 (D1)",
      "",
      "",
      "",
      "GPIO9 (FL-ESP8285)",
      "GPIO10 (FL-ESP8285)",
      "",
      "GPIO12 (D6)",
      "GPIO13 (D7)",
      "GPIO14 (D5)",
      "GPIO15 (D8)",
      "GPIO16 (D0)",
      "BRAK"
    };

    char www_username[MAX_MLOGIN];
    char www_password[MAX_MPASSWORD];
    char* update_path = UPDATE_PATH;

    String supla_webpage_save(void);

    ESP8266WebServer httpServer = {80};
    ESP8266HTTPUpdateServer httpUpdater;

    void handle();
    void set();
    void search();
    void setSearch();

    void handleNotFound();
    void createWebServer();
    String supla_webpage_start(int save);
    String supla_webpage_search(int save);

    String getLogoSupla(void);
    String getMacAddress();
};

extern SuplaWebServer WebServer;
#endif //SuplaWebServer_h
