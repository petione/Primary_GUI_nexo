#include "SuplaConfigManager.h"

#define CONFIG_FILE_PATH "/config.dat"

ConfigOption::ConfigOption(const char *key, const char *value, int maxLength) {
  size_t size = strlen(key) + 1;
  _key = (char *)malloc(sizeof(char) * size);
  memcpy(_key, key, size - 1);
  _key[size - 1] = 0;

  _maxLength = maxLength;
  setValue(value);
}

const char *ConfigOption::getKey() {
  return _key;
}

const char *ConfigOption::getValue() {
  return _value;
}

int ConfigOption::getValueInt() {
  return atoi(_value);
}

uint8_t *ConfigOption::getValueBin(size_t size) {
  uint8_t *buffer = (uint8_t *)malloc(sizeof(uint8_t) * (size));

  for (int i = 0; i < size; i++) {
    sscanf(&_value[i * 2], "%2hhx", &buffer[i]);
  }
  return buffer;
}

const char *ConfigOption::getValueHex(size_t size) {
  char *buffer = (char *)malloc(sizeof(char) * (size * 2));
  int a, b;
  
  buffer[0] = 0;
  b = 0;

  for (a = 0; a < size; a++) {
    snprintf(&buffer[b], 3, "%02X", (unsigned char)_value[a]);  // NOLINT
    b += 2;
  }
  return buffer;
}

int ConfigOption::getLength() {
  return _maxLength;
}

void ConfigOption::setValue(const char *value) {
  size_t size = _maxLength + 1;
  _value = (char *)malloc(sizeof(char) * (size));

  if (value != NULL) {
    memcpy(_value, value, size - 1);
    _value[size - 1] = 0;
  }
}

SuplaConfigManager::SuplaConfigManager() {
  if (SPIFFS.begin()) {
    Serial.println(F("\nSPIFFS mounted"));
  } else {
    Serial.println(F("\nFailed to mount SPIFFS"));
  }
  _optionCount = 0;
};

uint8_t SuplaConfigManager::addKey(const char *key, int maxLength) {
  return addKey(key, NULL, maxLength);
}

uint8_t SuplaConfigManager::addKey(const char *key, const char *value, int maxLength) {
  if (_optionCount == CONFIG_MAX_OPTIONS) {
    return E_CONFIG_MAX;
  }
  _options[_optionCount] = new ConfigOption(key, value, maxLength);
  _optionCount += 1;

  return E_CONFIG_OK;
}

uint8_t SuplaConfigManager::addKeyAndRead(const char *key, const char *value, int maxLength) {
  addKey(key, maxLength);
  if (this->loadItem(key) != E_CONFIG_OK) {
    Serial.println("ustawiono wartość domyślną");
    this->set(key, value);
  }

  return E_CONFIG_OK;
}

uint8_t SuplaConfigManager::deleteKey(const char *key) {
  for (int i = 0; i < _optionCount; i++) {
    if (strcmp(_options[i]->getKey(), key) == 0) {
      delete _options[_optionCount];
      _optionCount -= 1;
    }
  }

  return E_CONFIG_OK;
}


uint8_t SuplaConfigManager::load() {

  if (SPIFFS.begin()) {
    if (SPIFFS.exists(CONFIG_FILE_PATH)) {
      File configFile = SPIFFS.open(CONFIG_FILE_PATH, "r");

      if (configFile) {
        int i = 0;
        int offset = 0;
        int length = 0;

        for (i = 0; i < _optionCount; i++) {
          length += _options[i]->getLength();
        }

        if (length != configFile.size()) {
          return E_CONFIG_PARSE_ERROR;
        }

        uint8_t *content = (uint8_t *)malloc(sizeof(uint8_t) * length);
        configFile.read(content, length);

        for (i = 0; i < _optionCount; i++) {
          _options[i]->setValue((const char *)(content + offset));
          offset += _options[i]->getLength();
        }

        configFile.close();
        free(content);

        return E_CONFIG_OK;
      } else {
        configFile.close();
        return E_CONFIG_FILE_OPEN;
      }
    } else {
      return E_CONFIG_FILE_NOT_FOUND;
    }
  } else {
    return E_CONFIG_FS_ACCESS;
  }
}

