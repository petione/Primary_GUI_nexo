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

#include <Arduino.h>
#include <OneWire.h>

#include "SuplaWebServer.h"
#include "SuplaConfigESP.h"
#include "SuplaConfigManager.h"
#include "SuplaSensorDS18B20.h"
#include "SuplaDeviceGUI.h"

SuplaWebServer::SuplaWebServer() {
}

void SuplaWebServer::begin() {
  this->createWebServer();

  strcpy(this->www_username, ConfigManager->get(KEY_LOGIN)->getValue());
  strcpy(this->www_password, ConfigManager->get(KEY_LOGIN_PASS)->getValue());

  httpUpdater.setup(&httpServer, UPDATE_PATH, www_username, www_password);
  httpServer.begin();
}

void SuplaWebServer::iterateAlways() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    this->gui_color = GUI_BLUE;
  } else {
    this->gui_color = GUI_GREEN;
  }

  httpServer.handleClient();
}

void SuplaWebServer::createWebServer() {
  httpServer.on("/", HTTP_GET, std::bind(&SuplaWebServer::handle, this));
  httpServer.on("/", HTTP_POST, std::bind(&SuplaWebServer::handleWizardSave, this));
  httpServer.on("/set", std::bind(&SuplaWebServer::handleSave, this));
  httpServer.on("/search", std::bind(&SuplaWebServer::handleSearchDS, this));
  httpServer.on("/setSearch", std::bind(&SuplaWebServer::handleDSSave, this));
  httpServer.on("/firmware_up", std::bind(&SuplaWebServer::handleFirmwareUp, this));
  httpServer.on("/rbt", std::bind(&SuplaWebServer::supla_webpage_reboot, this));
}

void SuplaWebServer::handle() {
  Serial.println("HTTP_GET - metoda handle");
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!httpServer.authenticate(this->www_username, this->www_password))
      return httpServer.requestAuthentication();
  }
  httpServer.send(200, "text/html", supla_webpage_start(0));
}

void SuplaWebServer::handleWizardSave() {
  Serial.println("HTTP_POST - metoda handleWizardSave");

  if (strcmp(httpServer.arg("rbt").c_str(), "1") == 0) {
    Serial.println("RESTART ESP");
    rebootESP();
    return;
  }

  ConfigManager->set(KEY_WIFI_SSID, httpServer.arg("sid").c_str());
  ConfigManager->set(KEY_WIFI_PASS, httpServer.arg("wpw").c_str());
  ConfigManager->set(KEY_SUPLA_SERVER, httpServer.arg("svr").c_str());
  ConfigManager->set(KEY_SUPLA_EMAIL, httpServer.arg("eml").c_str());

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      Serial.println("E_CONFIG_OK: Config save");
      httpServer.send(200, "text/html", supla_webpage_start(1));
    case E_CONFIG_FILE_OPEN:
      Serial.println("E_CONFIG_FILE_OPEN: Couldn't open file");
      httpServer.send(200, "text/html", supla_webpage_start(4));
  }
}

