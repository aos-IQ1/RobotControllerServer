#include "config.h"
#include "motion.h"

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


void printWifiStatus();
void connect_WiFi();
void wait_command_ack();

void setup() {
  Serial2.begin(115200, SERIAL_8E1, 16, 17);
  M5.begin();
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
  cmd_result r = send_motion(M_OJIGI);
  log_d("motion result ojigi is %d", r);
  
  r = walk(sample_finish);
  log_d("walk result is %d", r);

  delay(5000);
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