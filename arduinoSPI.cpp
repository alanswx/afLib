/**
 * Copyright 2015 Afero, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "arduinoSPI.h"
#include "Arduino.h"
#include <SPI.h>


arduinoSPI::arduinoSPI(int chipSelect)
{
    _chipSelect = chipSelect;
    _spiSettings = SPISettings(1000000, LSBFIRST, SPI_MODE0);
}

void arduinoSPI::begin()
{
    pinMode(_chipSelect, OUTPUT);
    digitalWrite(_chipSelect, HIGH);
    SPI.begin();
}
void arduinoSPI::beginSPI() /* settings are in this class */
{
    SPI.beginTransaction(_spiSettings);
    digitalWrite(_chipSelect, LOW);
    delayMicroseconds(8);
}

void arduinoSPI::endSPI()
{
    //delayMicroseconds(1);
    digitalWrite(_chipSelect, HIGH);
    SPI.endTransaction();
}
void arduinoSPI::transfer(char *bytes,int len)
{
  SPI.transfer(bytes,len);
}
