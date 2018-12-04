#include <stdint.h>
#include "motion.h"
#include <M5Stack.h>
#include <Arduino.h>

bool wait_command_ack();


uint16_t motion_address_table[] = {
    0x0B80, // M001  お辞儀
    0x1380, // M002  ホームhポジション
    0x1B80, // M003  PreWalk
    0x2380, // M004  WalkL
    0x2B80, // M005  WalkR
    0x3380, // M005  PostWalkR
    0x3B80, // M005  PostWalkL
};

// command templates
uint8_t stop_motion_command[] = {0x09,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x0b};
uint8_t reset_program_counter_command[] = {0x11,0x00,0x02,0x02,0x00,0x00,0x4B,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x64};
uint8_t address_command[] = {0x07,0x0C,0x80,0x0B,0x00,0x00,0x9E};
uint8_t motion_restart_command[] = {0x09,0x00,0x02,0x00,0x00,0x00,0x03,0x00,0x0E};

/*
  ロボットに-oモーションを送信
*/
cmd_result send_motion(motions motion) {

  uint16_t motion_address = motion_address_table[motion];
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
  /*Serial2.write(address_command,address_command[0]);
  if(!wait_command_ack()){
    delay(5000);
    return C_CALL_FAILED;
  }*/

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