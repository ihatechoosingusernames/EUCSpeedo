#include "eucs/inmotion.h"

namespace euc {

void Inmotion::ProcessInput(uint8_t data[], size_t data_len) {
  // ArrayList<InMotionAdapter.Status> statuses = InMotionAdapter.getInstance().charUpdated(data);
  // if (statuses.size() < 1) return false;
  //     if (rideStartTime == 0) {
  //         rideStartTime = Calendar.getInstance().getTimeInMillis();
  //   mRidingTime = 0;
  // }		
  //     for (InMotionAdapter.Status status: statuses) {
  //         Timber.i(status.toString());
  //         if (status instanceof InMotionAdapter.Infos) {
  //     mWheelLightEnabled = ((InMotionAdapter.Infos) status).getLightState();
  //     mWheelLedEnabled = ((InMotionAdapter.Infos) status).getLedState();
  //     mWheelButtonDisabled = ((InMotionAdapter.Infos) status).getHandleButtonState();
  //     mWheelMaxSpeed = ((InMotionAdapter.Infos) status).getMaxSpeedState();
  //     mWheelSpeakerVolume = ((InMotionAdapter.Infos) status).getSpeakerVolumeState();
  //     mWheelTiltHorizon = ((InMotionAdapter.Infos) status).getTiltHorizon(); 
  //             mSerialNumber = ((InMotionAdapter.Infos) status).getSerialNumber();
  //             mModel = ((InMotionAdapter.Infos) status).getModelString();
  //             mVersion = ((InMotionAdapter.Infos) status).getVersion();
  //     mNewWheelSettings = true;
  //         } else if (status instanceof InMotionAdapter.Alert){
  //     if (mAlert == "") {
  //       mAlert = ((InMotionAdapter.Alert) status).getfullText();
  //     } else {
  //       mAlert = mAlert + " | " + ((InMotionAdapter.Alert) status).getfullText();
  //     }
  //   } else {
  //             mSpeed = (int) (status.getSpeed() * 360d);
  //             mVoltage = (int) (status.getVoltage() * 100d);
  //             mCurrent = (int) (status.getCurrent() * 100d);
  //     mTemperature = (int) (status.getTemperature() * 100d);
  //     mTemperature2 = (int) (status.getTemperature2() * 100d);
  //     mTotalDistance = (long) (status.getDistance()*1000d);
  //     mAngle = (double) (status.getAngle()); 
  //     mRoll = (double) (status.getRoll()); 
      
  //     mModeStr = status.getWorkModeString();
  //             setBatteryPercent((int) status.getBatt());
  //             setDistance((long) status.getDistance());
      
  //             int currentTime = (int) (Calendar.getInstance().getTimeInMillis() - rideStartTime) / 1000;
  //             setCurrentTime(currentTime);
  //             setTopSpeed(mSpeed);
  //             setVoltageSag(mVoltage);
  //         }
  //     }
  //     return true;
}

}