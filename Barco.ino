#include <ESP8266WiFi.h>
#include <espnow.h>

#define IN1 16
#define IN2 5
#define IN3 12
#define IN4 13

#define ENA 4
#define ENB 15

#define SIGNAL_TIMEOUT 1000  // This is signal timeout in milli seconds. We will reset the data if no signal

unsigned long lastRecvTime = 0;

//Structure example to receive data
//Must match the sender structure
struct comandos {
  byte leftX;
  byte leftY;
  byte leftBot;
  byte rightX;
  byte rightY;
  byte rightBot;
};

//Create a struct_message called myData
comandos comando;

void mapAndWriteValues(){
  if(comando.rightBot){
    if(comando.rightY==127){
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
    }
    else if(comando.rightY>127){
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      analogWrite(ENA, comando.rightY);
    }
    else if(comando.rightY<127){
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      analogWrite(ENA, map(comando.rightY,127,0,127,255));
    }
  }

  if(comando.leftBot){
    if(comando.leftY==127){
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);
    }
    else if(comando.leftY>127){
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
      analogWrite(ENB, comando.leftY);
    }
    else if(comando.leftY<127){
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
      analogWrite(ENB, map(comando.leftY,127,0,127,255));
    }
  }
}

//callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  if(len==0)
    return;

  memcpy(&comando, incomingData, sizeof(comando));
  mapAndWriteValues();
  lastRecvTime = millis();
  // Serial.print("Bytes received: ");
  // Serial.print(len);
  // Serial.print("  left x:   ");
  // Serial.print(comando.leftX);
  // Serial.print("  left y:   ");
  // Serial.print(comando.leftY);
  // Serial.print("  left bot:   ");
  // Serial.print(comando.leftBot);
  // Serial.print("  right x:   ");
  // Serial.print(comando.rightX);
  // Serial.print("  right y:   ");
  // Serial.print(comando.rightY);
  // Serial.print("  right bot:   ");
  // Serial.println(comando.rightBot);

}

void setUpPinModes(){
  //INICIAÇÃO DOS PINOS  E INFORMANDO Q ELE FUNCIONARAM NO MODO OUTPUT
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  mapAndWriteValues();
}

 
void setup() {

  setUpPinModes();
  //Initialize Serial Monitor
  Serial.begin(9600);
  
  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  //Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);

}
 
void loop() {
//Check Signal lost.
  unsigned long now = millis();
  if ( now - lastRecvTime > SIGNAL_TIMEOUT ) 
  {
    // setInputDefaultValues();
    mapAndWriteValues();  
  }
}
