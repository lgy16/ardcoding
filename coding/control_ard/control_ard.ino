//제어아두이노 코드 수정중4

#include <ArduinoJson.h>
#include <Servo.h>
#include <WiFi.h>
#include <SPI.h>
using namespace std;
Servo Servo1;
int coolpin = 7;
int heatpin = 4;

void Attach_Arduino();
void Attach_Devices();
void On_Off_Data(String);
void onoff_Set(String, String);
int Json_decoding();
void Send_json();

char ssid[] = "IIR_LAB"; //와이파이 이름
char pass[] = "robot2015!"; //와이파이 비밀번호
char server[] = "165.229.89.100";  //서버 IP주소
int portnum = 443;
int status = WL_IDLE_STATUS;
WiFiClient client;

String window = "OFF";
String heat = "OFF";
String cool = "OFF";

const char attach_ard[] PROGMEM = "{\"url\":\"/arduino\",\"method\":\"PUT\",\"body\":{\"id\":\"84:38:35:6f:03:33\",\"name\":\"arduino3\"}}";
const char attach_dev[] PROGMEM = "{\"url\":\"/devices\",\"method\":\"PUT\",\"body\":{\"id\":\"84:38:35:6f:03:33\",\"devices\":[\"window\",\"cool\",\"heat\"]}}";
const char on_off_data_1[] PROGMEM = "{\"url\":\"/onoff\",\"method\":\"POST\",\"body\":{\"id\":\"84:38:35:6f:03:33\",\"device\":\"";
const char on_off_data_2[] PROGMEM = "\",\"state\":\"";
const char on_off_data_3[] PROGMEM = "\"}}";
String temp_device;

char buffer[200];


void setup() {

  Serial.begin(115200);
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
  Attach_Arduino();
  Serial.println();

  delay(3000);
  
  Attach_Devices();
  Serial.println();

  Servo1.attach(5);
  Servo1.write(0);
  pinMode(coolpin, OUTPUT);
  pinMode(heatpin, OUTPUT);
}

void loop() {
  Serial.println("loop");

  //Request Device list :: 1
  //Request On/Off data :: 3
  //Set On/Off data :: 4
  
  int what_json = Json_decoding();

    switch(what_json)
  {
    case 1 : Attach_Devices(); break;
    case 3 : On_Off_Data(temp_device); break;
    case 4 : On_Off_Data(temp_device); break;
    default : break;
  }  
}

void Send_json(String json_str)
{
  String str1 = json_str.substring(0, 90);
  String str2 = json_str.substring(90);
  int str_length = json_str.length();
  client.print(str_length);
  Serial.print(json_str + "\t");
  Serial.print(str_length);
  Serial.print("\t");
  Serial.print(client.print(str1));
  Serial.print("\t");
  Serial.println(client.print(str2));
}

void onoff_Set(String device, String state) {

  int onoff = 0;

  if (state.equals("ON")) {
    onoff = 1;
  }
  else if (state.equals("OFF")) {
    onoff = 0;
  }

  if (device.equals("window")) {
    window = state;
    Servo1.write(75 * onoff);
  }
  else if (device.equals("cool")) {
    digitalWrite(coolpin, onoff);
    cool = state;
  }
  else if (device.equals("heat")) {
    digitalWrite(heatpin, onoff);
    heat = state;
  }
}

void Attach_Arduino() {
  Serial.println("Attach_Arduino");

  strcpy_P(buffer, (char*)attach_ard);
  String json_str = String(buffer);
  Send_json(json_str);
}

void Attach_Devices() {
  Serial.println("Attach_Devices");

  strcpy_P(buffer, (char*)attach_dev);
  String json_str = String(buffer);
  Send_json(json_str);
}

void On_Off_Data(String device) {
  Serial.println("On Off Data");
  String state;
  if (device.equals("window")) {
    state = window;
  }
  else if (device.equals("cool")) {
    state = cool;
  }
  else if (device.equals("heat")) {
    state = heat;
  }

  strcpy_P(buffer, (char*)on_off_data_1);
  String json_str = String(buffer) + device;
  strcpy_P(buffer, (char*)on_off_data_2);
  json_str += (buffer + state);
  strcpy_P(buffer, (char*)on_off_data_3);
  json_str += buffer;
  Send_json(json_str);
}

int Json_decoding()
{
  char json[256] {0};
  client.read((uint8_t*)json, 256);

  Serial.println(json);

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject & root = jsonBuffer.parseObject(json);

  const String url = root ["url"];
  const String method = root ["method"];
  JsonObject & body = root ["body"];
  const String id = body ["id"];
  const String sensor = body ["sensors"];
  const int value = body["value"];
  const String device = body["device"];
  const int second = body["second"];
  const String state = body["state"];

  if (!url.compareTo("/devices") && !method.compareTo("GET")) //Request Device list :: 1
  {
    Serial.println("Request Device list");

    return 1;
    //Attach_Devices();
  }

  if (!url.compareTo("/onoff") && !method.compareTo("GET")) //Request On/Off data :: 3
  {
    Serial.println("Request On/Off data");
    temp_device = device;

    return 3;
    //On_Off_Data(device);
  }

  if (!url.compareTo("/onoff") && !method.compareTo("PUT")) //Set On/Off data :: 4
  {
    Serial.println("Set On/Off data");
    //Serial.print("\nstate = ");
    //Serial.println(state);

    onoff_Set(device, state);
    temp_device = device;
    
    return 4;
    
    //On_Off_Data(device);
  }
  return 0;
}