uint8_t SuplaConfigManager::loadItem(const char *key) {

  if (SPIFFS.begin()) {
    if (SPIFFS.exists(CONFIG_FILE_PATH)) {
      File configFile = SPIFFS.open(CONFIG_FILE_PATH, "r");

      if (configFile) {
        int i = 0;
        int offset = 0;
        int length = 0;

        for (i = 0; i < _optionCount; i++) {
          length += _options[i]->getLength();
        }

        uint8_t *content = (uint8_t *)malloc(sizeof(uint8_t) * length);
        configFile.read(content, length);

        for (i = 0; i < _optionCount; i++) {
          if (strcmp(_options[i]->getKey(), key) == 0) {
            _options[i]->setValue((const char *)(content + offset));
          }
          offset += _options[i]->getLength();
        }

        configFile.close();
        free(content);

        return E_CONFIG_OK;
      } else {
        configFile.close();
        return E_CONFIG_FILE_OPEN;
      }
    } else {
      return E_CONFIG_FILE_NOT_FOUND;
    }
  } else {
    return E_CONFIG_FS_ACCESS;
  }
}

uint8_t SuplaConfigManager::save() {
  if (SPIFFS.begin()) {
    int i = 0;
    int offset = 0;
    int length = 0;

    for (i = 0; i < _optionCount; i++) {
      length += _options[i]->getLength();
    }

    File configFile = SPIFFS.open(CONFIG_FILE_PATH, "w+");
    if (configFile) {
      uint8_t *content = (uint8_t *)malloc(sizeof(uint8_t) * length);
      for (i = 0; i < _optionCount; i++) {
        memcpy(content + offset, _options[i]->getValue(), _options[i]->getLength());
        offset += _options[i]->getLength();
      }

      configFile.write(content, length);
      configFile.close();

      free(content);
      return E_CONFIG_OK;
    } else {
      return E_CONFIG_FILE_OPEN;
    }
  }

  return E_CONFIG_FS_ACCESS;
}

bool SuplaConfigManager::isDeviceConfigured() {
  return strcmp(this->get(KEY_SUPLA_GUID)->getValue(), "") != 0 ||
         strcmp(this->get(KEY_SUPLA_AUTHKEY)->getValue(), "") != 0 ||
         strcmp(this->get(KEY_WIFI_SSID)->getValue(), "") != 0 ||
         strcmp(this->get(KEY_WIFI_PASS)->getValue(), "") != 0 ||
         strcmp(this->get(KEY_LOGIN)->getValue(), "") != 0;
}

ConfigOption *SuplaConfigManager::get(const char *key) {
  for (int i = 0; i < _optionCount; i++) {
    if (strcmp(_options[i]->getKey(), key) == 0) {
      return _options[i];
    }
  }

  return NULL;
}

bool SuplaConfigManager::set(const char *key, const char *value) {
  for (int i = 0; i < _optionCount; i++) {
    if (strcmp(key, _options[i]->getKey()) == 0) {
      _options[i]->setValue(value);
      return true;
    }
  }
  return false;
}

void SuplaConfigManager::setGUIDandAUTHKEY() {
  char mac[6];
  int a;

  char GUID[SUPLA_GUID_SIZE];
  char AUTHKEY[SUPLA_AUTHKEY_SIZE];

  memset(GUID, 0, SUPLA_GUID_SIZE);
  memset(AUTHKEY, 0, SUPLA_AUTHKEY_SIZE);

  os_get_random((unsigned char*)GUID, SUPLA_GUID_SIZE);
  os_get_random((unsigned char*)AUTHKEY, SUPLA_AUTHKEY_SIZE);

  if ( SUPLA_GUID_SIZE >= 6 ) {
    wifi_get_macaddr(STATION_IF, (unsigned char*)mac);

    for (a = 0; a < 6; a++)
      GUID[a] = (GUID[a] * mac[a]) % 255;
  }

  if ( SUPLA_GUID_SIZE >= 12 ) {
    wifi_get_macaddr(SOFTAP_IF, (unsigned char*)mac);

    for (a = 0; a < 6; a++)
      GUID[a + 6] = ( GUID[a + 6] * mac[a] ) % 255;
  }

  for (a = 0; a < SUPLA_GUID_SIZE; a++) {
    GUID[a] = (GUID[a] + system_get_time() + spi_flash_get_id() + system_get_chip_id() + system_get_rtc_time()) % 255;
  }

  a = SUPLA_GUID_SIZE > SUPLA_AUTHKEY_SIZE ? SUPLA_AUTHKEY_SIZE : SUPLA_GUID_SIZE;
  a--;
  for (; a > 0; a--) {
    AUTHKEY[a] += GUID[a];
  }

  this->set(KEY_SUPLA_GUID, GUID);
  this->set(KEY_SUPLA_AUTHKEY, AUTHKEY);

}
