/**
  ******************************************************************************
  ******************************************************************************
  */
/* -------------------------------------*/

#ifndef __Dshot_H
#define __Dshot_H

#include "main.h"



void Dshot_pwm_set(void);


uint16_t add_checksum_and_telemetry_one(uint16_t packet, uint8_t telem);
void pwmWriteDigital_one(uint32_t *esc_cmd, uint16_t value);


















#endif /* __Dshot_H */







