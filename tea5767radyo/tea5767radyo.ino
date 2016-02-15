

#include <LiquidCrystal_I2C.h>

#include <Wire.h>
#include <TM1637Display.h>
#include "RTClib.h"
#include <OneWire.h> 
RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27,16,2);
unsigned char frequencyH = 0;
unsigned char frequencyL = 0;
int DS18S20_Pin = 4; 
OneWire ds(DS18S20_Pin);  
unsigned int frequencyB;
double frequency = 0;
int sayac=0;
void setup()
{
  Wire.begin();
  frequency = 104.7; //starting frequency
  setFrequency();
  pinMode(2,INPUT_PULLUP);
  pinMode(3,INPUT_PULLUP);
  Serial.begin(9600);
  lcd.begin();
  rtc.begin();
    //  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); 
    delay(2000);

}

void loop()
{

 if(!(digitalRead(2)))
 {
  frequency+=0.1F;
   while(!(digitalRead(3))) {};
  setFrequency();
 }

  if(!(digitalRead(3)))
 {
  frequency-=0.1F;
   while(!(digitalRead(2))) {};
  setFrequency();
 }
 
Serial.println(frequency);
lcd.home();

lcd.print(frequency,2);
lcd.print("MHz ");
    DateTime now = rtc.now();
    lcd.setCursor(0,1);
    int h= now.hour();
    int m = now.minute();
    int s = now.second();
    char crap[80];
    sprintf(crap, "%02d:%02d:%02d", h, m, s);
    lcd.print(crap);
    delay(1);

  float temperature = getTemp();
  lcd.setCursor(10,0);
  lcd.print(temperature);
  lcd.print("C");
 delay(100);
}
void setFrequency()
{
  frequencyB = 4 * (frequency * 1000000 + 225000) / 32768;
  frequencyH = frequencyB >> 8;
  frequencyL = frequencyB & 0XFF;
  delay(100);
  Wire.beginTransmission(0x60);
  Wire.write(frequencyH);
  Wire.write(frequencyL);
  Wire.write(0xB0);
  Wire.write(0x10);
  Wire.write((byte)0x00);
  Wire.endTransmission();
  delay(100); 

} 


float getTemp(){
  
  byte data[12];
  byte addr[8];

  if ( !ds.search(addr)) {
      ds.reset_search();
      return -1000;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return -1000;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      Serial.print("Device is not recognized");
      return -1000;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end

  byte present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE); // Read Scratchpad

  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }

  ds.reset_search();

  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;

  return TemperatureSum;

}

