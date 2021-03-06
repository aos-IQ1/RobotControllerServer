
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
  M_TO_LEFT       = 0x4380, // M006  To left
  M_TO_RIGHT      = 0x4B80, // M007  To right
  M_TURN_LEFT     = 0x5380, // M008  Turn left
  M_TURN_RIGHT    = 0x5B80, // M009  Turn right
  M_GET_UP_U      = 0x6380, // M010  起き上がり うつぶせ
  M_GET_UP_A      = 0x6B80, // M011  起き上がり 仰向け
  M_PUNCHL        = 0x7380, // M012  横パンチ左
  M_PUNCHR        = 0x7B80, // M013  横パンチ右
  M_UTUBUSE       = 0x8380, // M014  うつ伏せ
  M_AOMUKE        = 0x8B80, // M015  仰向け
  M_WAVE_HAND     = 0x9380, // M016  手を振る
  M_KUSSHIN       = 0x9B80, // M017  屈伸
  M_UDETATE       = 0xA380, // M018  腕立て伏せ
} motions;

typedef enum {
  J_HEAD                =  1,

  J_SHOULDER_L_PITCH    =  3,
  J_SHOULDER_R_PITCH    =  4,
  J_SHOULDER_L_ROLL     =  5,
  J_SHOULDER_R_ROLL     =  6,
  J_ELBOW_L             =  9,
  J_ELBOW_R             = 10,

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
