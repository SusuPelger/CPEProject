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
}

void loop() {
    // put your main code here, to run repeatedly:
    delay(1000);

    DateTime mtime = myRTC.now();
    
    Serial.print(mtime.year(), DEC);
    Serial.print('/');
    Serial.print(mtime.month(), DEC);
    Serial.print('/');
    Serial.print(mtime.day(), DEC);
    Serial.print(' ');
    Serial.print(mtime.hour(), DEC);
    Serial.print(':');
    Serial.print(mtime.minute(), DEC);
    Serial.print(':');
    Serial.print(mtime.second(), DEC);
    Serial.println();
}
