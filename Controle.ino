/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-one-to-many-esp32-esp8266/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#include <esp_now.h>
#include <WiFi.h>

// REPLACE WITH YOUR ESP RECEIVER'S MAC ADDRESS
uint8_t broadcastAddress[] = {0x48, 0x55, 0x19, 0xE4, 0x81, 0x51};

struct comandos {
  byte leftX;
  byte leftY;
  byte leftBot;
  byte rightX;
  byte rightY;
  byte rightBot;
};


comandos comando;

esp_now_peer_info_t peerInfo;

//This function is used to map 0-4095 joystick value to 0-254. hence 127 is the center value which we send.
//It also adjust the deadband in joystick.
//Jotstick values range from 0-4095. But its center value is not always 2047. It is little different.
//So we need to add some deadband to center value. in our case 1800-2200. Any value in this deadband range is mapped to center 127.
int mapAndAdjustJoystickDeadBandValues(int value)
{
  if (value >= 2200)
  {
    value = map(value, 2200, 4095, 127, 254);
  }
  else if (value <= 1800)
  {
    value = (value == 0 ? 0 : map(value, 1800, 0, 127, 0));  
  }
  else
  {
    value = 127;
  }
  // Serial.println(value);  
  return value;
}

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  Serial.print("Packet to: ");
  // Copies the sender mac address to a string
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
  Serial.print(" send status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
 
  byte botDir = 0;
  byte botEsq = 0;

void setup() {
  Serial.begin(9600);
 
  WiFi.mode(WIFI_STA);
 
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_send_cb(OnDataSent);
   
  // register peer
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  // register first peer  
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  pinMode(23, INPUT_PULLUP);
  pinMode(22,INPUT_PULLUP);
}
 
void loop() {

  if(!digitalRead(23)==1)
    if(botDir==1)
      botDir=0;
    else if(botDir==0)
      botDir=1;

  if(!digitalRead(22)==1)
    if(botEsq==1)
      botEsq=0;
    else if(botEsq==0)
      botEsq=1;

  comando.leftBot = botDir;
  comando.rightBot = botEsq;
  comando.leftX = mapAndAdjustJoystickDeadBandValues(analogRead(34));
  comando.leftY = mapAndAdjustJoystickDeadBandValues(analogRead(35));
  comando.rightX = mapAndAdjustJoystickDeadBandValues(analogRead(32));
  comando.rightY = mapAndAdjustJoystickDeadBandValues(analogRead(33));
 
  esp_err_t result = esp_now_send(0, (uint8_t *) &comando, sizeof(comandos));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(250);
}
