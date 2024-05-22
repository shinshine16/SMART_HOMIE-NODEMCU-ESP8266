#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

// Inisialisasi objek servo
Servo myservo;

/* Put your SSID & Password */
const char* ssid = "SmartHomie";  // Enter SSID here
const char* password = "87654321";  //Enter Password here

/* Put IP Address details */
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

ESP8266WebServer server(80);

uint8_t LED1pin = D7;
bool LED1status = LOW;

uint8_t LED2pin = D6;
bool LED2status = LOW;

uint8_t GATEpin = D8;
bool GATEstatus = false;

void setup() {
  Serial.begin(115200);
  pinMode(LED1pin, OUTPUT);
  pinMode(LED2pin, OUTPUT);
  myservo.attach(GATEpin);

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  
  server.on("/", handle_OnConnect);
  server.on("/led1on", handle_led1on);
  server.on("/led1off", handle_led1off);
  server.on("/led2on", handle_led2on);
  server.on("/led2off", handle_led2off);
  server.on("/gateopen", handle_gateOpen);
  server.on("/gateclose", handle_gateClose);
  server.onNotFound(handle_NotFound);
  
  server.begin();
  Serial.println("HTTP server started");
}
void loop() {
  server.handleClient();
  if(LED1status)
  {digitalWrite(LED1pin, HIGH);}
  else
  {digitalWrite(LED1pin, LOW);}
  
  if(LED2status)
  {digitalWrite(LED2pin, HIGH);}
  else
  {digitalWrite(LED2pin, LOW);}

  if(GATEstatus)
  {myservo.write(180);}
  else
  {myservo.write(0);}
}

void handle_OnConnect() {
  LED1status = LOW;
  LED2status = LOW;
  GATEstatus = false;
  server.send(200, "text/html", SendHTML(LED1status,LED2status,GATEstatus)); 
}

void handle_led1on() {
  LED1status = HIGH;
  server.send(200, "text/html", SendHTML(true,LED2status,GATEstatus)); 
}

void handle_led1off() {
  LED1status = LOW;
  server.send(200, "text/html", SendHTML(false,LED2status,GATEstatus)); 
}

void handle_led2on() {
  LED2status = HIGH;
  server.send(200, "text/html", SendHTML(LED1status,true,GATEstatus)); 
}

void handle_led2off() {
  LED2status = LOW;
  server.send(200, "text/html", SendHTML(LED1status,false,GATEstatus)); 
}
void handle_gateOpen() {
  // Mengatur posisi awal servo ke 0 derajat
  GATEstatus = true;
  server.send(200, "text/html", SendHTML(LED1status,LED2status,true)); 
}

void handle_gateClose() {
  // Mengatur posisi awal servo ke 180 derajat
  GATEstatus = false;
  server.send(200, "text/html", SendHTML(LED1status,LED2status,false)); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(uint8_t led1stat, uint8_t led2stat, uint8_t gatestat){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>LED Control</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #1abc9c;}\n";
  ptr +=".button-on:active {background-color: #16a085;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>Smart Homie</h1>\n";
  ptr +="<h3>Lamp and Gate Controller</h3>\n";
  
  if(led1stat)
  {ptr +="<p>LED1 Status: ON</p><a class=\"button button-off\" href=\"/led1off\">OFF</a>\n";}
  else
  {ptr +="<p>LED1 Status: OFF</p><a class=\"button button-on\" href=\"/led1on\">ON</a>\n";}

  if(led2stat)
  {ptr +="<p>LED2 Status: ON</p><a class=\"button button-off\" href=\"/led2off\">OFF</a>\n";}
  else
  {ptr +="<p>LED2 Status: OFF</p><a class=\"button button-on\" href=\"/led2on\">ON</a>\n";}

  if(gatestat)
  {ptr +="<p>Gate Status: Open</p><a class=\"button button-off\" href=\"/gateclose\">Close</a>\n";}
  else
  {ptr +="<p>Gate Status: Close</p><a class=\"button button-on\" href=\"/gateopen\">Open</a>\n";}

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}