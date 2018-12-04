#include <stdint.h>
#include "motion.h"
#include <M5Stack.h>
#include <Arduino.h>

bool wait_command_ack();


// command templates
uint8_t stop_motion_command[]           = {0x09,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x0b};
uint8_t reset_program_counter_command[] = {0x11,0x00,0x02,0x02,0x00,0x00,0x4B,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x64};
uint8_t address_command[]               = {0x07,0x0C,0x80,0x0B,0x00,0x00,0x9E};
uint8_t motion_restart_command[]        = {0x09,0x00,0x02,0x00,0x00,0x00,0x03,0x00,0x0E};

/*
  ロボットに-oモーションを送信
*/
cmd_result send_motion(motions motion) {

  uint16_t motion_address = (uint16_t) motion;
  address_command[2] = motion_address & 0xFF;
  address_command[3] = (motion_address >> 8) & 0xFF;

  Serial2.write(stop_motion_command,stop_motion_command[0]);
  if(!wait_command_ack()){
    delay(5000);
    return C_SUSPEND_FAILED;
  }

  Serial2.write(reset_program_counter_command,reset_program_counter_command[0]);
  if(!wait_command_ack()){
    delay(5000);
    return C_COUNTER_EEPROM_FAILED;
  }

  send_command(address_command, C_CALL_FAILED, 5000);

  Serial2.write(motion_restart_command,motion_restart_command[0]);
  if(!wait_command_ack()){
    delay(5000);
    return C_RESTART_FAILED;
  }

  //ロボットからのモーション完了を待つ
  log_d("wait for motion end");
  while(Serial2.available() == 0){}
  uint8_t command_length = Serial2.read(); Serial.printf("%x ", command_length);
  uint8_t command[command_length];
  command[0] = command_length;
  for(uint8_t i=1;i<command_length;i++){
    command[i] = Serial2.read();
    Serial.printf("%x ", command[i]);
  }
  Serial.printf("\n");

  if(!(command[2] == 'P' && command[3] == 'O' && command[4] == 'I')){
    // モーション完了でPOIが帰ってきてない
  }

  return C_OK;

}

/*
  ロボットからのコマンドの返り値をwaitする
  @return 返り値が成功ならばtrue,失敗ならばfalse
*/
bool wait_command_ack(){
  log_d("Serial2 avail:%d", Serial2.available());
  while(Serial2.available() == 0){}
  log_d("received data:%d", Serial2.available());

  uint8_t command_length = Serial2.read(); Serial.printf("%x ", command_length);
  uint8_t command[command_length];
  for(uint8_t i=1; i<command_length; i++){
    uint8_t c = Serial2.read();
    M5.Lcd.printf("%d ", c);
    Serial.printf("%x ", c);
    command[i] = c;
  }
  Serial.printf("\n");
  M5.Lcd.printf("\n");

  return (command[2] == 6);
}

uint8_t checksum(uint8_t *cmd) {
  uint8_t len = cmd[0];
  uint8_t sum = 0;
  for (uint8_t i = 0; i < len-1; i++)
   sum += cmd[i];

  return sum & 0xFF;
}

cmd_result send_command (uint8_t *cmd, cmd_result ret_type, uint16_t timeout) {
  uint8_t len = cmd[0];
  uint8_t chksum = checksum(cmd);
  log_d("chksum is %x", chksum);

  if (chksum != cmd[len-1])
    cmd[len-1] = chksum;
  
  Serial2.write(cmd, len);
  if(!wait_command_ack()){
    delay(timeout);
    return ret_type;
  }

  return C_OK;
}

/*
 finish() != true の間歩行
 a 片足が動くたびにfinish()が実行される。
 @return cmd_result
*/
cmd_result walk(bool (*finish)(void)) {
  
  cmd_result r = send_motion(M_PRE_WALK);
  log_d("motion result pre walk is %d", r);
  if (r != C_OK) return r;

  for (;;) {
    r = send_motion(M_WALKL);
    log_d("motion result walkl is %d", r);
    if (r != C_OK) return r;
    if (finish()) {
      r = send_motion(M_POST_WALKL);
      log_d("motion result post walkl is %d", r);
      return r;
    }

    r = send_motion(M_WALKR);
    log_d("motion result walkr is %d", r);
    if (r != C_OK) return r;
    if (finish()) {
      r = send_motion(M_POST_WALKR);
      log_d("motion result post walkr is %d", r);
      return r;
    }
  }
}

cmd_result drive_joint(joints joint, uint8_t speed, uint16_t position) {
  //               size  cmd   which servo to drive     
  //               |     |     |                             speed (small => fast)
  //               |     |     |                             |     position in [0x0000 0xFFFF]
  //               V     v     v                             v     L     H     chksum
  uint8_t cmd[] = {0x0B, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
  cmd[2+(joint-1)/8] += (1<< (joint-1)%8);
  cmd[7] = speed;
  cmd[8] = position & 0xFF;
  cmd[9] = (position >> 8)& 0xFF;
  return send_command(cmd, C_MULTI_SERVO_FAILED, 5000);
}
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