
#ifndef __MOTION_H__
#define __MOTION_H__

typedef enum { 
  M_OK, 
  M_SUSPEND_FAILED, 
  M_COUNTER_EEPROM_FAILED, 
  M_CALL_FAILED,
  M_RESTART_FAILED,
} motion_result;
typedef enum { M_OJIGI, } motions;

motion_result send_motion(motions motion);

#endif