#include "rtchandler.h"

#include <Wire.h>

#include "hwconfig.h"

namespace euc {

RtcHandler::RtcHandler() {
  Wire.setPins(PIN_PCF8563_SDA, PIN_PCF8563_SCL);
  Wire.begin();
}

void RtcHandler::UpdateTime() {
  Wire.beginTransmission(RTCC_ADDR);
  Wire.write(RTCC_STAT1_ADDR);
  Wire.endTransmission();
  
  Wire.requestFrom(RTCC_ADDR, 5); //request 5 bytes
  Wire.read();
  Wire.read();

  // Reading times, masking unnecessary bits
  second = BcdToDecimal(Wire.read() & 0x7f);
  minute = BcdToDecimal(Wire.read() & 0x7f);
  hour = BcdToDecimal(Wire.read() & 0x3f);
}

void RtcHandler::UpdateDate() {
  Wire.beginTransmission(RTCC_ADDR);
  Wire.write(RTCC_DAY_ADDR);
  Wire.endTransmission();
  
  Wire.requestFrom(RTCC_ADDR, 4); //request 4 bytes
  day = BcdToDecimal(Wire.read() & 0x3f);
  Wire.read();  // Skipping weekday for now
  month = BcdToDecimal(Wire.read() & 0x1f);
  year = BcdToDecimal(Wire.read());  
}

uint8_t RtcHandler::getSecond() { return second; }
uint8_t RtcHandler::getMinute() { return minute; }
uint8_t RtcHandler::getHour() { return hour; }

uint8_t RtcHandler::getDay() { return day; }
uint8_t RtcHandler::getMonth() { return month; }
uint8_t RtcHandler::getYear() { return year; }

uint8_t RtcHandler::BcdToDecimal(uint8_t bcd) {
  return (bcd / 10 * 16) + (bcd % 10);
}

uint8_t RtcHandler::DecimalToBcd(uint8_t decimal) {
  return (decimal / 16 * 10) + (decimal % 16);
}

}