#include <ArduinoJson.h>

void setup() {
  Serial.begin(9600);

  //1 Request Device list
  //char json[] = "{ \"url\" : \"/devices\", \"method\" : \"GET\" }";

  //2 Request Sensor data
  //char json[] = "{ \"url\" : \"/sensors\", \"method\" : \"POST\", \"body\" : { \"sensor\" : \"light\" } }";


  //3 Request On/Off data
  //char json[] = "{ \"url\" : \"/onoff\", \"method\" : \"GET\", \"body\" : { \"device\" : \"led\" } }";

  //4 Set On/Off data
  //char json[] = "{ \"url\" : \"/onoff\", \"method\" : \"PUT\", \"body\" : { \"device\" : \"led\", \"state\" : \"ON\" } }";


  //5 Request Range data
  //char json[] = "{ \"url\" : \"/range\", \"method\" : \"GET\", \"body\" : { \"device\" :  \"motor\" } }";

  //6 Set Range data
  //char json[] = "{ \"url\" : \"/range\", \"method\" : \"PUT\", \"body\" : { \"device\" : \"motor\", \"state\" : 256 } }";


  //7 Request Sensor data autocheck duration
  //char json[] = "{ \"url\" : \"/duration\", \"method\" : \"GET\", \"body\" : { \"sensor\" :  \"humid\" } }";

  //8 Set Sensor data autocheck duration
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
  //const int state = body["state"];  //state 가 두종류...
  const int second = body["second"];

  

  //1
  if(!strcmp(url, "/devices") && !strcmp(method, "GET"))
  {
    //Request Device list
    Serial.println("Request Device list");
  }

  //2
  if(!strcmp(url, "/sensors") && !strcmp(method, "POST"))
  {
    //Request Sensor data
    Serial.println("Request Sensor data");
  }

  //3
  if(!strcmp(url, "/onoff") && !strcmp(method, "GET"))
  {
    //Request On/Off data
    Serial.println("Request On/Off data");
  }

  //4
  if(!strcmp(url, "/onoff") && !strcmp(method, "PUT"))
  {
    //Set On/Off data
    Serial.println("Set On/Off data");

    const char* state = body["state"];
    Serial.print("\nstate = ");
    Serial.println(state);
  }

  //5
  if(!strcmp(url, "/range") && !strcmp(method, "GET"))
  {
    //Request Range data
    Serial.println("Request Range data");
  }

  //6
  if(!strcmp(url, "/range") && !strcmp(method, "PUT"))
  {
    //Set Range data
    Serial.println("Set Range data");

    const int state = body["state"];
    Serial.print("\nstate = ");
    Serial.println(state);
  }

  //7
  if(!strcmp(url, "/duration") && !strcmp(method, "GET"))
  {
    //Request Sensor data autocheck duration
    Serial.println("Request Sensor data autocheck duration");
  }    

  //8
  if(!strcmp(url, "/duration") && !strcmp(method, "PUT"))
  {
    //Set Sensor data autocheck duration
    Serial.println("Set Sensor data autocheck duration");
  }   

  Serial.println(" ");

  ///////////////확인을위한 출력
  //url
  Serial.print("url = ");
  Serial.println(url);

  //method
  Serial.print("method = ");
  Serial.println(method);

  //body
  //id
  Serial.print("id = ");
  Serial.println(id);
  //sensor
  Serial.print("sensor = ");
  Serial.println(sensor);
  //value
  Serial.print("value = ");
  Serial.println(value);
  //device 
  Serial.print("device = ");
  Serial.println(device);
  //state
  //Serial.print("state = ");
  //Serial.println(state);
  //second
  Serial.print("second = ");
  Serial.println(second);
}

void loop() {
  // put your main code here, to run repeatedly:

}
