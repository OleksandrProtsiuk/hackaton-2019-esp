#include <Arduino.h>
#include <Servo.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h> //
#include <ESP8266HTTPClient.h> //
#include <ArduinoJson.h>


#define GAS_LEAK 13
#define TEMP_BUS 2
#define GAS_COUNTER_LED 0
#define RESET_ALARM 12

void sendData();

Servo myservo;
OneWire oneWire(TEMP_BUS);
DallasTemperature sensor(&oneWire);

// const char *host = "http://e55c533b.ngrok.io/"; //Web/Server address to read/write from
const int threshold= 10;
int resetAlarm = 0;
int volume;
int status = 0;
int i = 0;

void setup() {
  Serial.begin(9600);
  pinMode(GAS_COUNTER_LED, OUTPUT);
  pinMode(GAS_LEAK, INPUT);
  pinMode(RESET_ALARM, INPUT);

  myservo.attach(5);
  myservo.write(90);
  sensor.begin();
  // put your setup code here, to run once:

  WiFi.begin("MIX Lite", "12345678");

    Serial.print("Connecting");  //  "Подключение"
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
    Serial.println();

    Serial.print("Connected, IP address: ");
             //  "Подключились, IP-адрес: "
    Serial.println(WiFi.localIP());
}



float getRoomTemperature() {
  sensor.requestTemperatures();
  float t = sensor.getTempCByIndex(0);
  return t;
}

void sendData() {
      StaticJsonBuffer<300> JSONbuffer;   //Declaring static JSON buffer
      JsonObject& JSONencoder = JSONbuffer.createObject();

      volume = i;

      JSONencoder["pincode"] = "123456";
      JSONencoder["volume"] = volume;
      JSONencoder["temperatura"] = getRoomTemperature();
      JSONencoder["status"] = status;


      char JSONmessageBuffer[300];
      JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
      Serial.println(JSONmessageBuffer);

      HTTPClient http;    //Declare object of class HTTPClient

      http.begin("http://768c618d.ngrok.io/devises");      //Specify request destination
      http.addHeader("Content-Type", "application/json");  //Specify content-type header

      int httpCode = http.POST(JSONmessageBuffer);   //Send the request
      String payload = http.getString();                                        //Get the response payload

      Serial.println(httpCode);   //Print HTTP return code
      Serial.println(payload);    //Print request response payload

      volume = 0;

      http.end();  //Close connection
}


void loop() {

  if (i == 5) {
    sendData();
    i = 0;
  }
//---------------------------------------------//
  boolean gasLeak = digitalRead(GAS_LEAK);
  int val= analogRead(0);

  if (gasLeak) {
    myservo.write(0);
    status = 0;
  }
  else {
    sendData();
    do {
      myservo.write(90);
      resetAlarm = digitalRead(RESET_ALARM);
      Serial.println("УТЕЧКА!");
      Serial.println(resetAlarm);
      getRoomTemperature();
      yield();
      delay(3000);
      status = 1;
    } while (resetAlarm == 0);
  }

  if (val >= threshold) {
      Serial.println(val);
      digitalWrite(GAS_COUNTER_LED, HIGH);
      delay(100);
      digitalWrite(GAS_COUNTER_LED, LOW);
      i += 1;
    }
  else {
    digitalWrite(GAS_COUNTER_LED, LOW);
  }
  // put your main code here, to run repeatedly:



  // Serial.printf("Connection status: %d\n", WiFi.status());
}
