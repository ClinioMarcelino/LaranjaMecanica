#include <esp_now.h>
#include <WiFi.h>

#define joyRx 35
#define joyRy 34
#define joySwR 26 //Botão joystick direita
#define joyLx 32
#define joyLy 33
#define joySwL 25 //Botão joystick esquerda
#define swR 27 // seitch direita 
#define swL 23 // switch esquerda 

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] =  {0x48,0x55,0x19,0xE1,0x32,0xD0};
// uint8_t broadcastAddress[] =  {0x48,0x55,0x19,0xE4,0x81,0x51};
//48:55:19:E4:81:51 Placa TESTE
//48:55:19:e1:32:D0 Placa BARCO


// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  byte leftX;
  byte leftY;
  byte leftBot;
  byte rightX;
  byte rightY;
  byte rightBot;
  byte swDir;
  byte swEsq;
} struct_message;

// Create a struct_message called myData
struct_message myData;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

int mapAndAdjustJoystickDeadBandValues(int value, bool reverse)
{
  if (value >= 2000)
  {
    value = map(value, 2000, 4095, 127, 255);
  }
  else if (value <= 1650)
  {
    value = (value == 0 ? 0 : map(value, 1650, 0, 127, 0));  
  }
  else
  {
    value = 127;
  }

  if (reverse)
  {
    value = 254 - value;
  }
  //Serial.println(value);  
  return value;
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  pinMode(swR,INPUT_PULLUP);
  pinMode(swL,INPUT_PULLUP);
  pinMode(joySwR,INPUT_PULLUP);
  pinMode(joySwL,INPUT_PULLUP);

}
 
void loop() {
  // Set values to send
  myData.leftX    = mapAndAdjustJoystickDeadBandValues(analogRead(joyLx), false);
  myData.leftY    = mapAndAdjustJoystickDeadBandValues(analogRead(joyLy), false);
  myData.rightX    = mapAndAdjustJoystickDeadBandValues(analogRead(joyRx), false);
  myData.rightY    = mapAndAdjustJoystickDeadBandValues(analogRead(joyRy), false);
  myData.rightBot   = !digitalRead(joySwL);
  myData.leftBot   = !digitalRead(joySwR);
  myData.swEsq   = !digitalRead(swL);
  myData.swDir   = !digitalRead(swR);


  // Send message via ESP-NOW
esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
// CAMADA DE TESTE DE DEAD RANGE
  // Serial.print("Direita X = ");
  // Serial.print(analogRead(joyRx));
  // Serial.print("  Direita Y = ");
  // Serial.println(analogRead(joyRy));
  // Serial.print("Esquerda X = ");
  // Serial.print(analogRead(joyLx));
  // Serial.print("  Esquerda Y = ");
  // Serial.println(analogRead(joyLy));

// CAMADA TESTE MAPEAMENTO
  Serial.print("  left x:   ");
  Serial.print(myData.leftX);
  Serial.print("  left y:   ");
  Serial.print(myData.leftY);
  Serial.print("  left bot:   ");
  Serial.print(myData.leftBot);
  Serial.print("  right x:   ");
  Serial.print(myData.rightX);
  Serial.print("  right y:   ");
  Serial.print(myData.rightY);
  Serial.print("  right bot:   ");
  Serial.print(myData.rightBot);
  Serial.print("  Switch Direita: ");
  Serial.print(myData.swDir);
  Serial.print("  Switch Esquerda: ");
  Serial.println(myData.swEsq);

  delay(125);
}