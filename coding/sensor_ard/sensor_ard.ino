//센서아두이노 코드 수정중4

#include <mthread.h>
#include <ArduinoJson.h>
#include <DHT.h>
#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
#include <WiFi.h>
#include <SPI.h>
#define COUNT_MAX 100
#define PACKET_SIZE 20
void Attatch_Arduino();
void Attatch_Devices();
void Sensor_Data(String, double);
void Post_Sensor_Data(String, double);
void On_Off_Data(String);
void Range_Data(String);
void Autocheck_Sensor(int);

char ssid[] = "smartfarm"; //와이파이 이름
char pass[] = "smartfarm1234"; //와이파이 비밀번호
char server[] = "165.229.89.100";  //서버 IP주소
int portnum = 80;
int status = WL_IDLE_STATUS;
WiFiClient client;
int sleeptime;

String waterpump;
int led;
int waterpumpPin = 3;
const int cdsPin = A2, ledPin = 9;
const int SoilSensorPin = A0;
const int GasSensorPin = A1;

class SensorThread : public Thread
{
  protected:
    bool loop();
};

bool SensorThread::loop()
{
  Sensor_Data("light", analogRead(cdsPin));   //조도
  Sensor_Data("temp", dht.readTemperature());  //온도
  Sensor_Data("humi", dht.readHumidity());  //습도
  Sensor_Data("soil", analogRead(SoilSensorPin));   //토양수분
  Sensor_Data("gas", analogRead(GasSensorPin));   //가스

  //Sleep for sleeptime
  sleep(sleeptime);
  return true;
}

class JsonThread : public Thread
{
  protected:
    bool loop();
};

bool JsonThread::loop()
{
  //json 받는 부분 코드 아직
  int how_long=0; //json 길이
  char json[] = "{ \"url\" : \"/duration\", \"method\" : \"PUT\", \"body\" : { \"sensor\" : \"humid\", \"second\" : 256 } }";
  


  StaticJsonBuffer<200> jsonBuffer;
  JsonObject & root = jsonBuffer.parseObject(json);

  const String url = root ["url"];
  const String method = root ["method"];

  JsonObject & body = root ["body"];
  const String id = body ["id"];
  const String sensor = body ["sensor"];
  const double value = body["value"];
  const String device = body["device"];
  const int second = body["second"];

  //Request Device list
  if (!url.compareTo("/devices") && !method.compareTo("GET")) 
  {
    Serial.println("Request Device list");

    Attatch_Devices();
  }

  //Request Sensor data
  if (!url.compareTo("/sensors") && !method.compareTo("POST"))
  {
    Serial.println("Request Sensor data");

    double sensor_value;

    if (!sensor.compareTo("light"))
    {
      sensor_value = analogRead(cdsPin);
    }
    else if (!sensor.compareTo("temp"))
    {
      sensor_value = dht.readTemperature();
    }
    else if (!sensor.compareTo("humid"))
    {
      sensor_value = dht.readHumidity();
    }
    else if (!sensor.compareTo("soil"))
    {
      sensor_value = analogRead(SoilSensorPin);
    }
    else if (!sensor.compareTo("gas"))
    {
      sensor_value = analogRead(GasSensorPin);
    }

    Post_Sensor_Data(sensor, sensor_value);
  }

  //Request On/Off data
  if (!url.compareTo("/onoff") && !method.compareTo("GET"))
  {
    Serial.println("Request On/Off data");

    On_Off_Data(device);
  }

  //Set On/Off data
  if (!url.compareTo("/onoff") && !method.compareTo("PUT"))
  {
    const String state = body["state"];

    waterpump = state;
    
    Serial.println("Set On/Off data");
    int onoff = 0;
    if (state.equals("ON"))
    {
      onoff = 1;
    }
    else if (state.equals("OFF")) 
    {
      onoff = 0;
    }
    digitalWrite(waterpumpPin, onoff);

    On_Off_Data(device);
  }

  //Request Range data
  if (!url.compareTo("/range") && !method.compareTo("GET"))
  {
    Serial.println("Request Range data");

    Range_Data(device);
  }

  //Set Range data
  if (!url.compareTo("/range") && !method.compareTo("PUT"))
  {
    Serial.println("Set Range data");
    const int state = body["state"];
    led = state;
    /*
    if (state == 0) {
      analogWrite(ledPin, 0);
    }
    else if (state == 1) {
      analogWrite(ledPin, 50);
    }
    else if (state == 2) {
      analogWrite(ledPin, 100);
    }
    else if (state == 3) {
      analogWrite(ledPin, 150);
    }
    else if (state == 4) {
      analogWrite(ledPin, 200);
    }
    else if (state == 5) {
      analogWrite(ledPin, 250);
    }
    */
    analogWrite(ledPin, (led * 50));
    Range_Data(device);
  }

  //Request Sensor data autocheck duration
  if (!url.compareTo("/duration") && !method.compareTo("GET")) 
  {
    Serial.println("Request Sensor data autocheck duration");

    Autocheck_Sensor(sleeptime);
  }

  //Set Sensor data autocheck duration
  if (!url.compareTo("/duration") && !method.compareTo("PUT"))
  {
    Serial.println("Set Sensor data autocheck duration");

    sleeptime = second;
    Autocheck_Sensor(sleeptime);
  }

  return true;
}

