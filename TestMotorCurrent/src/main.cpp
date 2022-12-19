#include <Arduino.h>
#include <TroykaCurrent.h>
 
// создаём объект для работы с датчиком тока
// и передаём ему номер пина выходного сигнала
ACS712 sensorCurrent(A0);

int cur_current, last_current;

void setup() {
  // put your setup code here, to run once:
  cur_current = sensorCurrent.readCurrentDC();
  Serial.begin(9600);
  Serial.print("Current is ");
  Serial.print(cur_current);
  Serial.println(" mA");
}

void loop() {
  // put your main code here, to run repeatedly:
  cur_current = sensorCurrent.readCurrentDC();


  Serial.print("Current is ");
  Serial.print(cur_current);
  Serial.println(" mA");

  // if (cur_current != last_current) {
  //   Serial.print("Current is ");
  //   Serial.print(cur_current);
  //   Serial.println(" mA");

  //   last_current = cur_current;
  // }

}