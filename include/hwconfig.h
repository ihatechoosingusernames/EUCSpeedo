#ifndef HWCONFIG_H_
#define HWCONFIG_H_

namespace euc {

// Touch Pad/Button
#define PIN_223B_VDD 25
#define PIN_223B_Q 33

// Battery
#define PIN_BATT 32

// RTC
#define PIN_PCF8563_SCL 22
#define PIN_PCF8563_SDA 21
#define PIN_PCF8563_INT 34

// Screen
#define PIN_ST7735_SDA 19
#define PIN_ST7735_SCL 18
#define PIN_ST7735_RS 23
#define PIN_ST7735_RES 26
#define PIN_ST7735_CS 5
#define PIN_ST7735_LEDK 27

// LED
#define PIN_LED 4

// Built in base clock
#define BASE_CLOCK_HZ 40000000

}

#endif