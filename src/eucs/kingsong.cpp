#include "eucs/kingsong.h"

namespace euc {

void Kingsong::ProcessInput(uint8_t data[], size_t size) {
  // if (rideStartTime == 0) {
  //         rideStartTime = Calendar.getInstance().getTimeInMillis();
  //   mRidingTime = 0;
  // }
  //     if (data.length >= 20) {
  //         int a1 = data[0] & 255;
  //         int a2 = data[1] & 255;
  //         if (a1 != 170 || a2 != 85) {
  //             return false;
  //         }
  //         if ((data[16] & 255) == 169) { // Live data
  //             mVoltage = byteArrayInt2(data[2], data[3]);
  //             mSpeed = byteArrayInt2(data[4], data[5]);
  //             mTotalDistance = byteArrayInt4(data[6], data[7], data[8], data[9]);
  //             if ((mModel.compareTo("KS-18L") == 0) && !m18Lkm) {
  //                 mTotalDistance = Math.round(mTotalDistance*KS18L_SCALER);
  //             }
  //             mCurrent = ((data[10]&0xFF) + (data[11]<<8));

  //     mTemperature = byteArrayInt2(data[12], data[13]);
  //             setVoltageSag(mVoltage);
  //             if ((data[15] & 255) == 224) {
  //                 mMode = data[14];
  //       mModeStr = String.format(Locale.US, "%d", mMode);
  //             }

  //             int battery;


  //             if ((mModel.compareTo("KS-18L") == 0) || (mModel.compareTo("KS-16X") == 0) ||(mBtName.compareTo("RW") == 0) || (mModel.compareTo("KS-18LH") == 0) || (mModel.compareTo("KS-S18") == 0) || (mName.startsWith("ROCKW"))) {
  //                 if (mBetterPercents) {
  //                     if (mVoltage > 8350) {
  //                         battery = 100;
  //                     } else if (mVoltage > 6800) {
  //                         battery = (mVoltage - 6650) / 17;
  //                     } else if (mVoltage > 6400) {
  //                         battery = (mVoltage - 6400) / 45;
  //                     } else {
  //                         battery = 0;
  //                     }
  //                 } else {
  //                     if (mVoltage < 6250) {
  //                         battery = 0;
  //                     } else if (mVoltage >= 8250) {
  //                         battery = 100;
  //                     } else {
  //                         battery = (mVoltage - 6250) / 20;
  //                     }
  //                 }
  //             } else {
  //                 if (mBetterPercents) {
  //                     if (mVoltage > 6680) {
  //                         battery = 100;
  //                     } else if (mVoltage > 5440) {
  //                         battery = (int) Math.round((mVoltage - 5320) / 13.6);
  //                     } else if (mVoltage > 5120) {
  //                         battery = (mVoltage - 5120) / 36;
  //                     } else {
  //                         battery = 0;
  //                     }
  //                 } else {
  //                     if (mVoltage < 5000) {
  //                         battery = 0;
  //                     } else if (mVoltage >= 6600) {
  //                         battery = 100;
  //                     } else {
  //                         battery = (mVoltage - 5000) / 16;
  //                     }
  //                 }

  //             }

  //             setBatteryPercent(battery);

  //             return true;
  //         } else if ((data[16] & 255) == 185) { // Distance/Time/Fan Data
  //             long distance = byteArrayInt4(data[2], data[3], data[4], data[5]);
  //             setDistance(distance);
  //             //int currentTime = byteArrayInt2(data[6], data[7]);
  //           int currentTime = (int) (Calendar.getInstance().getTimeInMillis() - rideStartTime) / 1000;
  //             setCurrentTime(currentTime);
  //             setTopSpeed(byteArrayInt2(data[8], data[9]));
  //             mFanStatus = data[12];
  //             return true;
  //         } else if ((data[16] & 255) == 187) { // Name and Type data
  //             int end = 0;
  //             int i = 0;
  //             while (i < 14 && data[i + 2] != 0) {
  //                 end++;
  //                 i++;
  //             }
  //             mName = new String(data, 2, end).trim();
  //             mModel = "";
  //             String[] ss = mName.split("-");
  //             for (i = 0; i < ss.length - 1; i++) {
  //                 if (i != 0) {
  //                     mModel += "-";
  //                 }
  //                 mModel += ss[i];
  //             }
  //             try {
  //                 mVersion = String.format(Locale.US, "%.2f", ((double)(Integer.parseInt(ss[ss.length - 1])/100.0)));
  //             } catch (Exception ignored) {
  //             }
  //             return true;
  //         } else if ((data[16] & 255) == 179) { // Serial Number
  //             byte[] sndata = new byte[18];
  //             System.arraycopy(data, 2, sndata, 0, 14);
  //             System.arraycopy(data, 17, sndata, 14, 3);
  //             sndata[17] = (byte) 0;
  //             mSerialNumber = new String(sndata);
  //             updateKSAlarmAndSpeed();
  //             return true;
  //         }
  //         else if ((data[16] & 255) == 164 || (data[16] & 255) == 181) { //0xa4 || 0xb5 max speed and alerts
  //             mWheelMaxSpeed = (data[10] & 255);
  //             mKSAlarm3Speed = (data[8] & 255);
  //             mKSAlarm2Speed = (data[6] & 255);
  //             mKSAlarm1Speed = (data[4] & 255);
  //             mKSAlertsAndSpeedupdated = true;
  //             // after received 0xa4 send same repeat data[2] =0x01 data[16] = 0x98
  //             if((data[16] & 255) == 164)
  //             {
  //                 data[16] = (byte)0x98;
  //                 mBluetoothLeService.writeBluetoothGattCharacteristic(data);
  //             }
  //             return true;
  //         }
  //     }
  //     return false;
}

}