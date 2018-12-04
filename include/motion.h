
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
    M_OJIGI         = 0x0B80, // M001  お辞儀
    M_HOME_POSITION = 0x1380, // M002  ホームhポジション
    M_PRE_WALK      = 0x1B80, // M003  PreWalk
    M_WALKL         = 0x2380, // M004  WalkL
    M_WALKR         = 0x2B80, // M005  WalkR
    M_POST_WALKR    = 0x3380, // M005  PostWalkR
    M_POST_WALKL    = 0x3B80, // M005  PostWalkL
} motions;

cmd_result send_motion(motions motion);
uint8_t checksum(uint8_t *cmd);
cmd_result send_command (uint8_t *cmd, cmd_result ret_type, uint16_t timeout);
cmd_result walk(bool (*finish)(void));

#endif