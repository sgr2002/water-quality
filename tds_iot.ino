


#define BLYNK_TEMPLATE_ID "TMPL3JGz_a_Kp" 
#define BLYNK_TEMPLATE_NAME "IOT" 
#define BLYNK_AUTH_TOKEN "pCIM_WVUDDVtBGD_Zzr6tScZ2eUW-8xx"

    
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp8266.h>
#include <OneWire.h>
#include <Wire.h> 
#include <DallasTemperature.h>
#include <Adafruit_ADS1X15.h>
#include <Adafruit_GFX.h>



#include <LiquidCrystal_I2C.h>    
LiquidCrystal_I2C lcd(0x27, 16, 2);
Adafruit_ADS1115 ads;
#define DS18B20 0  //Connect to GPIO2 pin
OneWire ourWire(DS18B20);
DallasTemperature sensor(&ourWire);

char auth[] = BLYNK_AUTH_TOKEN;


char ssid[] = "knocker";  // Enter your wifi name
char pass[] = "eighteights";  // Enter your wifi password

BlynkTimer timer;
int sensorPin = D5;
volatile long pulse;
unsigned long lastTime;
int pH_Value; 
float Voltage;


float calibration_value = 21.34 - 0.7;
int phval = 0; 
unsigned long int avgval; 
int buffer_arr[10],temp;


float volume,turbidity,tds,ph_act;

void sendSensor()
{
  // Request temperature to all devices on the data line
  sensor.requestTemperatures(); 
  
  //Serial.print("Celsius temperature: ");
  //We can connect more than one IC on the same data wire. 0 refers to the first IC on the wire
  //Serial.print(sensor.getTempCByIndex(0)); 
  //Serial.print(" - Fahrenheit temperature: ");
  //Serial.println(sensor.getTempFByIndex(0));
  int tempC=sensor.getTempCByIndex(0);
  int tempF=sensor.getTempFByIndex(0);
  delay(1000);
  
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
    Blynk.virtualWrite(V0, tempC);
    Blynk.virtualWrite(V1, turbidity);
    Blynk.virtualWrite(V2,volume);
    Blynk.virtualWrite(V3, tds);
    Blynk.virtualWrite(V4, ph_act);
    delay(500);
}
void setup()

{

  Blynk.begin(auth, ssid, pass);lcd.init();
   if (!ads.begin())
  {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }
  lcd.backlight();
   lcd.setCursor(0,0);
    lcd.print("welcome");
     lcd.setCursor(0,1);
    lcd.print("Intialzing");
    delay(200);
  pinMode(sensorPin, INPUT);
   Serial.begin(9600);
  sensor.begin();
  attachInterrupt(digitalPinToInterrupt(sensorPin), increase, RISING);

  
  timer.setInterval(100L, sendSensor);
 
  }

void loop()
{
  for(int i=0;i<10;i++) 
 { 
 buffer_arr[i]=analogRead(A0);
 delay(30);
 }
 for(int i=0;i<9;i++)
 {
 for(int j=i+1;j<10;j++)
 {
 if(buffer_arr[i]>buffer_arr[j])
 {
 temp=buffer_arr[i];
 buffer_arr[i]=buffer_arr[j];
 buffer_arr[j]=temp;
 }
 }
 }
 avgval=0;
 for(int i=2;i<8;i++)
 avgval+=buffer_arr[i];
 float volt=(float)avgval*5.0/1024/6; 
  ph_act = -5.70 * volt + calibration_value;

// Serial.print("pH Val: ");
 //Serial.println(ph_act);
 delay(1000);
 
 tds=0;

  volume = 2.663 * pulse / 1000 * 30;
  if (millis() - lastTime > 2000) {
    pulse = 0;
    lastTime = millis();
  }
  // Serial.print(volume);
  //Serial.println(" L/m");
  int16_t adc0, adc1, adc2, adc3;
  float volts0, volts1, volts2, volts3;
int A;
float v;
   ads.setGain(GAIN_ONE);  
  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);
  adc3 = ads.readADC_SingleEnded(3);

 
  volts0 = ads.computeVolts(adc0);
  volts1 = ads.computeVolts(adc1);
  volts2 = ads.computeVolts(adc2);
  volts3 = ads.computeVolts(adc3);
float volts0real=volts0*1.87;
turbidity=-101.47*volts0real+497.95;

Serial.print(volts0);
 

float ec,ec25;
//if(volts2>0.75)
//{


ec=(133.42*volts2*volts2*volts2-255.86*volts2*volts2+857.39*volts2)/3;
ec25=ec/(1.0+0.02*(sensor.getTempCByIndex(0)-25.0));
tds=ec25*0.5;
//}
  lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("flow:");
    lcd.print(volume);
    lcd.print(" l/m");

    lcd.setCursor(0,1);
    lcd.print("temp:");
    lcd.print(sensor.getTempCByIndex(0));
    lcd.print(" c");
 delay(1000);
 lcd.clear();
    lcd.setCursor(0,0);
     lcd.print(" pH:");
    lcd.print(ph_act);
   
 
    lcd.setCursor(0,1);
    lcd.print("turbty:");
    lcd.print(turbidity);
    lcd.print("NTU");
     delay(1000);
     lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("TDS:");
    lcd.print(tds);
    lcd.print("PPM");
    float tempd=sensor.getTempCByIndex(0);
    if(tds>=50&&tds<=200&&turbidity<=7&&turbidity>=1&&ph_act<=8.5&&ph_act>=6.5&&tempd<=35.00&&tempd>=25.00&&volume>=19.5&&volume<=45.45)
    {
      lcd.setCursor(0,1);
    lcd.print("Drinkable");
    }
    lcd.setCursor(0,1);
    lcd.print("Non-Drinkable");
    
  Blynk.run();
  timer.run();
  
  //Serial.println(Voltage);
  
   //Serial.println(volts0);
   
  
}
ICACHE_RAM_ATTR void increase() {
  pulse++;
  
    
}
