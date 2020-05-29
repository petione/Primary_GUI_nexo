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

#ifndef SuplaConfigManager_h
#define SuplaConfigManager_h

#define KEY_SUPLA_GUID          "GUID"
#define KEY_SUPLA_AUTHKEY       "AUTHKEY"
#define KEY_WIFI_SSID           "wifiSSID"
#define KEY_WIFI_PASS           "wifiPass"
#define KEY_LOGIN               "login"
#define KEY_LOGIN_PASS          "loginPass"
#define KEY_HOST_NAME           "hostName"
#define KEY_SUPLA_SERVER        "suplaServer"
#define KEY_SUPLA_EMAIL         "suplaEmail"
#define KEY_DS                  "ds"
#define KEY_DS_NAME             "dsName"
#define KEY_MAX_DS18B20         "ds18b20"

#define  MAX_GUID                 SUPLA_GUID_SIZE
#define  MAX_AUTHKEY              SUPLA_GUID_SIZE
#define  MAX_SSID                 32
#define  MAX_PASSWORD             64
#define  MIN_PASSWORD             8
#define  MAX_MLOGIN               32
#define  MAX_MPASSWORD            64
#define  MAX_HOSTNAME             32
#define  MAX_SUPLA_SERVER         SUPLA_SERVER_NAME_MAXSIZE
#define  MAX_EMAIL                SUPLA_EMAIL_MAXSIZE
#define  MAX_DS18B20_ADDRESS_HEX  16
#define  MAX_DS18B20_ADDRESS      8
#define  MAX_DS18B20_NAME         8

#define E_CONFIG_OK               0
#define E_CONFIG_FS_ACCESS        1
#define E_CONFIG_FILE_NOT_FOUND   2
#define E_CONFIG_FILE_OPEN        3
#define E_CONFIG_PARSE_ERROR      4
#define E_CONFIG_MAX              5

#define CONFIG_MAX_OPTIONS        30

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <SuplaDevice.h>
#include "FS.h"

class ConfigOption {
  public:
    ConfigOption(const char *key, const char *value, int maxLength);
    const char *getKey();
    const char *getValue();
    int getValueInt();
    uint8_t *getValueBin(size_t size);
    const char *getValueHex(size_t size);
    
    int getLength();
    void setValue(const char *value);

  private:
    char *_key;
    char *_value;
    int _maxLength;
};

class SuplaConfigManager {

  public:
    SuplaConfigManager();
    void begin();
    uint8_t addKey(const char *key, int maxLength);
    uint8_t addKey(const char *key, const char *value, int maxLength);
    uint8_t addKeyAndRead(const char *key, const char *value, int maxLength);
    uint8_t deleteKey(const char *key);
    uint8_t load();
    uint8_t loadItem(const char *key);
    uint8_t save();
    void showAllValue();

    ConfigOption *get(const char *key);
    bool set(const char *key, const char *value);

    bool isDeviceConfigured();
    void setGUIDandAUTHKEY();
    
  private:
    int _optionCount;
    ConfigOption *_options[CONFIG_MAX_OPTIONS];
};

extern SuplaConfigManager ConfigManager;
#endif
