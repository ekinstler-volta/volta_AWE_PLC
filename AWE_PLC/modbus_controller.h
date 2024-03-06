#ifndef MODBUS_CONTROLLER_H
#define MODBUS_CONTROLLER_H

#include <Arduino.h>
#include <ArduinoRS485.h>
#include <ArduinoModbus.h>

/* DEFINES */
#define SLAVE_ID (1)
#define BAUD (19200)
#define SERIAL_CONFIG (SERIAL_8N2)


/* GLOBALS */

/* FWD DECLARATIONS */
void setup_modbus(void);
void write_float(float f, int address);

#endif /* MODBUS_CONTROLLER_H */