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

SemaphoreHandle_t motion_sem;
SemaphoreHandle_t switch_image_sem;

void printWifiStatus();
void connect_WiFi();
void exec_command(char);
void exec_motion_task(char);
void exec_switch_image_task(char);
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

  M5.Lcd.println("Start RobotControllerServer POI");
  log_d("Start RobotControllerServer POI");

  delay(200);
  // attempt to connect to Wifi network:
  connect_WiFi();

  UDP.begin(udp_port);
  // you're connected now, so print out the status:
  printWifiStatus();
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

void exec_command(char input) {
  if ('0' <= input && input <= '9') {
    exec_motion_task(input);
  }else if ('A' <= input && input <= 'Z'){
    exec_switch_image_task(input);
  }
}

void exec_motion_task(char input) {
  motions motion;
  switch (input) {
      case '0': // bow
        motion = M_OJIGI;
        break;
      default :
        return;
  }
  xTaskCreate(task_motion, "task_motion", 4096, (void*)(&motion), 1, NULL);
}

void exec_switch_image_task(char input) {
  images image;
  switch (input) {
      case 'A':
        image = I_SAMPLE1;
        break;
      case 'B':
        image = I_SAMPLE2;
        break;
      default :
        return;
  }
  xTaskCreate(task_switch_image, "image_switch_motion", 4096, (void*)(&image), 1, NULL);
}

// function for task to send command
void task_motion(void *pvParameters) {
  motions motion = *(motions*) pvParameters;
  if(xSemaphoreTake(motion_sem, 0) != pdPASS) {
    // other task is running
    log_d("request for motion is blocked : %d", motion);
  } else {
    log_d("request for motion is accepted : %d", motion);
    
    cmd_result r = send_motion(motion);
    /*
    delay(10000); 
    cmd_result r = C_OK;
    */
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