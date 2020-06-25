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

#include <supla/element.h>
#include "SuplaConfigManager.h"

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

    const String SuplaMetas();
    const String SuplaStyle();
    const String SuplaFavicon();
    const String SuplaLogo();
    const String SuplaSummary();
    const String SuplaJavaScript();
    const String SuplaCopyrightBar();

    void rebootESP();

    void sendContent(const String content);
    void redirectToIndex();

    const char* Supported_Button[2] = {
      "MONOSTABLE",
      "BISTABLE"
    };

    const char* Supported_MonostableTrigger[2] = {
      "ON_RELEASE",
      "ON_PRESS"
    };

  //  char* gui_color;

    char www_username[MAX_MLOGIN];
    char www_password[MAX_MPASSWORD];
    char* update_path = (char*)UPDATE_PATH;

    ESP8266WebServer httpServer = {80};
    ESP8266HTTPUpdateServer httpUpdater;
};

#endif //SuplaWebServer_h