void SuplaWebServer::handleSave() {
  Serial.println("HTTP_POST - metoda handleSave");
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!httpServer.authenticate(this->www_username, this->www_password))
      return httpServer.requestAuthentication();
  }

  ConfigManager->set(KEY_WIFI_SSID, httpServer.arg("sid").c_str());
  ConfigManager->set(KEY_WIFI_PASS, httpServer.arg("wpw").c_str());
  ConfigManager->set(KEY_SUPLA_SERVER, httpServer.arg("svr").c_str());
  ConfigManager->set(KEY_SUPLA_EMAIL, httpServer.arg("eml").c_str());
  ConfigManager->set(KEY_HOST_NAME, httpServer.arg("supla_hostname").c_str());
  ConfigManager->set(KEY_LOGIN, httpServer.arg("modul_login").c_str());
  ConfigManager->set(KEY_LOGIN_PASS, httpServer.arg("modul_pass").c_str());
  ConfigManager->set(KEY_MONOSTABLE_TRIGGER, httpServer.arg("trigger_set").c_str());

  String button_value;
  for (int i = 0; i < Supla::GUI::button.size(); ++i) {
    String button = "button_set";
    button += i;
    button_value += httpServer.arg(button).c_str();
  }
  ConfigManager->set(KEY_TYPE_BUTTON, button_value.c_str());

  if (strcmp(httpServer.arg("max_ds18b20").c_str(), "") != 0) {
    ConfigManager->set(KEY_MAX_DS18B20, httpServer.arg("max_ds18b20").c_str());
  }

  for (int i = 0; i < ConfigManager->get(KEY_MAX_DS18B20)->getValueInt(); i++) {
    String ds_key = KEY_DS;
    String ds_name_key = KEY_DS_NAME;
    ds_key += i;
    ds_name_key += i;

    String ds = "ds18b20_channel_id_";
    String ds_name = "ds18b20_name_id_";
    ds += i;
    ds_name += i;

    ConfigManager->set(ds_key.c_str(), httpServer.arg(ds).c_str());
    ConfigManager->set(ds_name_key.c_str(), httpServer.arg(ds_name).c_str());
  }

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      Serial.println("E_CONFIG_OK: Dane zapisane");
      httpServer.send(200, "text/html", supla_webpage_start(5));
    case E_CONFIG_FILE_OPEN:
      Serial.println("E_CONFIG_FILE_OPEN: Couldn't open fileeeeeeeeee");
      httpServer.send(200, "text/html", supla_webpage_start(4));
  }
}

void SuplaWebServer::handleSearchDS() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!httpServer.authenticate(www_username, www_password))
      return httpServer.requestAuthentication();
  }
  httpServer.send(200, "text/html", supla_webpage_search(0));
}

void SuplaWebServer::handleDSSave() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!httpServer.authenticate(www_username, www_password))
      return httpServer.requestAuthentication();
  }
  for (int i = 0; i < ConfigManager->get(KEY_MAX_DS18B20)->getValueInt(); i++) {
    String ds_key = KEY_DS;
    ds_key += i;

    String ds = "ds18b20_channel_id_";
    ds += i;

    String address = httpServer.arg(ds).c_str();
    if (address != NULL) {
      ConfigManager->set(ds_key.c_str(), address.c_str());
    }
  }

  switch (ConfigManager->save()) {
    case E_CONFIG_OK:
      Serial.println("E_CONFIG_OK: Config save");
      httpServer.send(200, "text/html", supla_webpage_search(5));
      rebootESP();
    case E_CONFIG_FILE_OPEN:
      Serial.println("E_CONFIG_FILE_OPEN: Couldn't open file");
      httpServer.send(200, "text/html", supla_webpage_search(2));
  }
}

void SuplaWebServer::handleFirmwareUp() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!httpServer.authenticate(www_username, www_password))
      return httpServer.requestAuthentication();
  }
  httpServer.send(200, "text/html", supla_webpage_upddate());
}

