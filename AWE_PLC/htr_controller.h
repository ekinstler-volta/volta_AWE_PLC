#ifndef HTR_CONTROLLER_H
#define HTR_CONTROLLER_H
#include "Arduino.h"

/* DEFINES */
#define PWMCYCLETIME (10000)  // How long 1 cycle takes i.e. ~100ms
#define PWMCYCLERES (1000)    // Resolution of the cycle i.e. ~1ms
#define MAXPWM (PWMCYCLETIME / PWMCYCLERES)
#define MINPWM (0)
#define LOOPHTR (4) // Output pin to control Loop Heater Relay
#define BLOCKHTR (5) // Output pin to control Loop Heater Relay
#define HTR_KP (6.0f)
#define HTR_KI (0.3f)
#define HTR_KD (15.0f)

#define ANALOGBITS (12)
#define ANALOGRES ((1 << ANALOGBITS) - 1)
#define ANALOGREF (3.3f)
#define VIN (3.3f)
#define R1 (10000)
#define T25 (298.15f)
#define R25 (10000.0f)
#define BETA (3984.0f)

/* GLOBALS */
extern bool htrsEnabled;
extern float htrSetpoint;
extern float htrTemp;


/* FWD DECLARATIONS */
void pwmCycle(void);
void htrsON(void);
void htrsOFF(void);
short htrPID(void);
void setupHtrs(void);
float thermistor_temp(float countsTherm);

#endif /* HTR_CONTROLLER */