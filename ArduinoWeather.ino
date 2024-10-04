#include <ArduinoHttpClient.h>
#include <WiFiS3.h>
#include "settings.h"

char ssid[] = SECRET_SSID;
char password[] = SECRET_PASSWORD;

//char serverAdress[] = "192.168.0.128";  //PC
char serverAdress[] = "192.168.0.205";  //Laptop
int port = 8080;

int temperatureSensorPin = A5;
int temperatureLedPin = 7;
int temperatureSensorValue = 0;

int brightnessSensorPin = A2;
int brightness = 0;
String brightnessString = "0";

WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverAdress, port);


void setup() {
  Serial.begin(9600);
  Serial.println("Ansluter till wifi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("...");
    delay(500);
  }
  Serial.println("Ansluten till wifi");
  pinMode(temperatureLedPin, OUTPUT);
}

void loop() {
  digitalWrite(temperatureLedPin, LOW); //Turns off the LED on the temperature sensor so that it doesn't affect the brightness sensor.
  brightnessString = String(analogRead(brightnessSensorPin));
  temperatureSensorValue = analogRead(temperatureSensorPin);


  String weatherData = "{\"brightness\": " + brightnessString + ",\"temperature\": " + temperatureSensorValue + "}";
  delay(500);  //Delay so that weatherData is correct before trying to send it

  client.beginRequest();
  client.post("/postWeatherData");

  client.sendHeader("Content-Type", "application/json");
  client.sendHeader("Content-Length", weatherData.length());

  client.beginBody();
  client.print(weatherData);
  client.endRequest();

  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  delay(500); //Delay so that response and statusCode is correct

  if (statusCode == 200) {
    response = "\"Everything went as planned\"";
  }
  delay(500); //Delay so that response is correct before trying to acte on it

  String statusCodeAsString = String(statusCode);
  String errorStringData = "{\"statusCode\": " + statusCodeAsString + ",\"responseString\": " + response + "}";

  delay(500); //Delay so that errorStringData is correct before trying to send it

  client.beginRequest();
  client.post("/error");

  client.sendHeader("Content-Type", "application/json");
  client.sendHeader("Content-Length", errorStringData.length());

  client.beginBody();
  client.print(errorStringData);
  client.endRequest();

  int statusCodeError = client.responseStatusCode();
  String responseError = client.responseBody();

  delay(898000);  // 15 minute total delay so that the times doesn't change over time
}