String SuplaWebServer::supla_webpage_start(int save) {
  String content = "";

  content = "<!DOCTYPE HTML>";
  content += "<meta http-equiv='content-type' content='text/html'; charset='UTF-8'>";
  content += "<meta name='viewport' content='width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no'>";
  content += "<style>body{font-size:14px;font-family:HelveticaNeue,'Helvetica Neue',HelveticaNeueRoman,HelveticaNeue-Roman,'Helvetica Neue Roman',TeXGyreHerosRegular,Helvetica,Tahoma,Geneva,Arial,sans-serif;font-weight:400;font-stretch:normal;background:" + String(gui_color) + ";color:#fff;line-height:20px;padding:0}.s{width:460px;margin:0 auto;margin-top:calc(50vh - 340px);border:solid 3px #fff;padding:0 10px 10px;border-radius:3px}#l{display:block;max-width:150px;height:155px;margin:-80px auto 20px;background:" + String(gui_color) + ";padding-right:5px}#l path{fill:#000}.w{margin:3px 0 16px;padding:5px 0;border-radius:3px;background:#fff;" + String(gui_box_shadow) + "}h1,h3{margin:10px 8px;font-family:HelveticaNeueLight,HelveticaNeue-Light,'Helvetica Neue Light',HelveticaNeue,'Helvetica Neue',TeXGyreHerosRegular,Helvetica,Tahoma,Geneva,Arial,sans-serif;font-weight:300;font-stretch:normal;color:#000;font-size:23px}h1{margin-bottom:14px;color:#fff}span{display:block;margin:10px 7px 14px}i{display:block;font-style:normal;position:relative;border-bottom:solid 1px " + String(gui_color) + ";height:42px}i:last-child{border:none}label{position:absolute;display:inline-block;top:0;left:8px;color:" + String(gui_color) + ";line-height:41px;pointer-events:none}input,select{width:calc(100% - 145px);border:none;font-size:16px;line-height:40px;border-radius:0;letter-spacing:-.5px;background:#fff;color:#000;padding-left:144px;-webkit-appearance:none;-moz-appearance:none;appearance:none;outline:0!important;height:40px}select{padding:0;float:right;margin:1px 3px 1px 2px}button{width:100%;border:0;background:#000;padding:5px 10px;font-size:16px;line-height:40px;color:#fff;border-radius:3px;" + String(gui_box_shadow) + ";cursor:pointer}.c{background:#ffe836;position:fixed;width:100%;line-height:80px;color:#000;top:0;left:0;" + String(gui_box_shadow) + ";text-align:center;font-size:26px;z-index:100}@media all and (max-height:920px){.s{margin-top:80px}}@media all and (max-width:900px){.s{width:calc(100% - 20px);margin-top:40px;border:none;padding:0 8px;border-radius:0}#l{max-width:80px;height:auto;margin:10px auto 20px}h1,h3{font-size:19px}i{border:none;height:auto}label{display:block;margin:4px 0 12px;color:" + String(gui_color) + ";font-size:13px;position:relative;line-height:18px}input,select{width:calc(100% - 10px);font-size:16px;line-height:28px;padding:0 5px;border-bottom:solid 1px " + String(gui_color) + "}select{width:100%;float:none;margin:0}}</style>";
  if (save == 1) {
    content += "<div id=\"msg\" class=\"c\">data saved</div>";
  } else if (save == 2) {
    content += "<div id=\"msg\" class=\"c\">Restart modułu</div>";
    content += "<script type='text/javascript'>setTimeout(function(){location.href='/'} , 3000);</script>";
  } else if (save == 3) {
    content += "<div id=\"msg\" class=\"c\">Dane wymazane - należy zrobić restart urządzenia</div>";
  } else if (save == 4) {
    content += "<div id=\"msg\" class=\"c\">Błąd zapisu - nie można odczytać pliku - brak partycji FS.</div>";
  }  else if (save == 5) {
    content += "<div id=\"msg\" class=\"c\">Dane zapisane - restart modułu.</div>";
    content += "<script type='text/javascript'>setTimeout(function(){location.href='/'} , 3000);</script>";
  }

  content += "<script type='text/javascript'>setTimeout(function(){var element =  document.getElementById('msg');if ( element != null ) element.style.visibility = 'hidden';},3200);</script>";
  content += "<div class='s'>";
  content += getLogoSupla();
  content += "<h1>" + String(ConfigManager->get(KEY_HOST_NAME)->getValue()) + " by krycha</h1>";
  content += "<span>LAST STATE: " + String(ConfigESP->getLastStatusSupla()) + "<br>";
  content += "Firmware: SuplaDevice " + String(Supla::Channel::reg_dev.SoftVer) + "<br>";
  content += "GUID: " + String(ConfigManager->get(KEY_SUPLA_GUID)->getValueHex(SUPLA_GUID_SIZE)) + "<br>";
  content += "MAC: " + String(ConfigESP->getMacAddress(true)) + "</span>";
  //content += "<font size='2'>RSSI: " + read_rssi() + "</font>";

  content += "<form method='post' action='set'>";
  content += "<div class='w'>";
  content += "<h3>Ustawienia WIFI</h3>";
  content += "<i><input name='sid' value='" + String(ConfigManager->get(KEY_WIFI_SSID)->getValue()) + "'length=";
  content += MAX_SSID;
  content += "><label>Nazwa sieci</label></i>";
  content += "<i><input name='wpw' ";
  if (ConfigESP->configModeESP != NORMAL_MODE) {
    content += "type='password' ";
  }
  content += "value='" + String(ConfigManager->get(KEY_WIFI_PASS)->getValue()) + "'";

  if (ConfigManager->get(KEY_WIFI_PASS)->getValue() != 0) {
    content += ">";
  }
  else {
    content += "'minlength='";
    content += MIN_PASSWORD;
    content += "' required length=";
    content += MAX_PASSWORD;
    content += ">";
  }
  content += "<label>Hasło</label></i>";
  content += "<i><input name='supla_hostname' value='";
  content += ConfigManager->get(KEY_HOST_NAME)->getValue();
  content += "'length=";
  content += MAX_HOSTNAME;
  content += " placeholder='Nie jest wymagana'><label>Nazwa modułu</label></i>";
  content += "</div>";

  content += "<div class='w'>";
  content += "<h3>Ustawienia SUPLA</h3>";
  content += "<i><input name='svr' value='" + String(ConfigManager->get(KEY_SUPLA_SERVER)->getValue()) + "'length=";
  content += MAX_SUPLA_SERVER;
  content += "><label>Adres serwera</label></i>";
  content += "<i><input name='eml' value='" + String(ConfigManager->get(KEY_SUPLA_EMAIL)->getValue())  + "'length=";
  content += MAX_EMAIL;
  content += "><label>Email</label></i>";
  content += "</div>";

  content += "<div class='w'>";
  content += "<h3>Ustawienia administratora</h3>";
  content += "<i><input name='modul_login' value='" + String(ConfigManager->get(KEY_LOGIN)->getValue()) + "'length=";
  content += MAX_MLOGIN;
  content += "><label>Login</label></i>";
  content += "<i><input name='modul_pass' ";
  if (ConfigESP->configModeESP != NORMAL_MODE) {
    content += "type='password' ";
  }
  content += "value='" + String(ConfigManager->get(KEY_LOGIN_PASS)->getValue()) + "'";

  if (ConfigManager->get(KEY_LOGIN_PASS)->getValue() != 0) {
    content += ">";
  }
  else {
    content += "'minlength='";
    content += MIN_PASSWORD;
    content += "' required length=";
    content += MAX_MPASSWORD;
    content += ">";
  }
  content += "<label>Hasło</label></i>";
  content += "</div>";


  //DS****************************************************************************
  if (!Supla::GUI::sensorDS.empty()) {
    content += "<div class='w'>";
    content += "<h3>Temperatura</h3>";
    //for (auto channel = Supla::Channel::begin(); channel != nullptr; channel = channel->next()) {
    //    channel->doSomething();
    //if (channel->getChannelType() == SUPLA_CHANNELTYPE_THERMOMETER) {
    for (int i = 0; i < ConfigManager->get(KEY_MAX_DS18B20)->getValueInt(); i++) {
      String ds_key = KEY_DS;
      String ds_name_key = KEY_DS_NAME;
      ds_key += i;
      ds_name_key += i;

      double temp = Supla::GUI::sensorDS[i]->getValue();
      content += "<i><input name='ds18b20_name_id_";
      content += i;
      content += "' value='" + String(ConfigManager->get(ds_name_key.c_str())->getValue()) + "' maxlength=";
      content += MAX_DS18B20_NAME;
      content += "><label>";
      content += "Nazwa ";
      content += i + 1;
      content += "</label></i>";
      content += "<i><input name='ds18b20_channel_id_";
      content += i;
      content += "' value='" + String(ConfigManager->get(ds_key.c_str())->getValue()) + "' maxlength=";
      content += MAX_DS18B20_ADDRESS_HEX;
      content += "><label>";
      if (temp != -275)content += temp;
      else content += "--.--";
      content += " <b>&deg;C</b> ";
      content += "</label></i>";
      //}
    }
    content += "</div>";
  }

  //*****************************************************************************************
  content += "<div class='w'>";
  content += "<h3>Ustawienia modułu</h3>";
  if (!Supla::GUI::sensorDS.empty()) {
    content += "<i><label>MAX DS18b20</label><input name='max_ds18b20' type='number' placeholder='1' step='1' min='1' max='20' value='" + String(ConfigManager->get(KEY_MAX_DS18B20)->getValue()) + "'></i>";
  }

  //button***************************************************************************************
  if (!Supla::GUI::button.empty()) {
    for (int i = 0; i < Supla::GUI::button.size(); i++) {
      int select_button = ConfigManager->get(KEY_TYPE_BUTTON)->getValueElement(i);
      content += "<i><label>Button action IN";
      content += i + 1;
      content += "</label><select name='button_set";
      content += i;
      content += "'>";

      for (int suported_button = 0; suported_button < sizeof(Supported_Button) / sizeof(char*); suported_button++) {
        content += "<option value='";
        content += suported_button;
        if (select_button == suported_button) {
          content += "' selected>";
        }
        else content += "' >";
        content += (Supported_Button[suported_button]);
      }
      content += "</select></i>";
    }

    //monostable triger
    content += "<i><label>";
    content += "Monostable trigger";
    content += "</label><select name='trigger_set'>";
    int select_trigger = ConfigManager->get(KEY_MONOSTABLE_TRIGGER)->getValueInt();
    for (int suported_trigger = 0; suported_trigger < sizeof(Supported_MonostableTrigger) / sizeof(char*); suported_trigger++) {
      content += "<option value='";
      content += suported_trigger;
      if (select_trigger == suported_trigger) {
        content += "' selected>";
      }
      else content += "' >";
      content += (Supported_MonostableTrigger[suported_trigger]);
    }
    content += "</select></i>";
  }
  content += "</div>";
  //*****************************************************************************************

  content += "<button type='submit'>Zapisz</button></form>";
  content += "<br>";
  content += "<a href='/firmware_up'><button>Aktualizacja</button></a>";
  content += "<br><br>";
  if (!Supla::GUI::sensorDS.empty()) {
    content += "<a href='/search'><button>Szukaj DS</button></a>";
    content += "<br><br>";
  }
  content += "<form method='post' action='rbt'>";
  content += "<button type='submit'>Restart</button></form></div>";
  content += "<a target='_blank' rel='noopener noreferrer' href='https://forum.supla.org/viewtopic.php?f=11&t=5276'><span style='color: #ffffff !important;'>https://forum.supla.org/</span></a>";
  content += "<br><br>";
  return content;
}

