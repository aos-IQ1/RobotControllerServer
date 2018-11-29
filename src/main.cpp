#include <WiFi.h>
#include <WiFiUdp.h>
#include <M5Stack.h>

#include "password.h"

const char* ssid = AP_SSID; //  your network SSID (name)
const char* password = AP_PASSWORD;    // your network password (use for WPA, or use as key for WEP)

int status = WL_IDLE_STATUS;
int udp_port = 3333;
WiFiUDP UDP;

char WiFibuff[128];
bool command_send_success;  // コマンドのackが成功を示していればtrue,それ以外ならばfalse

void printWifiStatus();
void connect_WiFi();
void wait_command_ack();

void setup() {
  Serial.begin(115200);
  M5.begin();
  M5.Lcd.println("Start RobotControllerServer");

  // attempt to connect to Wifi network:
  connect_WiFi();

  UDP.begin(udp_port);   
  // you're connected now, so print out the status:
  printWifiStatus();
}


void loop(){ 
  if(UDP.parsePacket() > 0) {
    UDP.read(WiFibuff, 128);
    Serial.print(WiFibuff);
    UDP.flush();
  }else{
    M5.Lcd.println("NO UDP");
  }


  delay(500);
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

/*
  ロボットからのコマンドの返り値をwaitする
  @return 返り値が成功ならばtrue,失敗ならばfalse
*/
void wait_command_ack(){
  while(Serial.available() == 0){}

  uint8_t command_length = Serial.read();
  uint8_t commands[10];
  for(uint8_t i=1; i<command_length; i++){
    commands[i] = Serial.read();
  }


  command_send_success = (commands[2] == 6);
}