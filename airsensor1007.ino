#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

// 핀 및 센서 설정
#define DHTPIN D6       // 온습도 센서 DATA 핀
#define DHTTYPE DHT11   // 파란색 센서면 DHT11, 하얀색 센서면 DHT22
#define LIGHTPIN A0     // 조도 센서 (아날로그 A0 핀)

// ESP8266 자체 와이파이(AP 모드) 설정
const char* ssid = "yeyoung_room";
const char* password = "12345678";

DHT dht(DHTPIN, DHTTYPE);
ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  // 조도 센서 핀 입력 설정
  pinMode(LIGHTPIN, INPUT);

  // 자체 와이파이(AP 모드) 시작
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  Serial.println("\n--- AirSensor 보드 준비 완료 ---");
  Serial.print("연결할 와이파이: ");
  Serial.println(ssid);
  Serial.print("서버 IP 주소: http://");
  Serial.println(WiFi.softAPIP());

  // HTML 웹페이지의 데이터 요청(/data) 처리
  server.on("/data", []() {
    // 센서 데이터 읽기
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    int light = analogRead(LIGHTPIN); // A0 핀에서 조도 값(0 ~ 1023) 읽기

    // 온습도 센서 오류 처리 (읽지 못했을 경우 -1 전송)
    if (isnan(t) || isnan(h)) {
      t = -1;
      h = -1;
    }

    // JSON 데이터 생성 (조도 값 "light" 추가)
    String json = "{";
    json += "\"temp\":" + String(t) + ",";
    json += "\"hum\":" + String(h) + ",";
    json += "\"light\":" + String(light) + ",";
    json += "\"sound\": -1,";
    json += "\"dust\": -1";
    json += "}";

    // 브라우저 CORS 보안 에러 방지 헤더 추가
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", json);
  });

  server.begin();
}

void loop() {
  server.handleClient();
}