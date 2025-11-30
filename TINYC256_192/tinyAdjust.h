#ifndef _TINY_C_ADJUST_H_
#define _TINY_C_ADJUST_H_

#include "libirtemp.h"
#include "stdint.h"

#define SUCCESS 0
#define FAIL -1
#define NUCT_LEN 8192
#define TAU_TABLE_SIZE (4 * 14 * 64)

extern uint16_t correct_table[TAU_TABLE_SIZE];
extern uint16_t center_temp;
extern float org_kelvin_temp;
extern float new_kelvin_temp;

extern uint8_t gain_flag;
extern uint16_t nuc_table[NUCT_LEN];

extern EnvParam_t org_env_param;
extern EnvParam_t new_env_param;

extern EnvFactor_t org_env_factor;
extern EnvFactor_t new_env_factor;

// read the environmental parameters from device
char read_nuc_parameter(void);

// calculate the origin environmental variable correction parameters
char calculate_org_env_cali_parameter(void);

// calculate the new environmental variable correction parameters
char calculate_new_env_cali_parameter(uint16_t *correct_table, float ems, float ta, float tu, float dist, float hum);

// recalibrate the temperature with new environment parameters
char temp_calc_with_new_env_calibration(float org_temp, float *new_temp);

#endif /*_TINY_C_ADJUST_H_*/