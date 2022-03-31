/*
 *  Sends all the sensor readings to the provided URL
 *
 *  
 *
 */
#include "DHT.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "FS.h"
#include "SPIFFS.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <math.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"

#define DHTTYPE DHT11
#define DHTPIN 4
#define ONE_WIRE_BUS 13
#define TOUCH_N_HOLD_LIMIT 4
#define BPM_SENSOR_MAX_WAIT 30
#define MAX_BRIGHTNESS 255
#define HEART_PIN 36
#define ECG_BUFFER_LENGTH 2048
#define ECG_ZERO_READING_LIMIT 200
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// Sensors
DHT dht(DHTPIN, DHTTYPE);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
MAX30105 particleSensor;
Adafruit_MPU6050 mpu;

const char* ssid = "myesp32";
const char* password = "shoxie123";

const char* wifi_ssid = "";
const char* wifi_password = "";

boolean connected = false;
boolean softAPDisconnectNeeded = false;

const char* root_ca = "-----BEGIN CERTIFICATE-----\n" \
"MIIFJTCCBA2gAwIBAgISA5czjV4W11AtG0owIJNt8grdMA0GCSqGSIb3DQEBCwUA\n" \
"MDIxCzAJBgNVBAYTAlVTMRYwFAYDVQQKEw1MZXQncyBFbmNyeXB0MQswCQYDVQQD\n" \
"EwJSMzAeFw0yMjAyMjIwMjExNDdaFw0yMjA1MjMwMjExNDZaMBsxGTAXBgNVBAMT\n" \
"EGJhcnQubmtrYXJ5YS5jb20wggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIB\n" \
"AQDTG/fpy3QrI7PA7Ta/bVE3RaXByRlWjeR3BMLwtvhuWe1+hAkLDR1qqa0o0LQl\n" \
"BKlTo/XkTA0uFBp2mX/mBLmAXLkEoQ0V5Gntz96Lr3YvI6k1I5Yl1pPM2mb71jK3\n" \
"OJzJITz426ql4PTUMybOh7PuYpvwmriuR5xEtDx9U2zaWtcvvfQsjHIS4pd06H/x\n" \
"EK5rjQ5Mg0Mqk5VCytBdPyLs3UwJpEXxfaMzSweXBaxAaWY1h+c1EaIm9OM1s83t\n" \
"oqpHVHdmZCvA4QBiwzx8z1UJVx77BZni/InD543NRnkOLhPRdItm8ukaMbFuSMb7\n" \
"XRDY5yMNfFnfIhNH+7RQ4nURAgMBAAGjggJKMIICRjAOBgNVHQ8BAf8EBAMCBaAw\n" \
"HQYDVR0lBBYwFAYIKwYBBQUHAwEGCCsGAQUFBwMCMAwGA1UdEwEB/wQCMAAwHQYD\n" \
"VR0OBBYEFIN3ATElW2bpq3Bp06v2M0YZSN60MB8GA1UdIwQYMBaAFBQusxe3WFbL\n" \
"rlAJQOYfr52LFMLGMFUGCCsGAQUFBwEBBEkwRzAhBggrBgEFBQcwAYYVaHR0cDov\n" \
"L3IzLm8ubGVuY3Iub3JnMCIGCCsGAQUFBzAChhZodHRwOi8vcjMuaS5sZW5jci5v\n" \
"cmcvMBsGA1UdEQQUMBKCEGJhcnQubmtrYXJ5YS5jb20wTAYDVR0gBEUwQzAIBgZn\n" \
"gQwBAgEwNwYLKwYBBAGC3xMBAQEwKDAmBggrBgEFBQcCARYaaHR0cDovL2Nwcy5s\n" \
"ZXRzZW5jcnlwdC5vcmcwggEDBgorBgEEAdZ5AgQCBIH0BIHxAO8AdQBGpVXrdfqR\n" \
"IDC1oolp9PN9ESxBdL79SbiFq/L8cP5tRwAAAX8faeRHAAAEAwBGMEQCIQD5+tQC\n" \
"MsRIhCfS8QCf1pZ9VNrfRqOyh7Pl5clKuiuIDQIfKvjH6pU/TZruJtntYnfdVaGL\n" \
"2nw4r7OI1EfFa4wl+gB2AN+lXqtogk8fbK3uuF9OPlrqzaISpGpejjsSwCBEXCpz\n" \
"AAABfx9p5hMAAAQDAEcwRQIhAOOcuert9tbRafbPpOMymcV6KWpPX2JV0t+wpESO\n" \
"q3AlAiB5Pi/0MyxOeBeb4Rlc0Pc4QpFnphgrBB/dteQOZIFWdzANBgkqhkiG9w0B\n" \
"AQsFAAOCAQEATvZMhmR/zuN/UEoitO37iEnUpgQ8uA+V5qGLSwLoPtmRgsaDL6+k\n" \
"lQML1ArmdVUxVsggdmt2U/paNFNsNsc/ADH2NwgkKa/mduxWudqD0llarioDfghu\n" \
"LT1jbwuTavlXFBvSSdt3uGwoMRb9RfJyzNutsf9Z/aRS9BOUKBsK4Zk1oYvwt9kk\n" \
"dvFBFBzAaYLqWFVHAhtTDBMxwXCpkJmnIXgeviTFgGfLWaGbOz3hDzi2k6Sz6mC+\n" \
"Hp5pIjnYtyNESJSgZJyKfh0ao7ZelFu6L1tsShILTEVieYBtmx0DGTXcI2ntw/ZB\n" \
"+xrcuCqa54Os+b+w9DvsTyDiqbd2Ug3dyQ==\n" \
"-----END CERTIFICATE-----\n";

