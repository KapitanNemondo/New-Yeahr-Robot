 #include <Arduino.h>
#include "C:\Projects\New Year\Robot Happy New Yeahr\include\Command.h"

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

}

void loop() {
  // put your main code here, to run repeatedly:

  com transmition;

  transmition.direction = FORWARD;
  transmition.speed = 100;
  transmition.hand = 10;
  Serial.println("Send-1...");
  radio.write(&transmition, sizeof(transmition));

  delay(2000);

  transmition.direction = BACKWARD;
  transmition.speed = 200;
  transmition.hand = 30;
  Serial.println("Send-2...");
  radio.write(&transmition, sizeof(transmition));


  delay(2000);
  transmition.direction = RIGHT;
  transmition.speed = 50;
  transmition.hand = 10;
  Serial.println("Send-3...");
  radio.write(&transmition, sizeof(transmition));

  delay(2000);
  transmition.direction = LEFT;
  transmition.speed = 100;
  transmition.hand = 10;
  Serial.println("Send-4...");
  radio.write(&transmition, sizeof(transmition));

  delay(5000);

}




