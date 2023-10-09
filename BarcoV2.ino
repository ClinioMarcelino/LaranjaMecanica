#include <ESP8266WiFi.h>
#include <espnow.h>

#define IN1 5
#define IN2 16
#define IN3 12
#define IN4 13

#define ENA 4
#define ENB 15
#define ledS 2


#define SIGNAL_TIMEOUT 1000  // This is signal timeout in milli seconds. We will reset the data if no signal

int count=0;

unsigned long lastRecvTime = 0;

//Estrutura para envio dos dados. Deve ser a mesma tanto no emissor como no receptor.
typedef struct struct_message
{
  byte leftX;
  byte leftY;
  byte leftBot;
  byte rightX;
  byte rightY;
  byte rightBot;
  byte swDir;
  byte swEsq;
} struct_message;

//Cria uma struct_message chamada myData
struct_message myData;

//Funcao de Callback executada quando a mensagem for recebida
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  if(len==0)
    return;

  memcpy(&myData, incomingData, sizeof(myData));
  mapAndWriteValues();
  lastRecvTime = millis();
  Serial.print("Bytes received: ");
  Serial.println(len);
  // Serial.print("  left x:   ");
  // Serial.print(myData.leftX);
  // Serial.print("  left y:   ");
  // Serial.print(myData.leftY);
  // Serial.print("  left bot:   ");
  // Serial.print(myData.leftBot);
  // Serial.print("  right x:   ");
  // Serial.print(myData.rightX);
  // Serial.print("  right y:   ");
  // Serial.print(myData.rightY);
  // Serial.print("  right bot:   ");
  // Serial.print(myData.rightBot);
  // Serial.print("  Switch Direita: ");
  // Serial.print(myData.swDir);
  // Serial.print("  Switch Esquerda: ");
  // Serial.println(myData.swEsq);

}

void mapAndWriteValues(){
    if(myData.rightY==127){
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
    }
    else if(myData.rightY>127){
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      analogWrite(ENA, myData.rightY);
    }
    else if(myData.rightY<127){
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      analogWrite(ENA, map(myData.rightY,127,0,127,255));
    }

    if(myData.leftY==127){
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);
    }
    else if(myData.leftY>127){
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
      analogWrite(ENB, myData.leftY);
    }
    else if(myData.leftY<127){
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
      analogWrite(ENB, map(myData.leftY,127,0,127,255));
    }

    // if(myData.swDir==1){
    //   if(count==16){
    //     count=0;
    //   }
    //   else if(count<8){
    //     digitalWrite(ledS,HIGH);
    //   }
    //   else{
    //     digitalWrite(ledS,LOW);
    //   }
    //   count=count+1;
    // }else{
    //   digitalWrite(ledS,LOW);
    //   count=0;
    // }

    // Serial.println(count);
}

void setInputDefaultValues(){
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);
}

void setUpPinModes(){
  //INICIAÇÃO DOS PINOS  E INFORMANDO Q ELES FUNCIONAM NO MODO OUTPUT
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ledS, OUTPUT);
  // pinMode(ledVerm, OUTPUT);
  // mapAndWriteValues();
}

void setup()
{
  setUpPinModes();
  Serial.begin(115200);

  //Coloca o dispositivo no modo Wi-Fi Station
  WiFi.mode(WIFI_STA);

  //Inicializa o ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Erro ao inicializar o ESP-NOW");
    return;
  }
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
  setInputDefaultValues();
  digitalWrite(ledS, LOW);
  
}

void loop()
{
  unsigned long now = millis();
  if ( now - lastRecvTime > SIGNAL_TIMEOUT ) 
  {
    // setInputDefaultValues();
    // mapAndWriteValues();  
  }
  
}