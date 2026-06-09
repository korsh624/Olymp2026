#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

#define RED_LED     21
#define YELLOW_LED  22
#define GREEN_LED   23

Preferences prefs;
WebServer server(80);

String wifiSSID = "";
String wifiPASS = "";


// =====================
// Светофор
// =====================

void setLight(String color)
{
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


// =====================
// WiFi
// =====================

void loadWiFiSettings()
{
    prefs.begin("wifi", true);

    wifiSSID = prefs.getString("ssid", "");
    wifiPASS = prefs.getString("pass", "");

    prefs.end();
}

void saveWiFiSettings()
{
    prefs.begin("wifi", false);

    prefs.putString("ssid", wifiSSID);
    prefs.putString("pass", wifiPASS);

    prefs.end();

    Serial.println("WiFi настройки сохранены.");
}

void connectWiFi()
{
    if (wifiSSID.length() == 0)
    {
        Serial.println("SSID не задан.");
        return;
    }

    Serial.println();
    Serial.print("Подключение к ");
    Serial.println(wifiSSID);

    WiFi.mode(WIFI_STA);
    WiFi.begin(wifiSSID.c_str(), wifiPASS.c_str());

    int cnt = 0;

    while (WiFi.status() != WL_CONNECTED && cnt < 30)
    {
        delay(500);
        Serial.print(".");
        cnt++;
    }

    Serial.println();

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("WiFi подключен");
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());
    }
    else
    {
        Serial.println("Подключение не удалось");
    }
}


// =====================
// WEB
// =====================

void handleRoot()
{
    String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<title>ESP32 Traffic Light</title>

<style>
body{
font-family:Arial;
text-align:center;
margin-top:40px;
background:#f0f0f0;
}

button{
width:220px;
height:70px;
font-size:24px;
border:none;
border-radius:12px;
margin:10px;
color:white;
cursor:pointer;
}

.red{background:red;}
.yellow{background:orange;}
.green{background:green;}
.off{background:gray;}
</style>

</head>
<body>

<h1>Светофор ESP32</h1>

<a href="/red"><button class="red">Красный</button></a><br>
<a href="/yellow"><button class="yellow">Жёлтый</button></a><br>
<a href="/green"><button class="green">Зелёный</button></a><br>
<a href="/off"><button class="off">Выключить</button></a>

</body>
</html>
)rawliteral";

    server.send(200, "text/html", html);
}

void startWebServer()
{
    server.on("/", handleRoot);

    server.on("/red", []()
    {
        setLight("red");
        server.sendHeader("Location", "/");
        server.send(303);
    });

    server.on("/yellow", []()
    {
        setLight("yellow");
        server.sendHeader("Location", "/");
        server.send(303);
    });

    server.on("/green", []()
    {
        setLight("green");
        server.sendHeader("Location", "/");
        server.send(303);
    });

    server.on("/off", []()
    {
        setLight("");
        server.sendHeader("Location", "/");
        server.send(303);
    });

    server.begin();

    Serial.println("WEB сервер запущен");
}


// =====================
// SERIAL
// =====================

void processSerial()
{
    if (!Serial.available())
        return;

    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd.startsWith("ssid="))
    {
        wifiSSID = cmd.substring(5);

        Serial.print("Новый SSID: ");
        Serial.println(wifiSSID);
    }
    else if (cmd.startsWith("pass="))
    {
        wifiPASS = cmd.substring(5);

        Serial.println("Новый пароль принят");
    }
    else if (cmd == "save")
    {
        saveWiFiSettings();
    }
    else if (cmd == "show")
    {
        Serial.println();
        Serial.println("===== WIFI =====");
        Serial.println("SSID : " + wifiSSID);
        Serial.println("PASS : " + wifiPASS);
        Serial.println("================");
    }
    else if (cmd == "wifi")
    {
        connectWiFi();
    }
    else if (cmd == "reboot")
    {
        ESP.restart();
    }
    else
    {
        Serial.println();
        Serial.println("Команды:");
        Serial.println("ssid=имя_сети");
        Serial.println("pass=пароль");
        Serial.println("show");
        Serial.println("save");
        Serial.println("wifi");
        Serial.println("reboot");
    }
}


// =====================
// SETUP
// =====================

void setup()
{
    Serial.begin(115200);

    pinMode(RED_LED, OUTPUT);
    pinMode(YELLOW_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);

    setLight("");

    delay(1000);

    loadWiFiSettings();

    connectWiFi();

    if (WiFi.status() == WL_CONNECTED)
    {
        startWebServer();
    }

    Serial.println();
    Serial.println("ESP32 готова.");
}


// =====================
// LOOP
// =====================

void loop()
{
    processSerial();

    if (WiFi.status() == WL_CONNECTED)
    {
        server.handleClient();
    }
}
