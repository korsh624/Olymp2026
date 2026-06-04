#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "MC";
const char* password = "qwertyui1";

#define RED_LED    21
#define YELLOW_LED 22
#define GREEN_LED  23

WebServer server(80);

void updateLights(String color) {
  digitalWrite(RED_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(GREEN_LED, LOW);

  if (color == "red")
    digitalWrite(RED_LED, HIGH);

  if (color == "yellow")
    digitalWrite(YELLOW_LED, HIGH);

  if (color == "green")
    digitalWrite(GREEN_LED, HIGH);
}

void handleRoot() {
  String page = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<title>Светофор ESP32</title>
<style>
body{
  font-family:Arial;
  text-align:center;
  margin-top:50px;
}
button{
  width:200px;
  height:70px;
  font-size:24px;
  margin:10px;
  border:none;
  border-radius:10px;
  color:white;
}
.red{background:red;}
.yellow{background:orange;}
.green{background:green;}
.off{background:gray;}
</style>
</head>
<body>

<h1>Управление светофором</h1>

<p><a href="/red"><button class="red">Красный</button></a></p>
<p><a href="/yellow"><button class="yellow">Жёлтый</button></a></p>
<p><a href="/green"><button class="green">Зелёный</button></a></p>
<p><a href="/off"><button class="off">Выключить</button></a></p>

</body>
</html>
)rawliteral";

  server.send(200, "text/html", page);
}

void setup() {
  Serial.begin(115200);

  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  delay(1000);
  digitalWrite(RED_LED, HIGH);
  digitalWrite(YELLOW_LED, HIGH);
  digitalWrite(GREEN_LED, HIGH);
  delay(1000);
    digitalWrite(RED_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  updateLights("");

  WiFi.begin(ssid, password);

  Serial.print("Подключение");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("IP адрес: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);

  server.on("/red", []() {
    updateLights("red");
    server.sendHeader("Location", "/");
    server.send(303);
  });

  server.on("/yellow", []() {
    updateLights("yellow");
    server.sendHeader("Location", "/");
    server.send(303);
  });

  server.on("/green", []() {
    updateLights("green");
    server.sendHeader("Location", "/");
    server.send(303);
  });

  server.on("/off", []() {
    updateLights("");
    server.sendHeader("Location", "/");
    server.send(303);
  });

  server.begin();

  Serial.println("Веб-сервер запущен");
}

void loop() {
  server.handleClient();
}
