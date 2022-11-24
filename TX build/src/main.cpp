#include <Arduino.h>
#include "C:\Projects\New Year\Robot Happy New Yeahr\include\Command.h"

#define SPEED           A1
#define DIRECT_FOBA     A2
#define DIRECT_LERI     A3

#define KORREKT_DIRECT  20
#define KORRET_SPEED    5

#define PIN_UP_HAND         2
#define PIN_DOWN_HAND       3

byte direct_foba, last_foba, direct_leri, last_leri, speed, last_speed;

bool flag_send;
bool flag_up, flag_down;


#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

RF24 radio(9, 10); // "создать" модуль на пинах 9 и 10 Для Уно
// RF24 radio(9,53); // для Меги

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);

  radio.begin(); //активировать модуль
  radio.setAutoAck(1);         //режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);    //(время между попыткой достучаться, число попыток)
  radio.enableAckPayload();    //разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);     //размер пакета, в байтах

  radio.openReadingPipe(1, address[0]);     //хотим слушать трубу 0
  radio.setChannel(0x60);  //выбираем канал (в котором нет шумов!)

  radio.setPALevel (RF24_PA_MAX); //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_250KBPS); //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  //должна быть одинакова на приёмнике и передатчике!
  //при самой низкой скорости имеем самую высокую чувствительность и дальность!!

  radio.powerUp(); //начать работу
  radio.stopListening();  //не слушаем радиоэфир, мы передатчик 

  pinMode(DIRECT_FOBA, INPUT);
  pinMode(DIRECT_LERI, INPUT);
  pinMode(SPEED, INPUT);
  pinMode(PIN_UP_HAND, INPUT_PULLUP);
  pinMode(PIN_DOWN_HAND, INPUT_PULLUP);

}

void loop() {
  // put your main code here, to run repeatedly:

  com transmition;

  direct_foba = map(analogRead(DIRECT_FOBA), 0, 1023, 0, 255);
  direct_leri = map(analogRead(DIRECT_LERI), 0, 1023, 0, 255);
  speed = map(analogRead(SPEED), 0, 1023, 0, 255);

  bool now_up_state = !digitalRead(PIN_UP_HAND);
  bool now_down_state = !digitalRead(PIN_DOWN_HAND);

  if (now_up_state && !flag_up) {  // обработчик нажатия
    flag_up = true;
  }
  if (!now_up_state && flag_up) {  // обработчик отпускания
    flag_up = false;  
  }

  if () {  // обработчик нажатия
    flag_down = true;
  }
  if (!now_down_state && flag_down) {  // обработчик отпускания
    flag_down = false;  
  }

  if ( flag_up ) {

    flag_send = true;
    transmition.hand = UP_HAND;

  } else if ( flag_down ) {

    flag_send = true
    transmition.hand = DOWN_HAND;

  } else if ( !flag_down && !flag_up ) {

    flag_send = true
    transmition.hand = STOP_HAND;
    
  }


  if (speed > last_speed + KORRET_SPEED || speed < last_speed - KORRET_SPEED) {
    last_speed = speed;
    Serial.print("[Speed] "); Serial.println(speed);
    transmition.speed = speed;

    flag_send = true;

  }

  if (direct_leri > last_leri + KORREKT_DIRECT || direct_leri < last_leri - KORREKT_DIRECT) {
    last_leri = direct_leri;
 
    Serial.print("[Direct] "); Serial.println(direct_leri);

    if (direct_leri < 70) {
      Serial.println("[Direct] Right");
      transmition.direction = LEFT;
      // transmition.speed = 100;

    } else if (direct_leri > 160) {
      Serial.println("[Direct] Left");
      transmition.direction = RIGHT;
      // transmition.speed = 100;

    } else {
      Serial.println("[Direct] Stop");
      transmition.direction = STOP;
    }

    flag_send = true;
    

  }

  if (direct_foba > last_foba + KORREKT_DIRECT || direct_foba < last_foba - KORREKT_DIRECT) {
    last_foba = direct_foba;
 
    Serial.print("[Direct] "); Serial.println(direct_foba);

    if (direct_foba < 70) {
      Serial.println("[Direct] Forward");
      transmition.direction = FORWARD;
      // transmition.speed = 100;

    } else if (direct_foba > 160) {
      Serial.println("[Direct] Backward");
      transmition.direction = BACKWARD;
      // transmition.speed = 100;

    } else {
      Serial.println("[Direct] Stop");
      transmition.direction = STOP;
    }

    flag_send = true;
    

  }

  if (flag_send) {
    radio.write(&transmition, sizeof(transmition));
    flag_send = false;
  }


}