String SuplaWebServer::supla_webpage_search(int save) {
  String content = "";

  content = "<!DOCTYPE HTML>";
  content += "<meta http-equiv='content-type' content='text/html; charset=UTF-8'>";
  content += "<meta name='viewport' content='width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no'>";
  content += "<style>body{font-size:14px;font-family:HelveticaNeue,'Helvetica Neue',HelveticaNeueRoman,HelveticaNeue-Roman,'Helvetica Neue Roman',TeXGyreHerosRegular,Helvetica,Tahoma,Geneva,Arial,sans-serif;font-weight:400;font-stretch:normal;background:" + String(gui_color) + ";color:#fff;line-height:20px;padding:0}.s{width:460px;margin:0 auto;margin-top:calc(50vh - 340px);border:solid 3px #fff;padding:0 10px 10px;border-radius:3px}#l{display:block;max-width:150px;height:155px;margin:-80px auto 20px;background:" + String(gui_color) + ";padding-right:5px}#l path{fill:#000}.w{margin:3px 0 16px;padding:5px 0;border-radius:3px;background:#fff;" + String(gui_box_shadow) + "}h1,h3{margin:10px 8px;font-family:HelveticaNeueLight,HelveticaNeue-Light,'Helvetica Neue Light',HelveticaNeue,'Helvetica Neue',TeXGyreHerosRegular,Helvetica,Tahoma,Geneva,Arial,sans-serif;font-weight:300;font-stretch:normal;color:#000;font-size:23px}h1{margin-bottom:14px;color:#fff}span{display:block;margin:10px 7px 14px}i{display:block;font-style:normal;position:relative;border-bottom:solid 1px " + String(gui_color) + ";height:42px}i:last-child{border:none}label{position:absolute;display:inline-block;top:0;left:8px;color:" + String(gui_color) + ";line-height:41px;pointer-events:none}input,select{width:calc(100% - 145px);border:none;font-size:16px;line-height:40px;border-radius:0;letter-spacing:-.5px;background:#fff;color:#000;padding-left:144px;-webkit-appearance:none;-moz-appearance:none;appearance:none;outline:0!important;height:40px}select{padding:0;float:right;margin:1px 3px 1px 2px}button{width:100%;border:0;background:#000;padding:5px 10px;font-size:16px;line-height:40px;color:#fff;border-radius:3px;" + String(gui_box_shadow) + ";cursor:pointer}.c{background:#ffe836;position:fixed;width:100%;line-height:80px;color:#000;top:0;left:0;" + String(gui_box_shadow) + ";text-align:center;font-size:26px;z-index:100}@media all and (max-height:920px){.s{margin-top:80px}}@media all and (max-width:900px){.s{width:calc(100% - 20px);margin-top:40px;border:none;padding:0 8px;border-radius:0}#l{max-width:80px;height:auto;margin:10px auto 20px}h1,h3{font-size:19px}i{border:none;height:auto}label{display:block;margin:4px 0 12px;color:" + String(gui_color) + ";font-size:13px;position:relative;line-height:18px}input,select{width:calc(100% - 10px);font-size:16px;line-height:28px;padding:0 5px;border-bottom:solid 1px " + String(gui_color) + "}select{width:100%;float:none;margin:0}}</style>";
  if (save == 1) {
    content += "<div id=\"msg\" class=\"c\">Dane zapisane - restart modułu</div>";
  } else if (save == 2) {
    content += "<div id=\"msg\" class=\"c\">Błąd zapisu</div>";
  }
  content += "<div class='s'>";
  content += getLogoSupla();
  content += "<br>";
  content += "<center>";
  content += "<div class='w'>";
  content += "<h3>Temperatura</h3>";
  for (int i = 0; i < Supla::GUI::sensorDS.size(); i++) {
    double temp = Supla::GUI::sensorDS[i]->getValue();
    String ds_key = KEY_DS;
    ds_key += i;

    content += "<i><input name='ds18b20_";
    content += i;
    content += "' value='" + String(ConfigManager->get(ds_key.c_str())->getValue()) + "' maxlength=";
    content += MAX_DS18B20_ADDRESS_HEX;
    content += " readonly><label>";
    if (temp != -275)content += temp;
    else content += "--.--";
    content += " <b>&deg;C</b> ";
    content += "</label>";
    content += "<label style='left:80px'>GPIO: ";
    content += 2;
    content += "</label></i>";
  }
  content += "</div>";


  uint8_t address[8];
  uint8_t count = 0;
  int pin = Supla::GUI::sensorDS[0]->getPin();
  OneWire ow(pin);
  String strAddress = "";

  content += "<form method='post' action='setSearch'>";
  content += "<div class='w'>";
  content += "<h3>Znalezione DS18b20</h3>";

  if (ow.search(address))
  {
    do {
      content += "<i><input name='ds18b20_channel_id_";
      content += count;

      for (uint8_t i = 0; i < 8; i++) {
        if (address[i] < 16) strAddress += String(0, HEX);
        strAddress += String(address[i], HEX);
      }

      content += "' value='" + strAddress + "' maxlength=";
      content += MAX_DS18B20_ADDRESS_HEX;
      content += " readonly><label></i>";

      strAddress = "";
      count++;
    } while (ow.search(address));
  }

  if (count == 0)
    content += "<i><label>brak podłączonych czujników</label></i>";

  content += "</div>";
  content += "</center>";
  content += "<button type='submit'>Zapisz znalezione DSy</button></form>";
  content += "<br><br>";
  content += "<a href='/'><button>Powrót</button></a></div>";
  content += "<a target='_blank' rel='noopener noreferrer' href='https://forum.supla.org/viewtopic.php?f=11&t=5276'><span style='color: #ffffff !important;'>https://forum.supla.org/</span></a>";
  content += "<br><br>";

  return content;
}

