#include <ArduinoJson.h>

void setup() {
  Serial.begin(9600);

  //char json[]= "{ \"url\" : \"/range\", \"method\" : \"PUT\", \"body\" : { \"device\" : \"motor\", \"state\" : 256 } }";
  
  char json[]= "{ \"url\" : \"/range\", \"method\" : \"GET\", \"body\" : { \"device\" :  \"motor\" } }";
  
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject & root = jsonBuffer.parseObject(json);

  const char* url = root ["url"];
  const char* method = root ["method"];
  
  JsonObject & body = root ["body"];
  const char* id = body ["id"];
  const char* sensor = body ["sensors"];
  const int value = body["value"];
  const char* device = body["device"];
  const int state = body["state"];

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
  Serial.print("state = ");
  Serial.println(state);
}

void loop() {
  // put your main code here, to run repeatedly:

}
