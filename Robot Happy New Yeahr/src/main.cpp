#include <Arduino.h>
#include "Command.h"

#define MOTOR__PWM    5  // выходы arduino
#define HAND__PWM     6

#define DATA_PIN      2  // пин подключен к входу DS
#define LATCH_PIN     4  // пин подключен к входу ST_CP
#define CLOCK_PIN     7  // пин подключен к входу SH_CP

#define END_STOP      8  // пин конецевика на руке

#define SPEED         3  // скорость двигателя захвата
#define DIRECTION     A0 


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
#define MOTOR_STOP             0b00000000

#define ROBOT_FORWARD          0b01011010
#define ROBOT_BACKWARD         0b10100101
#define ROBOT_LEFT             0b01010101
#define ROBOT_RIGHT            0b10101010

#define HAND_UP                0b10100000
#define HAND_DOWN              0b01010000
#define HAND_STOP              0b00000000


#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

RF24 radio(9, 10); // "создать" модуль на пинах 9 и 10 Для Уно
// RF24 radio(9,53); // для Меги

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб

uint32_t timer;


// int motorSpeed = 255; //  скорость мотора

enum Direction {
  forward = 0,
  bacward = 1,
  left    = 2,
  right   = 3,
  stop    = 4
};

enum DirectHand {
  up        = 0,
  dw        = 1,
  stopHand  = 2
};

enum StateEnd {
  go_up    = 0,
  go_down  = 1,
//  stopEnd  = 2
};

Direction direct;
DirectHand hand_dir, lastdir, zachvat_dir;
StateEnd flag_stop;

com recived;


bool read_end_stop;


void Motor(Direction direct, byte speed_rs) {
  byte speed = 100;
  switch (direct)
  {
    case Direction :: stop: {
      analogWrite(MOTOR__PWM, 0); 
      //Serial.println("Stop");
      break;
    }

    case Direction :: forward: {

      //Serial.println("Forward");
     
      shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, ROBOT_FORWARD);

      analogWrite(MOTOR__PWM, speed);  
      break;
    }
    case Direction :: bacward: {

      //Serial.println("Backward");

      shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, ROBOT_BACKWARD);
     
      analogWrite(MOTOR__PWM, speed); 
      break;
    }

    case Direction :: right: {

      //Serial.println("Right");
      
      shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, ROBOT_RIGHT);

      analogWrite(MOTOR__PWM, speed);  
      break;
    }
    case Direction :: left: {

      //Serial.println("Left");

      shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, ROBOT_LEFT);
      
      analogWrite(MOTOR__PWM, speed); 
      break;
    }

    
  
  default:
    break;
  }
}

void Hand(DirectHand direct) {
  byte speed = 50;
  
  switch (direct)
  {
    case DirectHand :: up: {

      
      shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, HAND_UP);
      
      analogWrite(HAND__PWM, speed);  
      break;
    }
    case DirectHand :: dw: {

      shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, HAND_DOWN);

      analogWrite(HAND__PWM, speed); 
      break;
    }

    case DirectHand :: stopHand: {

      shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, HAND_STOP);

      analogWrite(HAND__PWM, 0); 
    }
  
  default:
    break;
  }
}

void GoHand(DirectHand recived_direct) {
  static bool flag;
  read_end_stop = !digitalRead(END_STOP);

  if (read_end_stop && recived_direct == DirectHand :: dw) {
    Hand(DirectHand :: up);
    flag = true;
    timer = millis();
  }

  if (read_end_stop && recived_direct == DirectHand :: up) {
    Hand(DirectHand :: dw);
    flag = true;
    timer = millis();
  }

  if (flag && millis() - timer > 1000) {
    flag = false;
    Hand(DirectHand :: stopHand);
  }

  if (!flag) {
    Hand(recived_direct);
  }
  
}