String SuplaWebServer::supla_webpage_upddate() {
  String content = "";

  content = "<!DOCTYPE HTML>";
  content += "<meta http-equiv='content-type' content='text/html; charset=UTF-8'>";
  content += "<meta name='viewport' content='width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no'>";
  content += "<style>body{font-size:14px;font-family:HelveticaNeue,'Helvetica Neue',HelveticaNeueRoman,HelveticaNeue-Roman,'Helvetica Neue Roman',TeXGyreHerosRegular,Helvetica,Tahoma,Geneva,Arial,sans-serif;font-weight:400;font-stretch:normal;background:" + String(gui_color) + ";color:#fff;line-height:20px;padding:0}.s{width:460px;margin:0 auto;margin-top:calc(50vh - 340px);border:solid 3px #fff;padding:0 10px 10px;border-radius:3px}#l{display:block;max-width:150px;height:155px;margin:-80px auto 20px;background:" + String(gui_color) + ";padding-right:5px}#l path{fill:#000}.w{margin:3px 0 16px;padding:5px 0;border-radius:3px;background:#fff;" + String(gui_box_shadow) + "}h1,h3{margin:10px 8px;font-family:HelveticaNeueLight,HelveticaNeue-Light,'Helvetica Neue Light',HelveticaNeue,'Helvetica Neue',TeXGyreHerosRegular,Helvetica,Tahoma,Geneva,Arial,sans-serif;font-weight:300;font-stretch:normal;color:#000;font-size:23px}h1{margin-bottom:14px;color:#fff}span{display:block;margin:10px 7px 14px}i{display:block;font-style:normal;position:relative;border-bottom:solid 1px " + String(gui_color) + ";height:42px}i:last-child{border:none}label{position:absolute;display:inline-block;top:0;left:8px;color:" + String(gui_color) + ";line-height:41px;pointer-events:none}input,select{width:calc(100% - 145px);border:none;font-size:16px;line-height:40px;border-radius:0;letter-spacing:-.5px;background:#fff;color:#000;padding-left:144px;-webkit-appearance:none;-moz-appearance:none;appearance:none;outline:0!important;height:40px}select{padding:0;float:right;margin:1px 3px 1px 2px}button{width:100%;border:0;background:#000;padding:5px 10px;font-size:16px;line-height:40px;color:#fff;border-radius:3px;" + String(gui_box_shadow) + ";cursor:pointer}.c{background:#ffe836;position:fixed;width:100%;line-height:80px;color:#000;top:0;left:0;" + String(gui_box_shadow) + ";text-align:center;font-size:26px;z-index:100}@media all and (max-height:920px){.s{margin-top:80px}}@media all and (max-width:900px){.s{width:calc(100% - 20px);margin-top:40px;border:none;padding:0 8px;border-radius:0}#l{max-width:80px;height:auto;margin:10px auto 20px}h1,h3{font-size:19px}i{border:none;height:auto}label{display:block;margin:4px 0 12px;color:" + String(gui_color) + ";font-size:13px;position:relative;line-height:18px}input,select{width:calc(100% - 10px);font-size:16px;line-height:28px;padding:0 5px;border-bottom:solid 1px " + String(gui_color) + "}select{width:100%;float:none;margin:0}}</style>";
  content += "<div class='s'>";
  content += getLogoSupla();
  content += "<div class='w'>";
  content += "<h3>Aktualizacja oprogramowania</h3>";
  content += "<br>";
  content += "<center>";
  content += "<iframe src=";
  content += UPDATE_PATH;
  content += ">Twoja przeglądarka nie akceptuje ramek! width='200' height='100' frameborder='100'></iframe>";
  content += "</center>";
  content += "</div>";
  content += "<a href='/'><button>POWRÓT</button></a></div>";
  content += "<br><br>";

  return content;
}

