//RTC code - tests for clock portion
//Susu Pelger

#include <Wire.h> //allows communication with SDA and SCL pins
#include <DS3231.h> //RTC library

DS3231 clock;
RTClib myRTC;

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    Serial.println("Initialize RTC module");
    Wire.begin();

    //only run this once to set time correctly
    //clock.setDate(5);
    //clock.setHour(16);
    //clock.setMinute(17);
    //clock.setSecond(20);
}

void loop() {
    // put your main code here, to run repeatedly:
    delay(1000);

    DateTime now = myRTC.now();
    
    Serial.print("Fan on at Date: ");
    Serial.print(now.year());
    Serial.print('/');
    Serial.print(now.month());
    Serial.print('/');
    Serial.print(now.day());
    Serial.print(" Time: ");
    Serial.print(now.hour());
    Serial.print(':');
    Serial.print(now.minute());
    Serial.print(':');
    Serial.print(now.second());
    Serial.println();
}
