#include <WiFi.h>
#include "Arduino.h"
#include "DHT.h"
#include "SI114X.h"
#include "BMP085.h"
#include <Wire.h>
#include <Firebase_Esp_Client.h>

float temperature; // parameters
float humidity;
float pressure;
float baro;
float uv;
float visibility;
float ir;

SI114X SI1145 = SI114X();
BMP085 thebaro;

#define DHTPIN 5
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "WeatherStation"
#define WIFI_PASSWORD "$$x541465DFA"

#define API_KEY "NeVerGonnaGIVEyou4pne4ergonnaletudown"
#define DATABASE_URL "https://weather-jkdfhuai-default-rtdb.asia-southeast1.firebasedatabase.app/" 

FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;   

void setup()
{
  thebaro.init();
  dht_sensor.begin();
  Serial.begin(115200);
  Serial.println("Beginning Si1145!");
  while (!SI1145.Begin())
  {
    Serial.println("Si1145 is not ready!");
    delay(1000);
  }
  Serial.println("Si1145 is ready!");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  config.api_key = API_KEY;

  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop()
{
    temperature = dht_sensor.readTemperature();
    humidity = dht_sensor.readHumidity();
    pressure = thebaro.bmp085GetPressure(thebaro.bmp085ReadUP());
    baro = pressure / 100;
    visibility = SI1145.Readvisibility(); 
    ir = SI1145.ReadIR();
    uv = SI1145.ReadUV();

    if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0)){
    //since we want the data to be updated every second
    sendDataPrevMillis = millis();
    // Enter Temperature in to the Weather Station Table
    if (Firebase.RTDB.setInt(&fbdo, "Weather/Temperature", temperature)){
      // This command will be executed even if you dont serial print but we will make sure its working
      Serial.print("Temperature : ");
      Serial.println(temperature);
    }
    else {
      Serial.println("Failed to Read from the Sensor");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    
    // Enter Humidity in to the Weather Station Table
    if (Firebase.RTDB.setFloat(&fbdo, "Weather/Humidity", humidity)){
      Serial.print("Humidity : ");
      Serial.print(humidity);
    }
    else {
      Serial.println("Failed to Read from the Sensor");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    // Enter pressure in to the Weather Station Table
    if (Firebase.RTDB.setFloat(&fbdo, "Weather/Pressure", pressure)){
      Serial.print("Pressure : ");
      Serial.print(pressure);
    }
    else {
      Serial.println("Failed to read pressure");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    // Enter barometer reading in to the Weather Station Table
    if (Firebase.RTDB.setFloat(&fbdo, "Weather/Barometer", baro)){
      Serial.print("Barometer : ");
      Serial.print(baro);
    }
    else {
      Serial.println("Failed to read from sensor");
      Serial.println("REASON: " + fbdo.errorReason());
    }// Enter uv in to the Weather Station Table
    if (Firebase.RTDB.setFloat(&fbdo, "Weather/UV", uv)){
      Serial.print("UV : ");
      Serial.print(uv);
    }
    else {
      Serial.println("Failed to read from sensor");
      Serial.println("REASON: " + fbdo.errorReason());
    }// Enter visibility in to the Weather Station Table
    if (Firebase.RTDB.setFloat(&fbdo, "Weather/Visibility", visibility)){
      Serial.print("Visibility : ");
      Serial.print(visibility);
    }
    else {
      Serial.println("Failed to read from sensor");
      Serial.println("REASON: " + fbdo.errorReason());
    }// Enter ir reading in to the Weather Station Table
    if (Firebase.RTDB.setFloat(&fbdo, "Weather/IRreading", ir)){
      Serial.print("IR reading : ");
      Serial.print(ir);
    }
    else {
      Serial.println("Failed to read from sensor");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
}