void SuplaWebServer::supla_webpage_reboot() {
  if (ConfigESP->configModeESP == NORMAL_MODE) {
    if (!httpServer.authenticate(www_username, www_password))
      return httpServer.requestAuthentication();
  }
  httpServer.send(200, "text/html", supla_webpage_start(2));
  rebootESP();
}

String SuplaWebServer::getLogoSupla() {
  return "";//"<center><a href='/'><svg version='1.1' id='l' x='0' y='0' viewBox='0 0 200 200' xml:space='preserve'><path d='M59.3,2.5c18.1,0.6,31.8,8,40.2,23.5c3.1,5.7,4.3,11.9,4.1,18.3c-0.1,3.6-0.7,7.1-1.9,10.6c-0.2,0.7-0.1,1.1,0.6,1.5c12.8,7.7,25.5,15.4,38.3,23c2.9,1.7,5.8,3.4,8.7,5.3c1,0.6,1.6,0.6,2.5-0.1c4.5-3.6,9.8-5.3,15.7-5.4c12.5-0.1,22.9,7.9,25.2,19c1.9,9.2-2.9,19.2-11.8,23.9c-8.4,4.5-16.9,4.5-25.5,0.2c-0.7-0.3-1-0.2-1.5,0.3c-4.8,4.9-9.7,9.8-14.5,14.6c-5.3,5.3-10.6,10.7-15.9,16c-1.8,1.8-3.6,3.7-5.4,5.4c-0.7,0.6-0.6,1,0,1.6c3.6,3.4,5.8,7.5,6.2,12.2c0.7,7.7-2.2,14-8.8,18.5c-12.3,8.6-30.3,3.5-35-10.4c-2.8-8.4,0.6-17.7,8.6-22.8c0.9-0.6,1.1-1,0.8-2c-2-6.2-4.4-12.4-6.6-18.6c-6.3-17.6-12.7-35.1-19-52.7c-0.2-0.7-0.5-1-1.4-0.9c-12.5,0.7-23.6-2.6-33-10.4c-8-6.6-12.9-15-14.2-25c-1.5-11.5,1.7-21.9,9.6-30.7C32.5,8.9,42.2,4.2,53.7,2.7c0.7-0.1,1.5-0.2,2.2-0.2C57,2.4,58.2,2.5,59.3,2.5z M76.5,81c0,0.1,0.1,0.3,0.1,0.6c1.6,6.3,3.2,12.6,4.7,18.9c4.5,17.7,8.9,35.5,13.3,53.2c0.2,0.9,0.6,1.1,1.6,0.9c5.4-1.2,10.7-0.8,15.7,1.6c0.8,0.4,1.2,0.3,1.7-0.4c11.2-12.9,22.5-25.7,33.4-38.7c0.5-0.6,0.4-1,0-1.6c-5.6-7.9-6.1-16.1-1.3-24.5c0.5-0.8,0.3-1.1-0.5-1.6c-9.1-4.7-18.1-9.3-27.2-14c-6.8-3.5-13.5-7-20.3-10.5c-0.7-0.4-1.1-0.3-1.6,0.4c-1.3,1.8-2.7,3.5-4.3,5.1c-4.2,4.2-9.1,7.4-14.7,9.7C76.9,80.3,76.4,80.3,76.5,81z M89,42.6c0.1-2.5-0.4-5.4-1.5-8.1C83,23.1,74.2,16.9,61.7,15.8c-10-0.9-18.6,2.4-25.3,9.7c-8.4,9-9.3,22.4-2.2,32.4c6.8,9.6,19.1,14.2,31.4,11.9C79.2,67.1,89,55.9,89,42.6z M102.1,188.6c0.6,0.1,1.5-0.1,2.4-0.2c9.5-1.4,15.3-10.9,11.6-19.2c-2.6-5.9-9.4-9.6-16.8-8.6c-8.3,1.2-14.1,8.9-12.4,16.6C88.2,183.9,94.4,188.6,102.1,188.6z M167.7,88.5c-1,0-2.1,0.1-3.1,0.3c-9,1.7-14.2,10.6-10.8,18.6c2.9,6.8,11.4,10.3,19,7.8c7.1-2.3,11.1-9.1,9.6-15.9C180.9,93,174.8,88.5,167.7,88.5z'/></svg></a></center>";
}

void SuplaWebServer::rebootESP() {
  delay(1000);
  WiFi.forceSleepBegin();
  wdt_reset();
  ESP.restart();
  while (1)wdt_reset();
}

SuplaWebServer *WebServer;
