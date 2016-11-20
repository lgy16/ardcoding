#include <ArduinoJson.h>
#include <DHT.h>
#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
#include <WiFi.h>
#include <SPI.h>

void Attatch_Arduino();
void Attatch_Devices();
void Sensor_Data(String, double);
void Post_Sensor_Data(String, double);
void On_Off_Data(String);
void Range_Data(String);
void Autocheck_Sensor(int);

char ssid[] = "IIR_LAB"; //와이파이 이름
char pass[] = "robot2015!"; //와이파이 비밀번호
char server[] = "165.229.89.100";  //서버 IP주소
int portnum = 443;
int status = WL_IDLE_STATUS;
WiFiClient client;
int sleeptime;

String waterpump;
int led;
int waterpumpPin = 3;
const int cdsPin = A2, ledPin = 9;
const int SoilSensorPin = A0;
const int GasSensorPin = A1;
int count = 0;

/*
class ByteCounter : public Print
{
  public:
    ByteCounter()
      : len(0)
    {
    }

    virtual size_t write(uint8_t c)
    {
      len++;
    }

    int length() const
    {
      return len;
    }

  private:
    int len;
};
*/

void setup()
{
  Serial.begin(115200);

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
    delay(5000);
  }

  sleeptime = 20;

  pinMode(waterpumpPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  dht.begin();

  Attatch_Arduino();
  Serial.println();
  Attatch_Devices();
  Serial.println();

  led = 0;
  waterpump = "OFF";

  analogWrite(ledPin, led * 50); //led 기본값 out
  digitalWrite(waterpumpPin, 0);  //waterpump 기본값 out

}


void loop()
{
  Serial.println("loop");
  char json[256] {0};
  client.read((uint8_t*)json, 256);

  Serial.println(json);

  //char json[] = "{ \"url\" : \"/duration\", \"method\" : \"PUT\", \"body\" : { \"sensor\" : \"humid\", \"second\" : 256 } }";

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
    count = 0;
    Autocheck_Sensor(sleeptime);
  }



  ////////////////////////sensing///////////////////////
  if (count == 0 || count == sleeptime)
  {

    Serial.println("sensing");
    Sensor_Data("light", analogRead(cdsPin));   //조도
    Sensor_Data("temp", dht.readTemperature());  //온도
    Sensor_Data("humid", dht.readHumidity());  //습도
    Sensor_Data("soil", analogRead(SoilSensorPin));   //토양수분
    Sensor_Data("gas", analogRead(GasSensorPin));   //가스

    count = count % sleeptime;
  }

  delay(1000);
  count++;
}




//////////////////////////json 함수들
void Attatch_Arduino()
{
  Serial.println("attctch arduino");
  StaticJsonBuffer<200> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();
  root["url"] = "/arduino";
  root["method"] = "PUT";
  JsonObject& body = root.createNestedObject("body");
  body["id"] = "84:38:35:6f:03:50";
  body["name"] = "arduino1";

  //ByteCounter counter;
  //root.printTo(counter);

  Serial.print(root.measureLength());  //숫자찍기
  Serial.print("\t");
  client.print(root.measureLength()); //서버로 숫자찍기
  root.printTo(Serial);  //시리얼에 내용찍기
  Serial.println("");
  root.printTo(client); //서버로 전송

}

void Attatch_Devices()
{

  Serial.println("attatch devices");
  StaticJsonBuffer<200> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();
  root["url"] = "/devices";
  root["method"] = "PUT";
  JsonObject& body = root.createNestedObject("body");
  body["id"] = "84:38:35:6f:03:50";
  JsonArray& devices = body.createNestedArray("devices");
  devices.add("temp");
  devices.add("humid");
  devices.add("soil");
  devices.add("light");
  devices.add("gas");
  devices.add("led");
  devices.add("pump");

/*
  ByteCounter counter;
  root.printTo(counter);

  Serial.println(counter.length());  //숫자찍기
  client.print(counter.length());
  root.printTo(Serial);  //내용찍기
  root.printTo(client);
  */
  Serial.print(root.measureLength());  //숫자찍기
  Serial.print("\t");
  client.print(root.measureLength()); //서버로 숫자찍기
  root.printTo(Serial);  //시리얼에 내용찍기
  Serial.println("");
  root.printTo(client); //서버로 전송
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

  Serial.print(root.measureLength());  //숫자찍기
  Serial.print("\t");
  client.print(root.measureLength()); //서버로 숫자찍기
  root.printTo(Serial);  //시리얼에 내용찍기
  Serial.println("");
  root.printTo(client); //서버로 전송
}

void Post_Sensor_Data(String sen, double value)
{
  Serial.println("post sensor data");
  StaticJsonBuffer<200> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();
  root["url"] = "/sensors";
  root["method"] = "POST";
  JsonObject& body = root.createNestedObject("body");
  body["id"] = "84:38:35:6f:03:50";
  body["sensor"] = sen;
  body["value"] = value;

  Serial.print(root.measureLength());  //숫자찍기
  Serial.print("\t");
  client.print(root.measureLength()); //서버로 숫자찍기
  root.printTo(Serial);  //시리얼에 내용찍기
  Serial.println("");
  root.printTo(client); //서버로 전송

}

void  On_Off_Data(String device)
{
  Serial.println("on off data");
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
  
  Serial.print(root.measureLength());  //숫자찍기
  Serial.print("\t");
  client.print(root.measureLength()); //서버로 숫자찍기
  root.printTo(Serial);  //시리얼에 내용찍기
  Serial.println("");
  root.printTo(client); //서버로 전송
}

void Range_Data(String device)  // led
{
  Serial.println("range data");
  StaticJsonBuffer<200> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();
  root["url"] = "/range";
  root["method"] = "POST";
  JsonObject& body = root.createNestedObject("body");
  body["id"] = "84:38:35:6f:03:50";
  body["device"] = device;
  body["state"] = led;
  
  Serial.print(root.measureLength());  //숫자찍기
  Serial.print("\t");
  client.print(root.measureLength()); //서버로 숫자찍기
  root.printTo(Serial);  //시리얼에 내용찍기
  Serial.println("");
  root.printTo(client); //서버로 전송

}

void Autocheck_Sensor(int duration)
{
  Serial.println("autocheck sensor");
  StaticJsonBuffer<200> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();
  root["url"] = "/duration";
  root["method"] = "POST";
  JsonObject& body = root.createNestedObject("body");
  body["id"] = "84:38:35:6f:03:50";
  body["second"] = duration;

  Serial.print(root.measureLength());  //숫자찍기
  Serial.print("\t");
  client.print(root.measureLength()); //서버로 숫자찍기
  root.printTo(Serial);  //시리얼에 내용찍기
  Serial.println("");
  root.printTo(client); //서버로 전송
}

