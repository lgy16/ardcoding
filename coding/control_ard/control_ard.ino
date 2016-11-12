//제어아두이노 코드 수정중1
#include <mthread.h>
#include <ArduinoJson.h>

#include <WiFi.h>
#include <SPI.h>
#define COUNT_MAX 100
#define PACKET_SIZE 20
void Attatch_Arduino();
void Attatch_Devices();
void On_Off_Data(String, String);

char ssid[] = "smartfarm"; //와이파이 이름
char pass[] = "smartfarm1234"; //와이파이 비밀번호
char server[] = "165.229.89.100";  //서버 IP주소
int portnum = 80; 
int status = WL_IDLE_STATUS;
WiFiClient client;

class DeviceThread : public Thread
{
  public:
    int win=0;   //0:off, 1:on
    int coo=0;
    int heat=0;
    
  protected:
    bool loop();
};

bool DeviceThread::loop()
{
  // Die if requested:
  if (kill_flag)
    return false;

 
  //디바이스 동작코드 추가해야함
  
  // Sleep for one second:
  // sleep(1);
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
    
  char json[] = "{ \"url\" : \"/duration\", \"method\" : \"PUT\", \"body\" : { \"sensor\" : \"humid\", \"second\" : 256 } }";

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject & root = jsonBuffer.parseObject(json);

  const char* url = root ["url"];
  const char* method = root ["method"];
  
  JsonObject & body = root ["body"];
  const char* id = body ["id"];
  const char* sensor = body ["sensors"];
  const int value = body["value"];
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

    Post_Sensor_Data();
  }

  //3
  if(!strcmp(url, "/onoff") && !strcmp(method, "GET"))
  {
    //Request On/Off data
    //Serial.println("Request On/Off data");

    On_Off_Data();
  }

  //4
  if(!strcmp(url, "/onoff") && !strcmp(method, "PUT"))
  {
    //Set On/Off data
    //Serial.println("Set On/Off data");

    const char* state = body["state"];
    //Serial.print("\nstate = ");
    //Serial.println(state);

    On_Off_Data();
  }

  //5
  if(!strcmp(url, "/range") && !strcmp(method, "GET"))
  {
    //Request Range data
    //Serial.println("Request Range data");

    Range_Data();
  }

  //6
  if(!strcmp(url, "/range") && !strcmp(method, "PUT"))
  {
    //Set Range data
    //Serial.println("Set Range data");

    const int state = body["state"];
    //Serial.print("\nstate = ");
    //Serial.println(state);

    Range_Data();
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
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // WPA/WAP2 네트워크에 연결
    status = WiFi.begin(ssid, pass);
    delay(5000);
  }
  
  Serial.print("connected ");

  while (!client.connect(server, portnum)) { //서버 연결 시도
    Serial.println("connecting to Server...");
    
    client.println();
    delay(10000);
  }


  Attatch_Arduino();
  Serial.println();
  Attatch_Devices();
  Serial.println();

  main_thread_list->add_thread(new DeviceThread());  //쓰레드 추가
  main_thread_list->add_thread(new JsonThread());

}

void onoff_Set() {

}

void Attatch_Arduino() {
  StaticJsonBuffer<200> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();
  root["url"] = "/arduino";
  root["method"] = "PUT";
  JsonObject& body = root.createNestedObject("body");
  body["id"] = "84:38:35:6f:03:50";
  body["name"] = "arduino1";
  root.printTo(Serial);
}
void Attatch_Devices() {
  StaticJsonBuffer<200> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();
  root["url"] = "/devices";
  root["method"] = "PUT";
  JsonObject& body = root.createNestedObject("body");
  body["id"] = "84:38:35:6f:03:50";
  JsonArray& devices = body.createNestedArray("devices");
  devices.add("window");
  devices.add("cool");
  devices.add("heat");
  root.printTo(Serial);
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
  root.printTo(Serial);
}
