#ifndef RTCHANDLER_H_
#define RTCHANDLER_H_

#include <Arduino.h>

// Code for RtcHandler adapted for ESP32 from https://github.com/orbitalair/Rtc_Pcf8563

namespace euc {

#define RTCC_ADDR 0x51

#define RTCC_SEC 1
#define RTCC_MIN 2
#define RTCC_HR 3
#define RTCC_DAY 4
#define RTCC_WEEKDAY 5
#define RTCC_MONTH 6
#define RTCC_YEAR 7
#define RTCC_CENTURY 8

// Register addresses
#define RTCC_STAT1_ADDR 0x0
#define RTCC_STAT2_ADDR 0x01
#define RTCC_SEC_ADDR 0x02
#define RTCC_MIN_ADDR 0x03
#define RTCC_HR_ADDR 0x04
#define RTCC_DAY_ADDR 0x05
#define RTCC_WEEKDAY_ADDR 0x06
#define RTCC_MONTH_ADDR 0x07
#define RTCC_YEAR_ADDR 0x08
#define RTCC_ALRM_MIN_ADDR 0x09
#define RTCC_SQW_ADDR 0x0D
#define RTCC_CENTURY_MASK 0x80

class RtcHandler {
  public:
    RtcHandler();

    void UpdateTime();
    void UpdateDate();

    void setTime(uint8_t hour, uint8_t minute, uint8_t second);
    void setDate(uint8_t day, uint8_t weekday, uint8_t month, uint8_t year);

    uint8_t getSecond();
    uint8_t getMinute();
    uint8_t getHour();

    uint8_t getDay();
    uint8_t getMonth();
    uint8_t getYear();

  private:
    uint8_t BcdToDecimal(uint8_t bcd);
    uint8_t DecimalToBcd(uint8_t decimal);

    uint8_t second, minute, hour, day, month, year;
};

}

#endif