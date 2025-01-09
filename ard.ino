#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>

String ssid = "";
String password = "";
String botToken = "";
String chatId = "";
String pcDataUrl = "";

WebServer server(80);

void sendTelegramMessage(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://api.telegram.org/bot" + botToken + "/sendMessage";
    url += "?chat_id=" + chatId;
    url += "&text=" + message;

    http.begin(url);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      Serial.println("Сообщение отправлено!");
    } else {
      Serial.print("Ошибка отправки: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("Wi-Fi не подключён");
  }
}

void handleConfig() {
  if (server.hasArg("ssid") && server.hasArg("password") &&
      server.hasArg("botToken") && server.hasArg("chatId") && server.hasArg("pcDataUrl")) {
    ssid = server.arg("ssid");
    password = server.arg("password");
    botToken = server.arg("botToken");
    chatId = server.arg("chatId");
    pcDataUrl = server.arg("pcDataUrl");

    server.send(200, "text/plain", "Данные сохранены. Переключаюсь на клиентский режим Wi-Fi...");
    delay(1000);

    server.stop();
    delay(100);

    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_STA);
    connectToWiFi();
  } else {
    server.send(200, "text/plain", "Укажите параметры: ssid, password, botToken, chatId, pcDataUrl");
  }
}

void connectToWiFi() {
  Serial.println("Подключение к Wi-Fi...");
  WiFi.begin(ssid.c_str(), password.c_str());

  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000) { // 15 секунд на подключение
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi подключён!");
    Serial.print("IP-адрес: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nНе удалось подключиться к Wi-Fi");
  }
}
String getPCData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(pcDataUrl);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      String payload = http.getString();
      http.end();
      return payload;
    } else {
      Serial.print("Ошибка получения данных: ");
      Serial.println(httpResponseCode);
      http.end();
      return "Ошибка запроса к ПК";
    }
  } else {
    return "Wi-Fi не подключён";
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAP("ESP32_Config");
  Serial.println("\nТочка доступа: ESP32_Config");
  Serial.print("IP адрес: ");
  Serial.println(WiFi.softAPIP());
  server.on("/config", handleConfig);
  server.begin();
  Serial.println("Сервер запущен");
  while (ssid == "" || password == "" || botToken == "" || chatId == "" || pcDataUrl == "") {
    server.handleClient();
  }
  WiFi.softAPdisconnect(true);
  delay(1000);
  WiFi.mode(WIFI_STA);
  connectToWiFi();
  sendTelegramMessage("ESP32 подключён и готов к работе!");
}

void loop() {
  String pcData = getPCData();
  sendTelegramMessage("Состояние ПК: " + pcData);
  delay(10000); // 1 час = 3600000 мс
}
