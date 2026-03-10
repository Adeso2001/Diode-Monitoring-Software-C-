/**
*   file nist.h
*   author Measurement Computing Corp.
*   brief This file contains NIST thermocouple linearization functions.
*
*   date 1 Feb 2018
*/
#ifndef _NIST_H
#define _NIST_H

#ifdef __cplusplus
extern "C" {
#endif

double NISTCalcVoltage(unsigned int type, double temperature);
double NISTCalcTemperature(unsigned int type, double voltage);

#ifdef __cplusplus
}
#endif

#endif
