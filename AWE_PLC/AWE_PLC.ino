/* INCLUDES */
#include "htr_controller.h"
#include "modbus_controller.h"

#define MODBUSCYCLERES (1000)

unsigned long timeNow = 0;
unsigned long timeLastPWM = 0;
unsigned long timeLastModbusRegCheck = 0;

void setup() {
  Serial.begin(9600);
  setupHtrs();
  setup_modbus();
  // set pwmCycle initial conditions, do this last!!
  timeNow = millis();
  timeLastPWM = timeNow;
}

void loop() {
  // Look for modbus requests
  ModbusRTUServer.poll();
  // Check to see if PWM loop should cycle
  timeNow = millis();
  if(timeNow - timeLastPWM > PWMCYCLERES) {
    timeLastPWM = timeNow;
    pwmCycle();
  }
  // Check to see if we should update Modbus values
  timeNow = millis();
    if(timeNow - timeLastModbusRegCheck > MODBUSCYCLERES) {
    timeLastModbusRegCheck = timeNow;
    modbusCycle();
  }
}

void pwmCycle(void) {
  static unsigned short steps = PWMCYCLETIME / PWMCYCLERES;
  static unsigned short setpointHtr = 0;
  if (steps == 1) {
    // RESET
    steps = PWMCYCLETIME / PWMCYCLERES;
    // CALCULATE SETPOINT(S)
    setpointHtr = htrPID();
    if(steps <= setpointHtr) {
      htrsON();
    } else {
      htrsOFF();
    }
    /* CALCULATE OTHER SETPOINTS HERE */
  } else {
    // INCREMENT
    steps -= 1;
    // CHECK(S)
    if (steps <= setpointHtr) {
      // ON CONDITION
      htrsON();
    }
    /*
    if (steps <= setpointOTHER) {
      // ON CONDITION
      ADD CODE HERE
    }
    */
  }
}

void modbusCycle(void) {
  /*                  Register MAP
  Arduino   |   HMI   |   Type    |   Description
  24999       25000     uint16      Pump RPM
  25099       25100     float       EC/PH
  25199       25200     float       Temperature "Pump"
  25299       25300     uint16      Temperature Setpoint
  25399       25400     float       Temperature Electrolyzer
  25499       25500     uint16      Heaters Enabled
  25599       25600     uint16      Level Sensor
                      Register MAP                        */
  /* Inputs from HMI */
  long modBusReg = 0;
  modBusReg = ModbusRTUServer.holdingRegisterRead(24999);
  // Input is a value from 0-5000 representing 0-5v linearly scaled
  // Output needs to be 0-255 representing 0-3.3v linearly scaled
  // Inputs above 3.3v should be treated as 3.3v
  // Input / 1000 = 0-5v
  // Output = (0-5v) * (255/3.3v) = (0-5v) * 77
  // !!!! MULTIPLY BEFORE DIVIDING TO AVOID LOSING PERCISION !!!!
  int output = modBusReg * 77 / 1000;
  if(output > 255) output = 255;
  analogWrite(A0, output);

  htrSetpoint = (float) ModbusRTUServer.holdingRegisterRead(25299);
  htrsEnabled = (bool) ModbusRTUServer.holdingRegisterRead(25499);

  /* Outputs to HMI */
  /* TODO */ //write_float(PH, 25099)
  write_float(htrTemp, 25199);
  write_float(htrTemp, 25399);
  /* TODO */ ModbusRTUServer.holdingRegisterWrite(25599, 1); //Write actual Level Sensor Value
}
