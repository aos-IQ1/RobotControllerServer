#include "config.h"
#include "motion.h"

#include <stdint.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <M5Stack.h>
#include <Arduino.h>

typedef enum { 
  I_SAMPLE
} images;

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
  xSemaphoreGive(motion_sem);

  switch_image_sem = xSemaphoreCreateBinary();
  if(switch_image_sem == NULL) {
    log_e("fail to create semaphore : switch_image_sem");
    while(1);
  }
  xSemaphoreGive(switch_image_sem);

  M5.Lcd.println("Start RobotControllerServer POI");
  log_d("Start RobotControllerServer POI");

  // attempt to connect to Wifi network:
  //connect_WiFi();

  //UDP.begin(udp_port);   
  // you're connected now, so print out the status:
  //printWifiStatus();
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

  // begin debug
  M5.update();
  if(M5.BtnA.wasPressed()){
    motions motion = M_OJIGI;
    xTaskCreate(task_motion, "task_motion", 4096, (void*)(&motion), 1, NULL);
  }
  if(M5.BtnC.wasPressed()){
    images image = I_SAMPLE;
    xTaskCreate(task_switch_image, "task_switch_image", 4096, (void*)(&image), 1, NULL);
  }
  // end debug
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
// returns succsess or not
void task_motion(void *pvParameters) {
  motions motion = *(motions*) pvParameters;
  if(xSemaphoreTake(motion_sem, 0) != pdTRUE) {
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
// returns succsess or not
void task_switch_image(void *pvParameters) {
  images image = *(images*) pvParameters;
  if(xSemaphoreTake(switch_image_sem, 0) != pdTRUE) {
    log_d("request for switch image is blocked : %d", image);
  } else {
    log_d("request for switch image is accepted : %d", image);
    // cmd_result r = send_motion(motion);
    delay(10000); 
    xSemaphoreGive(switch_image_sem);
  }
  vTaskDelete(NULL);
}