AsyncWebServer server(80);

// Unique Device ID
String deviceId = "0ee572eb-1175";

// Variables for sensor readings

float bodyTemp;
float humidity;
float envTemp;
int heartValue;
String ecgDefaultReading = "0000";
String ecgReading;
char str[5]; // Temporary buffer for individual ecg value
char zero = '0';
char ecgBuffer[ECG_BUFFER_LENGTH]; // Buffer for all ecg values
int bpmSum = 0;
int spo2Sum = 0;;
int32_t spo2;
int8_t validSPO2; //indicator to show if the SPO2 calculation is valid
int32_t heartRate; //indicator to show if the heart rate calculation is valid
int8_t validHeartRate;
uint32_t irBuffer[100]; //infrared LED sensor data
uint32_t redBuffer[100];  //red LED sensor data
int32_t bufferLength; //data length
byte pulseLED = 11;
// Touch sensor indication variables
boolean sendReadings = false;
int touchSensorValue = 1000;
int touchCount = 0;
// Variables for accelerometer/Gyroscope Fall detection
boolean fall = false;
int amplitude = 0;
int angleChange = 0;
int accCount = 0;

// Notification LED
byte notifLED = 2;

// POST APIs
const char* FALL_DETAILS_URL = "http://khandelwaltanmay.com/ping-of-things/fall_details/";
const char* SENSOR_DATA_URL = "http://khandelwaltanmay.com/ping-of-things/sensor_data/";


void setup()
{
    Serial.begin(115200);
    pinMode(notifLED, OUTPUT);
    pinMode(pulseLED, OUTPUT);
    WiFi.mode(WIFI_AP_STA);
    
    
    if(!SPIFFS.begin()) {
      Serial.println("SPIFFS Mount Failed");
      return;
    }
    WiFi.softAP(ssid, password);
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);

    // Handle main landing page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/index.html", "text/html");
    });

    server.on("/jquery-3.3.1.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/jquery-3.3.1.min.js", "text/javascript");
    });

    // connect to a wifi provided ssid and password
    server.on("/connect", HTTP_POST, [](AsyncWebServerRequest *request){
      if(connected) {
        request->send(200, "text/plain", "Already connected");
      }
      else {
        int paramsNr = request->params();
        Serial.println(paramsNr);
 
        for(int i=0;i<paramsNr;i++){
          AsyncWebParameter* p = request->getParam(i);
          if(p->name().equals("ssid")) {
            wifi_ssid = p->value().c_str();
          }
          else if (p->name().equals("pass")) {
            wifi_password = p->value().c_str();
          }
          Serial.print("Param name: ");
          Serial.println(p->name());
          Serial.print("Param value: ");
          Serial.println(p->value());
          Serial.println("------");
        }
        
        //connectWifi = true;
        WiFi.begin(wifi_ssid, wifi_password);
        unsigned long timeout = millis();
        Serial.print("Connecting to ");
        Serial.print(wifi_ssid);
        while (WiFi.status() != WL_CONNECTED) {
          if (millis() - timeout > 6000) {
            timeout = true;
            break;
          }
          delay(500);  
          Serial.print(".");
        }
        Serial.println("WiFi connected");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        
        request->send(200, "text/html", "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><title>Health Monitor</title></head><body><main><h2>Your Device is connected</h2></main></body>");
        connected = true;
        softAPDisconnectNeeded = true;
      }
        
    });

   // Scans and returns the list of WiFi networks available
    server.on("/getWifiList", HTTP_GET, [](AsyncWebServerRequest *request){
        int n = WiFi.scanNetworks();
        delay(100);
        Serial.println("scan done");
        if (n == 0) {
            Serial.println("no networks found");
            request->send(200, "application/json", "[]");
        } else {
          Serial.print(n);
          Serial.println(" networks found");
          String json = "[";
          for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            json += "{\"ssid\": \"";
            json += WiFi.SSID(i);
            json += "\", \"strength\": ";
            
            json += WiFi.RSSI(i);
            json += ", \"authMode\": ";
            json += WiFi.encryptionType(i);
            if(i == n - 1) {
              json += "}]";
            }
            else {
              json += "},";
            }
            delay(10);
          }
          request->send(200, "application/json", json);
        }
   
    });
    server.begin();
    // Initialize MAX30102 sensor - Start
    if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
    {
      Serial.println(F("MAX30105 was not found. Please check wiring/power."));
      while (1);
    }
    byte ledBrightness = 60;
    byte sampleAverage = 4;
    byte ledMode = 2;
    byte sampleRate = 100;
    int pulseWidth = 411;
    int adcRange = 4096;
    particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
    // Initialize MAX30102 sensor - End -->

    if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
      }
    }
    mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
    mpu.setGyroRange(MPU6050_RANGE_250_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    server.begin();
    dht.begin();
    sensors.begin();
     
    delay(500);
    Serial.println("Setup done");
}

