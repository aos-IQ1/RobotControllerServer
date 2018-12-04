
#ifndef __MOTION_H__
#define __MOTION_H__
#include <stdint.h>

typedef enum { 
  C_OK, 
  C_SUSPEND_FAILED, 
  C_COUNTER_EEPROM_FAILED, 
  C_CALL_FAILED,
  C_RESTART_FAILED,
} cmd_result;

typedef enum { 
  M_OJIGI, 
  M_HOME_POSITION,
  M_PRE_WALK,
  M_WALKL,
  M_WALKR,
  M_POST_WALKR,
  M_POST_WALKL,
} motions;

cmd_result send_motion(motions motion);
uint8_t checksum(uint8_t *cmd);
cmd_result send_command (uint8_t *cmd, cmd_result ret_type, uint16_t timeout);

#endif