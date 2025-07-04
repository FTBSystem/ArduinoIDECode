#include <ESP8266WiFi.h>

const char* ssid = "wifi";
const char* password = "password";

String name = "HugNode1";

const char* host = "dweet.io";
const int httpPort = 80;

int error = 0;
//const char* host = "192.168.1.78";
//const int httpPort = 1010;


long a, b, send;

String data = "";


void GetIN() {

  a = digitalRead(12);
  b = digitalRead(13);
  delay(20);
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Serial.println(String(name) + " boot");



  pinMode(12, INPUT);
  pinMode(13, INPUT);


  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("connecting wifi...");

    if (error > 50) {

      ESP.restart();
    }

    error++;
  }

  Serial.println("Connected");
}

void loop() {
  // put your main code here, to run repeatedly:
  GetIN();


  WiFiClient client;

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    delay(5);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print("connecting wifi...");

      if (error > 50) {

        ESP.restart();
      }

      error++;
    }



    return;
  }

  if (a == 0 && b == 0) {
    data = ("Hug=True");
    send = 1;
  }



  if (a == 1 || b == 1) {
    data = ("Hug=False");
    send++;
  }

  if (send < 4) {
    client.print("GET /dweet/for/" + String(name) + "?" + String(data) + "&time=" + String(millis()) + "\r\n\r\n");
    Serial.println(send);
    Serial.println("Send:");
    Serial.println("GET /dweet/for/" + String(name) + "?" + String(data) + "&time=" + String(millis()) + "\r\n\r\n");
  }





  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.println(line);
  }

  delay(2000);
}