void loop()
{
  // Disconnect Soft AP when device is connected to WiFi
  if(softAPDisconnectNeeded) {
    delay(500);
    WiFi.softAPdisconnect(true);
    softAPDisconnectNeeded = false;
  }
  // Accelerometer and Gyroscope readings
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  amplitude = round(pow(pow(a.acceleration.x,2)+pow(a.acceleration.y,2)+pow(a.acceleration.z,2),0.5));
  angleChange = round(pow(pow(g.gyro.x,2)+pow(g.gyro.y,2)+pow(g.gyro.z,2),0.5));

  // Fall detection - first threshold
  if(accCount == 0 && amplitude >= 15 && angleChange >= 3) {
    Serial.print("Amplitude: ");
    Serial.println(amplitude);
    Serial.print("Angle change: ");
    Serial.println(angleChange);
    accCount = 1;
  }
  // Fall detection - second threshold
  else if(accCount == 1 && amplitude >= 30 && angleChange >= 4) {
    Serial.print("Amplitude: ");
    Serial.println(amplitude);
    Serial.print("Angle change: ");
    Serial.println(angleChange);
    accCount = 2;
  }
  else {
    accCount = 0;
  }

  // Fall detected. notify the web application, which will trigger SMS notification
  if(accCount == 2) {
    Serial.println("Fall detected");
    accCount = 0;
    HTTPClient httpClient;
    httpClient.begin(FALL_DETAILS_URL);
    httpClient.addHeader("Content-Type", "application/json");
    int code  = httpClient.POST("{\"device_id\": \"" + deviceId + "\"}");
    Serial.print("Response code: ");
    Serial.println(code);
    httpClient.end();
    delay(1000);
  }
  
  touchSensorValue = touchRead(T3);
  // Touch-and-hold sensor. number of settings configured by TOUCH_N_HOLD_LIMIT variable
  while(touchSensorValue < 30 && touchCount < TOUCH_N_HOLD_LIMIT) {
    touchCount = touchCount + 1;
    delay(1000);
    touchSensorValue = touchRead(T3);
  }
  if(touchCount >= TOUCH_N_HOLD_LIMIT) {
    sendReadings = true;
  }
  touchCount = 0;

  if(sendReadings) {
    // Notification LED high to indicate request was accepted
    digitalWrite(notifLED, HIGH);
    Serial.println("Touch event detected. Sending readings.");
    // Small delay to let the user use the SPO2 sensor
    delay(1000);
    readEnvTempAndHumidity();   // Read environment temperature and humidity
    readBodyTemperature();    // Read body temperature

    // BPM and SPO2 readings - Start
    int irCount = 0;
    Serial.println("Checking for IR reading");
    // Wait for (100*BPM_SENSOR_MAX_WAIT) ms for any readings, else skip BPM/SPO2
    while(particleSensor.getIR() < 5000) {
      Serial.print(".");
      if(irCount == BPM_SENSOR_MAX_WAIT) {
        break;
      }
      irCount++;
      delay(100);
    }
    Serial.println();
    // Take average of 3 readings
    if(irCount < BPM_SENSOR_MAX_WAIT) {
      Serial.println("Taking BPM and SPO2 readings.");
      for(int j = 0; j < 3; j++) {
        calculateSpo2AndBPM();    
      }
      Serial.print("avg spo2: ");
      Serial.println(spo2Sum/3);
      Serial.print("avg bpm: ");
      Serial.println(bpmSum/3);
      digitalWrite(notifLED, LOW);
    }
    else {
      Serial.println("Skipping BPM and SPO2 readings.");
      heartRate = spo2 = bpmSum = spo2Sum = 0;
    }    
    // BPM and SPO2 readings - End -->
    
    // ECG readings start
    int k = 0;
    int ecgZeroCount = 0;
    while(k - 4 < ECG_BUFFER_LENGTH) {
      if(ecgZeroCount == ECG_ZERO_READING_LIMIT) {
        break;
      }
      heartValue = analogRead(HEART_PIN);
      Serial.println(heartValue);
      if(heartValue == 0) {
        ecgZeroCount++;
        continue;
      }
      int j = 0;
      // Convert int reading to char array (in reverse)
      while(heartValue != 0) {
        byte rem = heartValue % 10;
        str[j++] = (char)(rem + zero);
        heartValue = heartValue/10;
      }
      j--;
      // Copy char array in reverse from temp array to buffer
      for(; j >= 0; j--) {
        if(k == ECG_BUFFER_LENGTH) {
          break;
        }
        ecgBuffer[k++] = str[j]; 
      }
      // Comma separated readings
      if(k < ECG_BUFFER_LENGTH) {
        ecgBuffer[k++] = ',';  
      }
      delay(5);
    }
    
    if(ecgZeroCount < ECG_ZERO_READING_LIMIT) {
      ecgReading = ecgBuffer;
    }
    else {
      Serial.println("Zero count limit of ECG readings reached. Skipping ECG.");
      ecgReading = ecgDefaultReading;
    }
    // ECG readings end
    
    // Building request JSON
    String json = "{\"device_id\": \"" + deviceId + "\", \"body_temperature\": " + round(bodyTemp) + ", \"humidity\": " + round(humidity) + ", \"heart_rate\": " + (bpmSum/3) 
    + ", \"environment_temperature\": " + round(envTemp) + ", \"blood_oxygen\": " + (spo2Sum/3) + ", \"reading\": \"" + ecgReading + "\"}";
    Serial.println(json);
    // Initializing http client
    HTTPClient httpClient;
    httpClient.begin("http://54.159.190.199/ping-of-things/sensor_data/");
    httpClient.addHeader("Content-Type", "application/json");
    // Sending the request to the server
    int code  = httpClient.POST(json);
    Serial.print("Response code: ");
    Serial.println(code);
    String response = httpClient.getString();
    Serial.print("Response content: ");
    Serial.println(response);
    httpClient.end();
    delay(200);
    Serial.println("Readings sent. Reverting back to default values.");
    envTemp = humidity = bodyTemp = spo2 = heartRate = bpmSum = spo2Sum = 0;
    sendReadings = false;
  }
  delay(100);
}

