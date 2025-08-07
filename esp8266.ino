#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#define LED_PIN 2 
// WiFi config
const char* ssid = "No Phone No Brand";
const char* password = "29varanpong";
// Telegram Bot
const char* botToken = "8098175957:AAEdZ2MYXfvd5ok3aGzuUvGlX4dRa6JGy2Q";
const String chat_id = "-4860196307";

// URL เต็มของปลายทาง
const char* server_host = "http://172.20.10.3/receive";

// สร้าง Server และ Telegram Bot
ESP8266WebServer server(80);
WiFiClientSecure client;
UniversalTelegramBot bot(botToken, client);

// เวลาอัปเดต Telegram
unsigned long lastTelegramCheck = 0;
const unsigned long telegramInterval = 3000; // 3 วินาที

void setup() {
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi connected");

  Serial.print("ESP8266 IP Address: ");
  Serial.println(WiFi.localIP());

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // เริ่มต้นปิดไฟ

  client.setInsecure();

  server.on("/post_log", HTTP_POST, handlePostLog);
  server.on("/send_telegram", HTTP_GET, handleTelegramSend);
  server.begin();

  Serial.println("HTTP Server started");
}
String logBuffer[20];
int logIndex = 0;

void addToLogBuffer(String msg) {
  logBuffer[logIndex] = msg;
  logIndex = (logIndex + 1) % 20;
}

void loop() {
  server.handleClient();

  // อ่านคำสั่งจาก Telegram
  if (millis() - lastTelegramCheck > telegramInterval) {
    checkTelegramMessages();
    lastTelegramCheck = millis();
  }

  // ส่ง log ไปยัง server ทุก 10 วินาที
  static unsigned long lastSend = 0;
  if (millis() - lastSend > 10000) {
    sendLogToServer("เปิดไฟแล้ว");
    lastSend = millis();
  }
}

// ✅ รับ log ผ่าน HTTP POST แล้วแสดงทาง Serial

void handlePostLog() {
  String body = server.arg("plain");
  Serial.print("Got log from ESP32: ");
  Serial.println(body);

  StaticJsonDocument<200> doc; 
  DeserializationError error = deserializeJson(doc, body);
  
  if (!error) {
    const char* flashState = doc["flash_state"];      // อ่าน flash_state เป็น string
    int finger_count = doc["finger_count"];           // อ่าน finger_count เป็น int
    
    if (flashState != nullptr) {
      if (strcmp(flashState, "ON") == 0) {
        digitalWrite(LED_PIN, LOW); // เปิดไฟ LED (active low)
        Serial.println("LED ON");
      } else if (strcmp(flashState, "OFF") == 0) {
        digitalWrite(LED_PIN, HIGH); // ปิดไฟ LED
        Serial.println("LED OFF");
      }
    } else {
      Serial.println("flash_state not found in JSON");
    }
    
    Serial.print("Finger count: ");
    Serial.println(finger_count);
    
    // ตัวอย่างเช็ค finger_count == 4
    if (finger_count == 4) {
      Serial.println("Detected 4 fingers");
      // จะทำอะไรเพิ่มเติมตรงนี้ก็ได้
    }
    
  } else {
    Serial.print("Failed to parse JSON: ");
    Serial.println(error.c_str());
  }

  server.send(200, "text/plain", "OK");
}
// ✅ รับ HTTP GET แล้วส่งข้อความ Telegram
void handleTelegramSend() {
  if (server.hasArg("msg")) {
    String msg = server.arg("msg");
    Serial.println("Received log: " + msg);

    bool sent = bot.sendMessage(chat_id, "📢 Log: " + msg, "");
    if (sent) {
      server.send(200, "text/plain", "Message sent: " + msg);
    } else {
      server.send(500, "text/plain", "Failed to send Telegram message");
    }
  } else {
    server.send(400, "text/plain", "Missing 'msg' parameter");
  }
}

// ✅ ส่ง HTTP GET ไปยังปลายทาง
void sendLogToServer(String logMessage) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient httpClient;

    String url = String(server_host) + "/send_telegram?msg=" + logMessage;
    Serial.println("Sending HTTP GET to: " + url);

    http.begin(httpClient, url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      Serial.printf("HTTP Response code: %d\n", httpCode);
      String payload = http.getString();
      Serial.println("Response: " + payload);
    } else {
      Serial.printf("HTTP request failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
}

void checkTelegramMessages() {
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

  while (numNewMessages) {
    for (int i = 0; i < numNewMessages; i++) {
      String text = bot.messages[i].text;
      String from = bot.messages[i].from_name;

      Serial.println("Got message: " + text + " from " + from);

      // ✅ คำสั่ง /start
      if (text == "/start") {
        if (WiFi.status() != WL_CONNECTED) {
          bot.sendMessage(chat_id, "📡 Hello! WiFi not connected.", "");
        } else {
          bot.sendMessage(chat_id,
                          "👋 Hello! Bot is online.\n📍 IP: " + WiFi.localIP().toString(),
                          "");
        }
      }

      // ✅ คำสั่ง /sent
      else if (text == "/sent") {
        if (WiFi.status() != WL_CONNECTED) {
          bot.sendMessage(chat_id, "📡 Hello! WiFi not connected.", "");
        } else {
          String allLogs = "📤 Last 20 logs:\n";

          for (int j = 0; j < 20; j++) {
            int idx = (logIndex + j) % 20;
            if (logBuffer[idx].length() > 0) {
              allLogs += String(j + 1) + ". " + logBuffer[idx] + "\n";
            }
          }

          bot.sendMessage(chat_id, allLogs);
          Serial.println("Sending last 20 logs via /sent command:");
          Serial.println(allLogs);
        }
      }

      // ✅ คำสั่ง /open
      else if (text == "/open") {
        digitalWrite(LED_PIN, LOW);  // เปิดไฟ
        bot.sendMessage(chat_id, "💡 LED is now ON.", "");
      }

      // ✅ คำสั่ง /close
      else if (text == "/close") {
        digitalWrite(LED_PIN, HIGH);  // ปิดไฟ
        bot.sendMessage(chat_id, "💡 LED is now OFF.", "");
      }

      // ✅ คำสั่งไม่รู้จัก
      else {
        bot.sendMessage(chat_id,
                        "❓ Unknown command.\nAvailable commands:\n"
                        "/start\n/sent\n/open\n/close",
                        "");
      }
    }

    // ตรวจสอบข้อความใหม่อีกครั้ง
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }
}
