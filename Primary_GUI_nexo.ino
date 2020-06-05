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

#include <SPI.h>
#include <SuplaDevice.h>
#include <supla/sensor/esp_free_heap.h>

#include "SuplaDevicePrimary.h"
#include "SuplaWebServer.h"

void setup() {
  Serial.begin(74880);

  SuplaDevicePrimary.addRelayButton(5, 0);
  SuplaDevicePrimary.addRelayButton(5, 14);
  SuplaDevicePrimary.addDS18B20MultiThermometer(16);
  SuplaDevicePrimary.addConfigESP(0, 2, CONFIG_MODE_5SEK_HOLD); // CONFIG_MODE_10_ON_PRESSES, CONFIG_MODE_5SEK_HOLD

  new Supla::Sensor::EspFreeHeap();

  SuplaDevicePrimary.begin();
}

void loop() {
  SuplaDevice.iterate();
}