void ZachVat(DirectHand direct) {
  static bool flag;

  if (direct == DirectHand :: dw && flag == false) {
    digitalWrite(DIRECTION, HIGH);
    digitalWrite(SPEED, LOW);
    flag = true;
    // delay(200);
    // analogWrite(SPEED, 0);

  } else if (direct == DirectHand :: up && flag == false) {
    digitalWrite(DIRECTION, LOW);
    digitalWrite(SPEED, HIGH);
    flag = true;
    // delay(200);
    // analogWrite(SPEED, 0);

  } else if (direct == DirectHand :: stopHand) {
    digitalWrite(SPEED, LOW);
    digitalWrite(DIRECTION, LOW);
    flag = false;
    
  }
}

void setup(){

  Serial.begin(9600);
  Serial.println("Start");

  pinMode(END_STOP, INPUT_PULLUP);

  pinMode(MOTOR__PWM, OUTPUT);
  pinMode(HAND__PWM, OUTPUT);

  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);

  pinMode(SPEED, OUTPUT);
  pinMode(DIRECTION, OUTPUT);

  digitalWrite(LATCH_PIN, HIGH);


  
  radio.begin(); //активировать модуль
  radio.setAutoAck(1);         //режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);    //(время между попыткой достучаться, число попыток)
  radio.enableAckPayload();    //разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);     //размер пакета, в байтах

  radio.openReadingPipe(1, address[0]);     //хотим слушать трубу 0
  radio.setChannel(0x60);  // выбираем канал (в котором нет шумов!)

  radio.setPALevel (RF24_PA_MAX); //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_250KBPS); //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  //должна быть одинакова на приёмнике и передатчике!
  //при самой низкой скорости имеем самую высокую чувствительность и дальность!!

  radio.powerUp(); //начать работу
  radio.startListening();  //начинаем слушать эфир, мы приёмный модуль
  
  recived.speed = 50;

  flag_stop = StateEnd :: go_up;

  while (flag_stop == StateEnd :: go_up) {
    
    read_end_stop = !digitalRead(END_STOP);

    Serial.print("[Hand set correct] ");  Serial.println(read_end_stop);

    if (read_end_stop) {
      flag_stop = StateEnd :: go_down;
      digitalWrite(LATCH_PIN, LOW);  // цифра один

      Hand(DirectHand :: stopHand);
      Motor(Direction :: stop, 0);

      digitalWrite(LATCH_PIN, HIGH);

    } else {
      digitalWrite(LATCH_PIN, LOW);  // цифра один

      Hand(DirectHand :: up);
      Motor(Direction :: stop, 0);

      digitalWrite(LATCH_PIN, HIGH);
    }
  }

  recived.hand = DirectHand :: stopHand;
  recived.direction = Direction :: stop;
  direct = Direction :: stop;
  zachvat_dir = DirectHand :: stopHand;

}

void loop() {


  
  byte pipeNo;
  while ( radio.available(&pipeNo)) {                                 // слушаем эфир со всех труб

    //radio.read( &gotByte, sizeof(gotByte) );                          // чиатем входящий сигнал
    radio.read( &recived, sizeof(com) );

    Serial.print("[Direction] "); Serial.println(recived.direction);
    Serial.print("[Hand] "); Serial.println(recived.hand);
    Serial.print("[Speed] "); Serial.println(recived.speed);
    Serial.print("[Zachvat] "); Serial.println(recived.zachvat); Serial.println();

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

    case STOP: {
      direct = stop;
      break;
    }

    default:
      break;
    }

    switch (recived.hand) {
      case UP_HAND: {
        hand_dir = DirectHand :: up;
        break;
      }

      case DOWN_HAND: {
        hand_dir = DirectHand :: dw;
        break;
      }

      case STOP_HAND: {
        hand_dir = DirectHand :: stopHand;
      }

      default:
        break;
    }

    switch (recived.zachvat) {
      case UP_ZACH: {
        zachvat_dir = DirectHand :: up;
        break;
      }

      case DOWN_ZACH: {
        zachvat_dir = DirectHand :: dw;
        break;
      }

      case STOP_ZACH: {
        zachvat_dir = DirectHand :: stopHand;
      }

      default:
        break;
    }
  }

  digitalWrite(LATCH_PIN, LOW);  // цифра один

  GoHand(hand_dir);
  Motor(direct, recived.speed);

  digitalWrite(LATCH_PIN, HIGH);

  ZachVat(zachvat_dir);

} 