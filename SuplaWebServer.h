/*
  Copyright (C) krycha88

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

#include "SuplaConfigESP.h"
#include "SuplaConfigManager.h"
#include <supla/channel.h>

#define  GUI_BLUE               "#005c96"
#define  GUI_GREEN              "#00D151"

#define  DEFAULT_LOGIN         "admin"
#define  DEFAULT_PASSWORD      "password"

#define  UPDATE_PATH           "/firmware"

class SuplaWebServer : public Supla::Element {
  public:
    SuplaWebServer();
    void begin();

  private:
    void iterateAlways();
    void handle();
    void handleWizardSave();
    void handleSave();
    void handleSearchDS();
    void handleDSSave();
    void handleFirmwareUp();
    
    void createWebServer();

    String supla_webpage_start(int save);
    String supla_webpage_search(int save);
    String supla_webpage_upddate();
    void supla_webpage_reboot();

    String getLogoSupla(void);

    void rebootESP();
    char* gui_color;
    char* gui_box_shadow = "box-shadow:0 1px 30px rgba(0,0,0,.9)";

    char www_username[MAX_MLOGIN];
    char www_password[MAX_MPASSWORD];
    char* update_path = UPDATE_PATH;

    char* Supported_Button[2] = {
      "MONOSTABLE",
      "BISTABLE"
    };

    char* Supported_MonostableTrigger[2] = {
      "ON_PRESS",
      "ON_RELEASE"
    };

    ESP8266WebServer httpServer = {80};
    ESP8266HTTPUpdateServer httpUpdater;
};

extern SuplaWebServer *WebServer;
#endif //SuplaWebServer_h
