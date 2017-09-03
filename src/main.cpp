/*
  Part of TRACER

  Copyright (c) 2017 Raphaël Outhier

  TRACER is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  TRACER is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  aint32_t with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/
#include "Arduino.h"
#include "Core/Core.h"

#include <usb_serial.h>

/*
int main() {
    Serial.begin(38400);
    while(!Serial);
    bool b = true;

    pinMode(13, OUTPUT);
    while(true)
    {
        digitalWrite(13, b);
        Serial.println("BITE");
        b = !b;
        delay(1000);
    }

}
 */

int main() {

    /*
     * Serial.begin(115200);

    while(true) {
        if (Serial.available()) {
            Serial.println(Serial.read_output());
        }
    }
     */
    Core::begin();

    while(true)
    {
        Core::run();
    }

}

/*
#include "Core/Core.h"

void setup() {
    Core::begin();
}

void loop() {
    Core::run();

}
 */


