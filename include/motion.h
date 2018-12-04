
#ifndef __MOTION_H__
#define __MOTION_H__
#include <stdint.h>

typedef enum { 
  C_OK, 
  C_SUSPEND_FAILED, 
  C_COUNTER_EEPROM_FAILED, 
  C_CALL_FAILED,
  C_RESTART_FAILED,
  C_MULTI_SERVO_FAILED,
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

typedef enum {
  J_HEAD                =  1,

  J_SHOULDER_L_PITCH    =  3,
  J_SHOULDER_R_PITCH    =  4,
  J_SHOULDER_L_ROLL     =  5,
  J_SHOULDER_R_ROLL     =  6,
  J_ELBOW_L             =  9,
  J_ELBOW_L             = 10,

  J_THIGH_L_ROLL        = 13,  
  J_THIGH_R_ROLL        = 14,  
  J_THIGH_L_PITCH       = 15,  
  J_THIGH_R_PITCH       = 16,  
  J_KNEE_L              = 17,
  J_KNEE_R              = 18,
  J_ANKLE_L_PITCH       = 19,
  J_ANKLE_R_PITCH       = 20,
  J_ANKLE_L_ROLL        = 21,
  J_ANKLE_R_ROLL        = 22,
} joints;
/*
              1 頭
  3:左肩ピッチ         4:右肩ピッチ
  5:左肩ロール         6:右肩ロール
  9:左肘             10:右 肘

  13:左腿ロール        14:右腿ロール
  15:左腿ピッチ        16:右腿ピッチ
  17:左膝ピッチ        18:右膝ピッチ
19:左足首ピッチ        20:右足首ピッチ
21:左足首ロール        22:右足首ロール
*/

cmd_result send_motion(motions motion);
uint8_t checksum(uint8_t *cmd);
cmd_result send_command (uint8_t *cmd, cmd_result ret_type, uint16_t timeout);
cmd_result walk(bool (*finish)(void));
cmd_result drive_joint(joints joint, uint8_t speed, uint16_t position);

#endif