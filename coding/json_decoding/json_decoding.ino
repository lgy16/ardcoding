#include <ArduinoJson.h>

void setup() {
  Serial.begin(9600);

  char json[]="";
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject & root = jsonBuffer.parseObject(json);

  const char* url = root ["url"];
  const char* method = root ["method"];
  


}

void loop() {
  // put your main code here, to run repeatedly:

}
