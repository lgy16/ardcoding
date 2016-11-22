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
void Json_decoding();

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

  Attatch_Arduino();  //attach_arduino
  Serial.println();

  delay(3000);
  
  Attatch_Devices();  //attach_devices
  Serial.println();

  led = 0;
  waterpump = "OFF";

  analogWrite(ledPin, led * 50); //led 기본값 out
  digitalWrite(waterpumpPin, 0);  //waterpump 기본값 out

}


void loop()
{
  Serial.println("loop");
 
  ////////////////////////sensing///////////////////////
  if (count == 0 || count == sleeptime)
  {
    Serial.println("sensing");
   
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
  Serial.println("Attach_Arduino");
  
  String json_str = "{\"url\":\"/arduino\",\"method\":\"PUT\",\"body\":{\"id\":\"84:38:35:6f:03:51\",\"name\":\"arduino2\"}}";
  String str1 = json_str.substring(0, 90);
  String str2 = json_str.substring(90);
  int str_length = json_str.length();
  client.print(str_length);
  Serial.print(json_str + "\t");
  Serial.print(str_length);
  Serial.print("\t");
  Serial.print(client.print(str1));
  Serial.print("\t");
  Serial.print(client.print(str2));
}

void Attatch_Devices()
{
 Serial.println("Attach_Devices");
  
  String json_str = "{\"url\":\"/devices\",\"method\":\"PUT\",\"body\":{\"id\":\"84:38:35:6f:03:51\",\"devices\":[\"light\",\"temp\",\"humid\",\"soil\",\"gas\",\"led\",\"pump\"]}}";
  String str1 = json_str.substring(0, 90);
  String str2 = json_str.substring(90);
  int str_length = json_str.length();
  client.print(str_length);
  Serial.print(json_str + "\t");
  Serial.print(str_length);
  Serial.print("\t");
  Serial.print(client.print(str1));
  Serial.print("\t");
  Serial.print(client.print(str2));
}

void Sensor_Data(String sen, double value)
{
  Serial.println("Sensor Data");
  
  String json_str = "{\"url\":\"/sensors\",\"method\":\"PUT\",\"body\":{\"id\":\"84:38:35:6f:03:51\",\"sensor\":\"" + sen + "\",\"value\":" + String(value) + "}}";
  String str1 = json_str.substring(0, 90);
  String str2 = json_str.substring(90);
  int str_length = json_str.length();
  client.print(str_length);
  Serial.print(json_str + "\t");
  Serial.print(str_length);
  Serial.print("\t");
  Serial.print(client.print(str1));
  Serial.print("\t");
  Serial.print(client.print(str2));
}

void Post_Sensor_Data(String sen, double value)
{
  Serial.println("Post Sensor Data");
  
  String json_str = "{\"url\":\"/sensors\",\"method\":\"POST\",\"body\":{\"id\":\"84:38:35:6f:03:51\",\"device\":\""+sen+"\",\"value\":"+String(value)+"}}";
  String str1 = json_str.substring(0, 90);
  String str2 = json_str.substring(90);
  int str_length = json_str.length();
  client.print(str_length);
  Serial.print(json_str + "\t");
  Serial.print(str_length);
  Serial.print("\t");
  Serial.print(client.print(str1));
  Serial.print("\t");
  Serial.print(client.print(str2));
}

void  On_Off_Data(String device)
{
  Serial.println("On Off Data");
  
  String json_str = "{\"url\":\"/onoff\",\"method\":\"POST\",\"body\":{\"id\":\"84:38:35:6f:03:51\",\"device\":"+device+"\",\"state\":\""+waterpump+"}}"; 
  String str1 = json_str.substring(0, 90);
  String str2 = json_str.substring(90);
  int str_length = json_str.length();
  client.print(str_length);
  Serial.print(json_str + "\t");
  Serial.print(str_length);
  Serial.print("\t");
  Serial.print(client.print(str1));
  Serial.print("\t");
  Serial.print(client.print(str2));
}

void Range_Data(String device)  // led
{
  Serial.println("Range Data");
  
  String json_str = "{\"url\":\"/range\",\"method\":\"POST\",\"body\":{\"id\":\"84:38:35:6f:03:51\",\"device\":\""+device+"\",\"state\":"+String(led)+"}}"; 
  String str1 = json_str.substring(0, 90);
  String str2 = json_str.substring(90);
  int str_length = json_str.length();
  client.print(str_length);
  Serial.print(json_str + "\t");
  Serial.print(str_length);
  Serial.print("\t");
  Serial.print(client.print(str1));
  Serial.print("\t");
  Serial.print(client.print(str2));
}

void Autocheck_Sensor(int duration)
{
  Serial.println("Autocheck Sensor");
  
  String json_str = "{\"url\":\"/duration\",\"method\":\"POST\",\"body\":{\"id\":\"84:38:35:6f:03:51\",\"second\":"+String(duration)+"}}";
  String str1 = json_str.substring(0, 90);
  String str2 = json_str.substring(90);
  int str_length = json_str.length();
  client.print(str_length);
  Serial.print(json_str + "\t");
  Serial.print(str_length);
  Serial.print("\t");
  Serial.print(client.print(str1));
  Serial.print("\t");
  Serial.print(client.print(str2));
}






////////Json Decoding
void Json_decoding()
{
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

}

