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

#include <SPI.h>
#include <SuplaDevice.h>
#include <supla/sensor/esp_free_heap.h>

#include "SuplaDeviceGUI.h"

void setup() {
  Serial.begin(74880);

  Supla::GUI::addRelayButton(14, 0);
  Supla::GUI::addRelayButton(12, 0, false);
  Supla::GUI::addDS18B20MultiThermometer(13);
  Supla::GUI::addConfigESP(0, 2, CONFIG_MODE_5SEK_HOLD); // pinNumberConfig, pinLedConfig, CONFIG_MODE_10_ON_PRESSES/CONFIG_MODE_5SEK_HOLD

  new Supla::Sensor::EspFreeHeap();

  Supla::GUI::begin();
}

void loop() {
  SuplaDevice.iterate();
}
