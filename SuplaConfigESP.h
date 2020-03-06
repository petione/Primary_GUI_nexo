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

#include "SuplaConfigManager.h"
//#include "SuplaWebServer.h"

typedef enum _configModeESP {
  normal,
  mode_1,
  mode_2
};

class SuplaConfigESP {
  public:
    SuplaConfigESP();
    SuplaConfigESP(const SuplaConfigManager& configManager);
    
    void configModeInit();
    _configModeESP configModeESP;
  private:
    SuplaConfigManager configManager;

};

#endif //SuplaConfigESP_h
