#include "modbus_controller.h"


void write_float(float f, int address) {
  union {
    float val;
    uint16_t regs[2];
  } u;
  u.val = f;
  ModbusRTUServer.holdingRegisterWrite(address, u.regs[0]);
  ModbusRTUServer.holdingRegisterWrite(address + 1, u.regs[1]);
}

void setup_modbus(void) {
  /*
    Modbus library uses default RX and TX pins for HW serial.

    DEFAULT PINS
    RX = 0
    TX = 1
  */
  ModbusRTUServer.begin(SLAVE_ID, BAUD, SERIAL_CONFIG);
  // Configure registers
  // Holding registers = R/W (We as the slave write data for the master here)
  // Use a disgusting amount of memory because someone decided to place a register
  // every 100 addresses.
  // Also modbus registers in the HMI are one lower in the slave device to due to
  // count from one registers in the portenta arduino pro PLC.
  
  /*                  Register MAP
  Arduino   |   HMI   |   Type    |   Description
  24999       25000     uint16      Pump RPM
  25099       25100     float       EC/PH
  25199       25200     float       Temperature "Pump"
  25299       25300     uint16      Pump Setpoint
  25399       25400     float       Temperature Electrolyzer
  25499       25500     uint16      Heaters Enabled
  25599       25600     uint16      Level Sensor
                      Register MAP                        */
  ModbusRTUServer.configureHoldingRegisters(24999, 601);
}