#include <Arduino.h>
#include "Command.h"

#define MOTOR__PWM    5  // выходы arduino

#define DATA_PIN      2  // пин подключен к входу DS
#define LATCH_PIN     4  // пин подключен к входу ST_CP
#define CLOCK_PIN     7  // пин подключен к входу SH_CP


// #define MOTOR_LEFT_AIN1       A0
// #define MOTOR_LEFT_AIN2       A1
// #define MOTOR_LEFT_BIN1       A2
// #define MOTOR_LEFT_BIN2       A3

// #define MOTOR_RIGHT_AIN1      2
// #define MOTOR_RIGHT_AIN2      4
// #define MOTOR_RIGHT_BIN1      7
// #define MOTOR_RIGHT_BIN2      8
//#define STBY 13 

#define MOTOR_LEFT_FORWARD     0b01010000
#define MOTOR_LEFT_BACKWARD    0b10100000
#define MOTOR_RIGHT_FORWARD    0b00001010
#define MOTOR_RIGHT_BACKWARD   0b00000101

#define ROBOT_FORWARD          0b01011010
#define ROBOT_BACKWARD         0b10100101
#define ROBOT_LEFT             0b01010101
#define ROBOT_RIGHT            0b10101010


#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

RF24 radio(9, 10); // "создать" модуль на пинах 9 и 10 Для Уно
// RF24 radio(9,53); // для Меги

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб


// int motorSpeed = 255; //  скорость мотора

enum Direction {
  forward = 0,
  bacward = 1,
  left    = 2,
  right   = 3
};

Direction direct;

// class Motor {
//   private:
//     /* data */
//   public:
//     Motor(/* args */);
//     ~Motor();
// };

// Motor::Motor(/* args */) {

// }

// Motor::~Motor() {

// }


void Motor(Direction direct, byte speed) {
  switch (direct)
  {
    case Direction :: forward: {

      Serial.println("Forward");
      
      digitalWrite(LATCH_PIN, LOW);  // цифра один
      shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, ROBOT_FORWARD);
      digitalWrite(LATCH_PIN, HIGH);

      analogWrite(MOTOR__PWM, speed);  
      break;
    }
    case Direction :: bacward: {

      Serial.println("Backward");

      digitalWrite(LATCH_PIN, LOW);  // цифра один
      shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, ROBOT_BACKWARD);
      digitalWrite(LATCH_PIN, HIGH);
      
      analogWrite(MOTOR__PWM, speed); 
      break;
    }

    case Direction :: right: {

      Serial.println("Right");
      
      digitalWrite(LATCH_PIN, LOW);  // цифра один
      shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, ROBOT_RIGHT);
      digitalWrite(LATCH_PIN, HIGH);

      analogWrite(MOTOR__PWM, speed);  
      break;
    }
    case Direction :: left: {

      Serial.println("Left");

      digitalWrite(LATCH_PIN, LOW);  // цифра один
      shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, ROBOT_LEFT);
      digitalWrite(LATCH_PIN, HIGH);
      
      analogWrite(MOTOR__PWM, speed); 
      break;
    }
  
  default:
    break;
  }
}

void MotorRight(Direction direct, byte speed) {
  switch (direct)
  {
    case Direction :: forward: {
      digitalWrite(LATCH_PIN, LOW);  // цифра один
      shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, MOTOR_RIGHT_FORWARD);
      digitalWrite(LATCH_PIN, HIGH);

      analogWrite(MOTOR__PWM, speed);  
      break;
    }
    case Direction :: bacward: {
      digitalWrite(LATCH_PIN, LOW);  // цифра один
      shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, MOTOR_RIGHT_BACKWARD);
      digitalWrite(LATCH_PIN, HIGH);
      
      analogWrite(MOTOR__PWM, speed); 
      break;
    }
  
  default:
    break;
  }
}

void setup(){

  Serial.begin(9600);
  Serial.println("Start");



  pinMode(MOTOR__PWM, OUTPUT); 

  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);

  digitalWrite(LATCH_PIN, HIGH);

  
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
  radio.startListening();  //начинаем слушать эфир, мы приёмный модуль
  

}

void loop() {


  com recived;
  byte pipeNo, gotByte;
  while ( radio.available(&pipeNo)) {                                 // слушаем эфир со всех труб
    //radio.read( &gotByte, sizeof(gotByte) );                          // чиатем входящий сигнал
    radio.read( &recived, sizeof(com) );

    Serial.print("[Direction] "); Serial.println(recived.direction);
    Serial.print("[Hand] "); Serial.println(recived.hand);
    Serial.print("[Speed] "); Serial.println(recived.speed);

    switch (recived.direction)
    {
    case FORWARD: {
      direct = forward;
      break;
    }
    
    case BACKWARD: {
      direct = bacward;
      break;
    }

    case LEFT: {
      direct = left;
      break;
    }

    case RIGHT: {
      direct = right;
      break;
    }

    default:
      break;
    }

  }

  Motor(direct, recived.speed);

} 