void readEnvTempAndHumidity() {
  Serial.println("Reading environment temperature and humidity");
  humidity = dht.readHumidity();
  envTemp = dht.readTemperature(true);
  for(int i = 0; i < 10; i++) {
    
    if(!isnan(envTemp) && !isnan(humidity)) {
      break;
    }
    if(isnan(humidity) || humidity == 0) {
      humidity = dht.readHumidity();  
    }
    if(isnan(envTemp) || envTemp == 0) {
      envTemp = dht.readTemperature(true);  
    }
    delay(500);
  }
  if(isnan(envTemp)) {
    envTemp = 0;
  }
  if(isnan(humidity)) {
    humidity = 0;
  }
}
void readBodyTemperature() {
  sensors.requestTemperatures(); 
  bodyTemp = sensors.getTempFByIndex(0);
}

void calculateSpo2AndBPM() {
  
  bufferLength = 100; //buffer length of 100 stores 4 seconds of samples running at 25sps
  
  //read the first 100 samples, and determine the signal range
  for (byte i = 0 ; i < bufferLength ; i++)
  {
    while (particleSensor.available() == false) {
      particleSensor.check(); //Check the sensor for new data
    } 
    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample(); //We're finished with this sample so move to next sample
    //calculate heart rate and SpO2 after first 100 samples (first 4 seconds of samples)
    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

    //Continuously taking samples from MAX30102.  Heart rate and SpO2 are calculated every 1 second
      
    //dumping the first 25 sets of samples in the memory and shift the last 75 sets of samples to the top
    for (byte i = 25; i < 100; i++)
    {
      redBuffer[i - 25] = redBuffer[i];
      irBuffer[i - 25] = irBuffer[i];
    }
    //take 25 sets of samples before calculating the heart rate.
    for (byte i = 75; i < 100; i++)
    {
      while (particleSensor.available() == false){ //do we have new data?
        particleSensor.check(); //Check the sensor for new data
      }
      redBuffer[i] = particleSensor.getRed();
      irBuffer[i] = particleSensor.getIR();
 
      particleSensor.nextSample(); //We're finished with this sample so move to next sample
    } 
    if (validSPO2 == 1 && validHeartRate == 1 && spo2 > 30) {
      Serial.println("valid spo2 and HR found");
      Serial.println(spo2);
      spo2Sum += spo2;
      bpmSum += heartRate;
      return;
    }    
    //After gathering 25 new samples recalculate HR and SP02
    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
  }
  
}
