#include "htr_controller.h"
#include "modbus_controller.h"

bool htrsEnabled = false;
float htrSetpoint = 0.0f;
float htrTemp = 0.0f;

void setupHtrs(void) {
  /* SETUP GPIO PINS */
  pinMode(LOOPHTR, OUTPUT);
  digitalWrite(LOOPHTR, HIGH);
  pinMode(BLOCKHTR, OUTPUT);
  digitalWrite(BLOCKHTR, HIGH);
  analogReadResolution(ANALOGBITS);
  htrsEnabled = false;
}

void htrsON(void) {
  // CHECK IF HTRS ALLOWED ON
  if (htrsEnabled) {
    // TURN HTRS ON
    digitalWrite(LOOPHTR, LOW);
    digitalWrite(BLOCKHTR, LOW);
    // CHECK IF PUMP ON
    long pumpDemand = ModbusRTUServer.holdingRegisterRead(24999);
    if(pumpDemand < 500) {
      ModbusRTUServer.holdingRegisterWrite(24999, 500);
      analogWrite(A0, 39); // 500 * 77 / 1000
    }
  }
}

void htrsOFF(void) {
  // TURN HTRS OFF
  digitalWrite(LOOPHTR, HIGH);
  digitalWrite(BLOCKHTR, HIGH);
}

short htrPID(void) {
  static float kP = HTR_KP;
  static float kI = HTR_KI * (float)(PWMCYCLETIME / 1000);
  static float kD = HTR_KD / (float)(PWMCYCLETIME / 1000);
  static float err, err_last, err_sig, err_P, err_I, err_D = 0;
  // Read Heater Temp
  htrTemp = thermistor_temp((float)analogRead(A1));

  // Calculate
  err = htrSetpoint - htrTemp;
  err_P = kP * err;
  err_I += kI * err_P;
  err_D = kD * (err_last - err);
  err_last = err;
  err_sig = err_P + err_I + err_D;
  // Limit output and prevent windup
  if (err_sig > MAXPWM) {
    if(kI > 0) {
      // Anti-windup
      err_I -= err_sig - MAXPWM;
    }
    // Limit
    err_sig = MAXPWM;
  }
  else if (err_sig < MINPWM) {
    if(kI > 0) {
      // Anti-windup
      err_I += MINPWM - err_sig;
    }
    //Limit
    err_sig = MINPWM;
  }
  return (short)err_sig;
}

float thermistor_temp(float countsTherm) {
  float vTherm = countsTherm * ANALOGREF / ANALOGRES;
  float rTherm = vTherm * R1 / (VIN - vTherm);
  Serial.println(rTherm);
  float kTherm = pow((log(rTherm/R25)/BETA) + (1/T25),-1);
  Serial.println(kTherm);
  return kTherm - 273.15;
}