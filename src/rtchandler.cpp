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

void RtcHandler::SetTime(uint8_t hour, uint8_t minute, uint8_t second) {
  Wire.beginTransmission(RTCC_ADDR);
  Wire.write(RTCC_SEC_ADDR);

  Wire.write(DecimalToBcd(second)); //set seconds
  Wire.write(DecimalToBcd(minute)); //set minutes
  Wire.write(DecimalToBcd(hour)); //set hour
  Wire.endTransmission();
}

void RtcHandler::SetDate(byte day, byte weekday, byte month, byte year) {
  month = DecimalToBcd(month);
  month &= ~RTCC_CENTURY_MASK; // Century is always > 2000

  Wire.beginTransmission(RTCC_ADDR);
  Wire.write(RTCC_DAY_ADDR);
  Wire.write(DecimalToBcd(day)); // set day
  Wire.write(DecimalToBcd(weekday)); // set weekday
  Wire.write(month); // set month, century
  Wire.write(DecimalToBcd(year)); // set year
  Wire.endTransmission();
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