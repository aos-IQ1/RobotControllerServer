#include "config.h"
#include "motion.h"
#include "image.h"

#include <stdint.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <M5Stack.h>
#include <Arduino.h>

const char* ssid = AP_SSID; //  your network SSID (name)
const char* password = AP_PASSWORD;    // your network password (use for WPA, or use as key for WEP)

int status = WL_IDLE_STATUS;
int udp_port = 3333;
WiFiUDP UDP;

char WiFibuff[128];

SemaphoreHandle_t motion_sem;
SemaphoreHandle_t switch_image_sem;

void printWifiStatus();
void connect_WiFi();
void task_motion(void*);
void task_switch_image(void*);

void setup() {
  Serial2.begin(115200, SERIAL_8E1, 16, 17);
  M5.begin();

  motion_sem = xSemaphoreCreateBinary();
  if(motion_sem == NULL) {
    log_e("fail to create semaphore : motion_sem");
    while(1);
  }
  xSemaphoreGive(motion_sem); // initialize to 'empty'

  switch_image_sem = xSemaphoreCreateBinary();
  if(switch_image_sem == NULL) {
    log_e("fail to create semaphore : switch_image_sem");
    while(1);
  }
  xSemaphoreGive(switch_image_sem); // initalize to 'empty'

  render_image(I_SAMPLE1);

  M5.Lcd.println("Start RobotControllerServer POI");
  log_d("Start RobotControllerServer POI");

  // attempt to connect to Wifi network:
  //connect_WiFi();

  //UDP.begin(udp_port);   
  // you're connected now, so print out the status:
  //printWifiStatus();
}

bool sample_finish() {
  static uint8_t step = 0;
  step++;
  if (step == 2) {
    step = 0;
    return true;
  } else {
    return false;
  }
}

void loop(){ 
  /*
  if(UDP.parsePacket() > 0) {
    UDP.read(WiFibuff, 128);
    Serial.print(WiFibuff);
    UDP.flush();
  }else{
    M5.Lcd.println("NO UDP");
  }
  */

  M5.Lcd.println("Loop");
  cmd_result r;

if (Serial.available()) {
    delay(10);
    uint8_t len = Serial.available();
    char line[len];
    for (uint8_t i = 0; i < len; i++) line[i] = Serial.read();
    uint8_t joint, speed;
    uint16_t vol;
    sscanf(line, "%d:%d:%d", &joint, &speed, &vol);
    //uint16_t vol = analogRead(36)*0xFFFF/4095;
    log_d("vol is %d", vol);
    r = drive_joint((joints)joint, speed, vol);
    log_d("drive joint%d result is %d", joint, r);
  } else {

    r = send_motion(M_OJIGI);
    log_d("motion result ojigi is %d", r);
    
    r = walk(sample_finish);
    log_d("walk result is %d", r);

    
    r = drive_joint(J_SHOULDER_L_ROLL, 5, 10000);
    log_d("drive joint result is %d", r);
    r = drive_joint(J_SHOULDER_L_PITCH, 5, 3000);
    log_d("drive joint result is %d", r);
    delay(500);
    r = drive_joint(J_SHOULDER_L_ROLL, 5, 7500);
    log_d("drive joint result is %d", r);
    r = drive_joint(J_SHOULDER_L_PITCH, 5, 7500);
    log_d("drive joint result is %d", r);

    delay(3000);
  }

}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");

  M5.Lcd.printf("Connected");
}

// WiFiに接続
void connect_WiFi(){
    WiFi.disconnect(true);
    WiFi.begin(ssid,password);

    while(WiFi.status() != WL_CONNECTED){
        delay(1000);
        M5.Lcd.println("WiFi connecting...");
    }
    M5.Lcd.println("WiFi Connected!");
}

// function for task to send command
void task_motion(void *pvParameters) {
  motions motion = *(motions*) pvParameters;
  if(xSemaphoreTake(motion_sem, 0) != pdPASS) {
    // other task is running
    log_d("request for motion is blocked : %d", motion);
  } else {
    log_d("request for motion is accepted : %d", motion);
    
    // cmd_result r = send_motion(motion);
    delay(10000); 
    cmd_result r = C_OK;
    log_d("result is %d", r);
    xSemaphoreGive(motion_sem);
  }
  vTaskDelete(NULL);
}

// task function for task to switch facial image
void task_switch_image(void *pvParameters) {
  images image = *(images*) pvParameters;
  if(xSemaphoreTake(switch_image_sem, 0) != pdPASS) {
    // other task is running
    log_d("request for switch image is blocked : %d", image);
  } else {
    log_d("request for switch image is accepted : %d", image);
    render_image(image);
    xSemaphoreGive(switch_image_sem);
  }
  vTaskDelete(NULL);
}