void setup() {
  Serial.begin(9600);

  // 현재 아두이노에 연결된 실드를 확인
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // 와이파이 실드가 아닐 경우
    while (true);
  }
  // 와이파이 연결 시도
  while (status != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    //WPA/WAP2 네트워크에 연결
    status = WiFi.begin(ssid, pass);
    delay(5000);
  }

  Serial.print("connected ");

  while (!client.connect(server, portnum))
  {
    //서버 연결 시도
    Serial.println("connecting to Server...");

    client.println();
    delay(10000);
  }

  sleeptime = 60;

  pinMode(waterpumpPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  dht.begin();

  Attatch_Arduino();
  Serial.println();
  Attatch_Devices();
  Serial.println();

  led = 0;
  waterpump = "OFF";
  
  analogWrite(ledPin, led*50); //led 기본값 out
  digitalWrite(waterpumpPin, 0);  //waterpump 기본값 out

  main_thread_list->add_thread(new SensorThread());  //쓰레드 추가
  main_thread_list->add_thread(new JsonThread());

}

void Attatch_Arduino()
{
  StaticJsonBuffer<200> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();
  root["url"] = "/arduino";
  root["method"] = "PUT";
  JsonObject& body = root.createNestedObject("body");
  body["id"] = "84:38:35:6f:03:50";
  body["name"] = "arduino1";

  String buffer;
  root.printTo(buffer);
  client.print(buffer.length());
  root.printTo(client);
}

void Attatch_Devices()
{
  StaticJsonBuffer<200> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();
  root["url"] = "/devices";
  root["method"] = "PUT";
  JsonObject& body = root.createNestedObject("body");
  body["id"] = "84:38:35:6f:03:50";
  JsonArray& devices = body.createNestedArray("devices");
  devices.add("humid");
  devices.add("soil");
  devices.add("light");
  devices.add("gas");
  devices.add("led");
  devices.add("pump");

  String buffer;
  root.printTo(buffer);
  client.print(buffer.length());
  root.printTo(client);
}

void Sensor_Data(String sen, double value) 
{
  //조도, 토양수분, 온습도, 가스
  StaticJsonBuffer<200> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();
  root["url"] = "/sensors";
  root["method"] = "PUT";
  JsonObject& body = root.createNestedObject("body");
  body["id"] = "84:38:35:6f:03:50";
  body["sensor"] = sen;
  body["value"] = value;

  String buffer;
  root.printTo(buffer);
  client.print(buffer.length());
  root.printTo(client);
}

void Post_Sensor_Data(String sen, double value) 
{
  StaticJsonBuffer<200> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();
  root["url"] = "/sensors";
  root["method"] = "POST";
  JsonObject& body = root.createNestedObject("body");
  body["id"] = "84:38:35:6f:03:50";
  body["sensor"] = sen;
  body["value"] = value;

  String buffer;
  root.printTo(buffer);
  client.print(buffer.length());
  root.printTo(client);
}

void  On_Off_Data(String device) 
{  
  //waterpump
  //fan, pump
  StaticJsonBuffer<200> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();
  root["url"] = "/onoff";
  root["method"] = "Post";
  JsonObject& body = root.createNestedObject("body");
  body["id"] = "84:38:35:6f:03:50";
  body["device"] = device;
  body["state"] = waterpump;

  String buffer;
  root.printTo(buffer);
  client.print(buffer.length());
  root.printTo(client);
}

void Range_Data(String device)  // led
{
  StaticJsonBuffer<200> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();
  root["url"] = "/range";
  root["method"] = "POST";
  JsonObject& body = root.createNestedObject("body");
  body["id"] = "84:38:35:6f:03:50";
  body["device"] = device;
  body["state"] = led;

  String buffer;
  root.printTo(buffer);
  client.print(buffer.length());
  root.printTo(client);
}

void Autocheck_Sensor(int duration)
{
  StaticJsonBuffer<200> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();
  root["url"] = "/duration";
  root["method"] = "POST";
  JsonObject& body = root.createNestedObject("body");
  body["id"] = "84:38:35:6f:03:50";
  body["second"] = duration;

  String buffer;
  root.printTo(buffer);
  client.print(buffer.length());
  root.printTo(client);
}
