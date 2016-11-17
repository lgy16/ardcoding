//센서아두이노 코드 수정중
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
void On_Off_Data(String, String);
void Range_Data(String, String);
void Autocheck_Sensor(int);

char ssid[] = "smartfarm"; //와이파이 이름
char pass[] = "smartfarm1234"; //와이파이 비밀번호
char server[] = "165.229.89.100";  //서버 IP주소
int portnum = 80; 
int status = WL_IDLE_STATUS;
WiFiClient client;
int sleeptime;

const int cdsPin = A2, ledPin = 9;
const int SoilSensorPin = A0;
const int GasSensorPin = A1;
int cdsValue;
float temp, humi, soil, gas;

class SensorThread : public Thread
{
  protected:
    bool loop();
};

bool SensorThread::loop()
{
  // Die if requested:
  if (kill_flag)
    return false;

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
  // Die if requested:
  if (kill_flag)
    return false;

  //json 받는 부분 코드 아직
  char json[] = "{ \"url\" : \"/duration\", \"method\" : \"PUT\", \"body\" : { \"sensor\" : \"humid\", \"second\" : 256 } }";

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject & root = jsonBuffer.parseObject(json);

  const char* url = root ["url"];
  const char* method = root ["method"];
  
  JsonObject & body = root ["body"];
  const char* id = body ["id"];
  const char* sensor = body ["sensor"];
  const double value = body["value"];
  const char* device = body["device"];
  const int second = body["second"];

  //1
  if(!strcmp(url, "/devices") && !strcmp(method, "GET"))
  {
    //Request Device list
    //Serial.println("Request Device list");

    Attatch_Devices();
  }

  //2
  if(!strcmp(url, "/sensors") && !strcmp(method, "POST"))
  {
    //Request Sensor data
    //Serial.println("Request Sensor data");

    double sensor_value;

    if(!strcmp(sensor, "light"))
    {
      sensor_value = analogRead(cdsPin);
    }
    else if(!strcmp(sensor, "temp"))
    {
      sensor_value = dht.readTemperature();
    }
    else if(!strcmp(sensor, "humid"))
    {
      sensor_value = dht.readHumidity();
    }
    else if(!strcmp(sensor, "soil"))
    {
      sensor_value = analogRead(SoilSensorPin);
    }
    else if(!strcmp(sensor, "gas"))
    {
      sensor_value = analogRead(GasSensorPin);
    }

    Post_Sensor_Data(sensor, sensor_value);
  }

  //3
  if(!strcmp(url, "/onoff") && !strcmp(method, "GET"))
  {
    //Request On/Off data
    //Serial.println("Request On/Off data");

    //On_Off_Data();  // -> 처리할 장치는 water pump 뿐,, 아직 코드가 없
  }

  //4
  if(!strcmp(url, "/onoff") && !strcmp(method, "PUT"))
  {
    //Set On/Off data
    //Serial.println("Set On/Off data");

    const char* state = body["state"];
    //Serial.print("\nstate = ");
    //Serial.println(state);

    ///////////////////////////////여기 set on/off하는 부분이 들어가야함 , 그후 결과를 On_Off_Date() 로 보냄
    //On_Off_Data(); -> 처리할 장치는 water pump 뿐,, 아직 코드가 없
  }

  //5
  if(!strcmp(url, "/range") && !strcmp(method, "GET"))
  {
    //Request Range data
    //Serial.println("Request Range data");

    //Range_Data(); -> led
  }

  //6
  if(!strcmp(url, "/range") && !strcmp(method, "PUT"))
  {
    //Set Range data
    //Serial.println("Set Range data");

    const int state = body["state"];
    //Serial.print("\nstate = ");
    //Serial.println(state);

    //range설정하는부분 후 응답 Range_Data()
    //Range_Data();
  }

  //7
  if(!strcmp(url, "/duration") && !strcmp(method, "GET"))
  {
    //Request Sensor data autocheck duration
    //Serial.println("Request Sensor data autocheck duration");

    Autocheck_Sensor(sleeptime);
  }    

  //8
  if(!strcmp(url, "/duration") && !strcmp(method, "PUT"))
  {
    //Set Sensor data autocheck duration
    //Serial.println("Set Sensor data autocheck duration");

    sleeptime = second;
    Autocheck_Sensor(sleeptime);
  }  
  
  // Sleep for one second:
  // sleep(1);
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
    // WPA/WAP2 네트워크에 연결
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

  sleeptime=60;
    
  pinMode(ledPin, OUTPUT);
  dht.begin();

  Attatch_Arduino();
  Serial.println();
  Attatch_Devices();
  Serial.println();

  main_thread_list->add_thread(new SensorThread());  //쓰레드 추가
  main_thread_list->add_thread(new JsonThread());

}

void onoff_Set() {

}
void Range_Set() {

}

void Attatch_Arduino() {
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

void Attatch_Devices() {
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
  devices.add("les");
  devices.add("pump");

  String buffer;
  root.printTo(buffer);
  client.print(buffer.length());
  root.printTo(client);
}

void Sensor_Data(String sen, double value) {
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

void Post_Sensor_Data(String sen, double value){
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

void  On_Off_Data(String device, String state) {
  //led, fan, pump
  StaticJsonBuffer<200> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();
  root["url"] = "/onoff";
  root["method"] = "Post";
  JsonObject& body = root.createNestedObject("body");
  body["id"] = "84:38:35:6f:03:50";
  body["device"]=device;
  body["state"]=state;
  
  String buffer;
  root.printTo(buffer);
  client.print(buffer.length());
  root.printTo(client);
}

void Range_Data(String device,String state)
{
  StaticJsonBuffer<200> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();
  root["url"] = "/range";
  root["method"] = "POST";
  JsonObject& body = root.createNestedObject("body");
  body["id"] = "84:38:35:6f:03:50";
  body["device"] = device;
  body["state"] = state;
  
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
