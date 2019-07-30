#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <MQ2.h>
#include <Wire.h>


// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "TH0herQH8VrsQmkL8uVsocmb_xWNPdzQ";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "6";
char pass[] = "passwordlega";

#define DHTPIN 2          // What digital pin we're connected to

// Uncomment whatever type you're using!
#define DHTTYPE DHT11     // DHT 11
//#define DHTTYPE DHT22   // DHT 22, AM2302, AM2321
//#define DHTTYPE DHT21   // DHT 21, AM2301

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

//change this with the pin that you use
int pin = A0;
int lpg, co, smoke;

MQ2 mq2(pin);

//PIR pin 
#define ledPin D7 
#define pirPin D1
int pirValue;

// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V5, h);
  Blynk.virtualWrite(V6, t);
}

void setup()
{
  // Debug console
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  dht.begin();

  // Setup a function to be called every second
  timer.setInterval(1000L, sendSensor);

  mq2.begin();
  timer.setInterval(2000L, gas);

  pinMode(ledPin, OUTPUT);
  pinMode(pirPin, INPUT);
  digitalWrite(ledPin, LOW);
}

//for MQ2 sensor---------------------------------------------------------------------------------------------

void gas() {

  /*read the values from the sensor, it returns
    an array which contains 3 values.
    1 = LPG in ppm
    2 = CO in ppm
    3 = SMOKE in ppm
  */
  float* values = mq2.read(true); //set it false if you don't want to print the values in the Serial

  //lpg = values[0];
  lpg = (mq2.readLPG()-2147483648.00)/1000000;
  Blynk.virtualWrite(1, lpg);
  //co = values[1];
  co = (mq2.readCO()-2147483648.00)/1000000;
  Blynk.virtualWrite(2, co);
  //smoke = values[2];
  smoke = (mq2.readSmoke()-2147483648.00)/1000000;
  Blynk.virtualWrite(3, smoke);
}

//for PIR sensor-----------------------------------------------------------------------------------------------

void getPirValue(void)
{
  pirValue = digitalRead(pirPin);
  if (pirValue) 
  { 
    Serial.println("==> Motion detected");
    Blynk.notify("T==> Motion detected");  
  }
  digitalWrite(ledPin, pirValue);
}

void loop()
{
  getPirValue();
  Blynk.run();
  timer.run();
}
