#include <Arduino.h>
#include <Servo.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define GAS_LEAK 13
#define TEMP_BUS 2
#define GAS_COUNTER_LED 0
#define RESET_ALARM 12

Servo myservo;
OneWire oneWire(TEMP_BUS);
DallasTemperature sensor(&oneWire);

const int threshold= 10;
int resetAlarm = 0;

void setup() {
  Serial.begin(9600);
  pinMode(GAS_COUNTER_LED, OUTPUT);
  pinMode(GAS_LEAK, INPUT);
  pinMode(RESET_ALARM, INPUT);

  myservo.attach(5);
  myservo.write(90);
  sensor.begin();
  // put your setup code here, to run once:
}

void getRoomTemperature() {
  sensor.requestTemperatures();
  float t = sensor.getTempCByIndex(0);
  Serial.println(t);
}

void loop() {

  boolean gasLeak = digitalRead(GAS_LEAK);
  int val= analogRead(0);

  if (gasLeak) {
    myservo.write(0);
  }
  else {
    do {
      myservo.write(90);
      resetAlarm = digitalRead(RESET_ALARM);
      Serial.println("УТЕЧКА!");
      Serial.println(resetAlarm);
      getRoomTemperature();
      yield();
      delay(3000);
    } while (resetAlarm == 0);

  }

  if (val >= threshold) {
      Serial.println(val);
      digitalWrite(GAS_COUNTER_LED, HIGH);
      delay(100);
      digitalWrite(GAS_COUNTER_LED, LOW);
    }
  else {
    digitalWrite(GAS_COUNTER_LED, LOW);
  }
  // put your main code here, to run repeatedly:
}
