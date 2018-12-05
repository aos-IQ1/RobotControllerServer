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

motions motion_list[14] = {
  M_OJIGI,
  M_HOME_POSITION,
  M_PRE_WALK, // dummy for walk command
  M_TO_LEFT,
  M_TO_RIGHT,
  M_TURN_LEFT,
  M_TURN_RIGHT,
  M_GET_UP_U,
  M_GET_UP_A,
  M_PUNCHL,
  M_PUNCHR,
  M_UTUBUSE,
  M_AOMUKE,
  M_WAVE_HAND
};

images image_list[4] = {
  I_ICON,
  I_WEB,
  I_CYBER,
  I_SUITS
};

SemaphoreHandle_t motion_sem;
SemaphoreHandle_t switch_image_sem;

void printWifiStatus();
void connect_WiFi();
void exec_command(char);
void task_motion(void*);
void task_walk(void*);
bool is_waik_finished();
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

  M5.Lcd.println("Start RobotControllerServer POI");
  log_d("Start RobotControllerServer POI");

  delay(200);
  // attempt to connect to Wifi network:
  connect_WiFi();

  UDP.begin(udp_port);
  // you're connected now, so print out the status:
  printWifiStatus();

  delay(5000);
  render_image(I_ICON);
}

void loop(){ 
  if(UDP.parsePacket() > 0) {
    char in;
    UDP.read(&in, 1);
    Serial.printf("%c\n", in);
    exec_command(in);
    UDP.flush();
  }
  delay(200);
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  M5.Lcd.print("SSID: ");
  M5.Lcd.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  M5.Lcd.print("IP Address: ");
  M5.Lcd.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  M5.Lcd.print("signal strength (RSSI):");
  M5.Lcd.print(rssi);
  M5.Lcd.println(" dBm");

  M5.Lcd.println("Connected");
}

// WiFiに接続
void connect_WiFi(){
    WiFi.disconnect(true);
    WiFi.begin(ssid,password);

    while(WiFi.status() != WL_CONNECTED){
        delay(1000);
        log_d("WiFi connecting...");
    }
    log_d("WiFi Connected!");
}

// execute command according to input
void exec_command(char input) {
  static motions motion;
  static images image;
  if ('a' <= input && input <= ('a' + 13)) {
    if(input == 'c') { 
      // walking command
      xTaskCreate(task_walk, "task_walk", 4096, NULL, 1, NULL);
    } else {
      motion = motion_list[input - 'a'];
      xTaskCreate(task_motion, "task_motion", 4096, (void*)(&motion), 1, NULL);
    }
  }else if ('A' <= input && input <= 'D'){
    image = image_list[input - 'A'];
    xTaskCreate(task_switch_image, "task_switch_image", 4096, (void*)(&image), 1, NULL);
  }
}

// function for task to send command
// walking command is not included
void task_motion(void *pvParameters) {
  motions motion = *(motions*) pvParameters;
  if(xSemaphoreTake(motion_sem, 0) != pdPASS) {
    // other task is running
    log_d("request for motion is blocked : %x", motion);
  } else {
    log_d("request for motion is accepted : %x", motion);
    cmd_result r = send_motion(motion);
    /*
    delay(10000); 
    cmd_result r = C_OK;
    */
    log_d("result is %x", r);
    xSemaphoreGive(motion_sem);
  }
  vTaskDelete(NULL);
}

// function for task to send walk command
void task_walk(void *pvParameters) {
   if(xSemaphoreTake(motion_sem, 0) != pdPASS) {
    // other task is running
    log_d("request for walking is blocked");
  } else {
    log_d("request for walking is accepted");
    cmd_result r = walk(is_waik_finished);
    /*
    delay(10000); 
    cmd_result r = C_OK;
    */
    log_d("result is %x", r);
    xSemaphoreGive(motion_sem);
  }
  vTaskDelete(NULL); 
}

// predicate for finshing walking 
bool is_waik_finished() {
  static uint8_t step = 0;
  step++;
  if (step == 2) {
    step = 0;
    return true;
  } else {
    return false;
  }
}

// function for task to switch facial image
void task_switch_image(void *pvParameters) {
  images image = *(images*) pvParameters;
  if(xSemaphoreTake(switch_image_sem, 0) != pdPASS) {
    // other task is running
    log_d("request for switch image is blocked : %x", image);
  } else {
    log_d("request for switch image is accepted : %x", image);
    render_image(image);
    xSemaphoreGive(switch_image_sem);
  }
  vTaskDelete(NULL);
}