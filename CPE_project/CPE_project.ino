//CPE 301 - Project
//Uses water, temperature, and humidity sensors to control motor and LCD screen
//Buttons to control idle state and vent
//(Evaporation Cooler Simulation)
//Written by Susu Pelger, Fall 2020

//LIBRARIES
//LCD library
#include <LiquidCrystal.h>
LiquidCrystal lcd(23, 24, 25, 26, 27, 28); //(RS, E, D4, D5, D6, D7) initializes LCD screen

//DHT library
#include <DHT.h>
#define DHTPIN A1 //digital pin A1
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE); //initializes DHT sensor

//Servo library
#include <Servo.h>
Servo myservo; //sets up servo
unsigned int bpress = 1;

//Clock libraries
#include <Wire.h> //allows communication with SDA and SCL pins
#include <DS3231.h> //RTC library

//REGISTER POINTERS
//pointers for ADC
volatile unsigned char *my_ADMUX = (unsigned char*) 0x7C;
volatile unsigned char *my_ADCSRB = (unsigned char*) 0x7B;
volatile unsigned char *my_ADCSRA = (unsigned char*) 0x7A;
volatile unsigned int *my_ADC_DATA = (unsigned int*) 0x78;

//pointers for port b to light up LEDs
volatile unsigned char *port_b = (unsigned char*) 0x25;
volatile unsigned char *ddr_b = (unsigned char*) 0x24;
volatile unsigned char *pin_b = (unsigned char*) 0x23;

//pointers for port h for DC motor
volatile unsigned char *port_h = (unsigned char*) 0x102;
volatile unsigned char *ddr_h = (unsigned char*) 0x101;
volatile unsigned char *pin_h = (unsigned char*) 0x100;

//VARIABLES
//DHT variables
float humid = 0;
float temp = 0;

//interrupt variables
unsigned int ventpin = 2; //pin vent button
unsigned int dispin = 3; //pin disable button

//temperature and water levels
unsigned int watermin = 150; //minimum water level is 150
float tempmax = 75.00; //maximum temperature is 75.00 degrees F

//adc vars
int adc_id = 0;
int Historyvalue = 0;
char printBuffer[128];
unsigned int adc_reading = 0;

//state variables
bool disable = false;
bool lcdclr = false;
bool fanon = false;

//clock variables
DS3231 clock;
RTClib myRTC;

void setup()
{
    Serial.begin(9600);
    adc_init(); //sets up ADC
    lcd.begin(16, 2); //sets up LCD columns and rows
    dht.begin(); //sets up dht sensor
    Wire.begin(); //sets up clock pins
    *ddr_b |= 0xF0; //sets 4-7 (LEDs) outputs
    *port_b &= 0x0F; //sets LEDs to low for now
    *ddr_h |= 0x18; //sets PB3 and PB4 to outputs (motor)
    *port_h &= 0xE7; //sets motor to low

    myservo.attach(51); //servo connected to pin 51
    myservo.write(0); //servo set at 0°

    attachInterrupt(digitalPinToInterrupt(ventpin), venton, RISING); //pull-down resistor
    attachInterrupt(digitalPinToInterrupt(dispin), disabled, CHANGE); //pull-down resistor
}

void loop()
{
    if (disable == true) //first checks if disable button is pressed
    {
        *port_b |= 0x20; //lights up yellow LED
        *port_b &= 0x2F; //!yellow LED off
        if ((*pin_h & 0x10))
            {
                timeoff(); //prints time if motor is turned off
                *port_h &= 0xEF; //turns fan motor off (enable off)
            }
        //*port_h &= 0xEF; //turns fan motor off (enable off)

    }
    else //otherwise
    {
        *port_b &= 0xDF; //yellow LED off
        
        //first read sensors, then use readings to decide what state
        dhtsense(); //reads dht
        watersense(); //reads water sensor

        //check for other states
        if (adc_reading < watermin) //if water level is too low, goes into error state
        {
            //errorstate();
            *port_b |= 0x10; //lights up red LED
            *port_b &= 0x1F; //!red LED off
            if ((*pin_h & 0x10))
            {
                timeoff(); //prints time if motor is turned off
                *port_h &= 0xEF; //turns fan motor off (enable off)
            }
            //*port_h &= 0xEF; //turns fan motor off (enable off)
    
            //error message displayed on LCD
            if (lcdclr == true)
                {
                    lcd.clear(); //clears if temp and humidity were showing
                    lcdclr = false;
                }
            lcd.setCursor (0,0); //row 1
            lcd.print("ERROR: REFILL");
   
            lcd.setCursor (0,1); //row 2
            lcd.print("WATER LEVEL");
        }
        else if (temp > tempmax) //if temp is too high, goes into running state
        {
            lcdth(); //prints temperature and humidity on LCD screen
            lcdclr = true;
            
            *port_b |= 0x80; //lights up blue LED
            *port_b &= 0x8F; //!blue LED off
            //turns fan motor on
            *port_h |= 0x10; //turns enable on
            *port_h |= 0x08; //turns motor on
            if (fanon == false)
            {
                timeon();
            }
        }
        else //goes into idle state
        {
            lcdth(); //prints temperature and humidity on LCD screen
            lcdclr = true;
            *port_b |= 0x40; //lights up green LED
            *port_b &= 0x4F; //!green LED off
            if ((*pin_h & 0x10))
            {
                timeoff(); //prints time if motor is turned off
                *port_h &= 0xEF; //turns fan motor off (enable off)
            }
            //*port_h &= 0xEF; //turns fan motor off (enable off)
        }
    }
}

//FUNCTIONS
//sets up registers for ADC
void adc_init()
{
    //setup A register
    *my_ADCSRA |= 0b10000000; //set bit 7 to 1 - enable ADC
    *my_ADCSRA &= 0b11011111; //clear bit 5 to 0 - disable ADC trigger mode
    *my_ADCSRA &= 0b11110111; //clear bit 3 to 0 - disable ADC interrupt
    *my_ADCSRA &= 0b11111000; //clear bit 5 to 0 - set prescalar selection to slow reading
    
    //setup B register
    *my_ADCSRB &= 0b11110111; //clear bit 3 to 0 - reset channel and gain bits
    *my_ADCSRB &= 0b11111000; //clear bits 0-2 to 0 - set free running mode
    
    //setup MUX register
    *my_ADMUX &= 0b01111111; //clear bit 7 to 0 - AVCC analog reference
    *my_ADMUX |= 0b01000000; //set bit 6 to 1 - AVCC analog reference
    *my_ADMUX &= 0b11011111; //clear bit 5 to 0 - right adjust result
    *my_ADMUX &= 0b11100000; //clear bits 0-4 to 0 - reset channel and gain bits
}

//reads Analog data
unsigned int adc_read(unsigned char adc_channel)
{
    *my_ADMUX &= 0b11100000; //clear channel selection bits MUX 4-0
    *my_ADCSRB &= 0b11110111; //clear channel selection bit MUX 5
    
    *my_ADMUX |= adc_channel; //changes based on pin used
    
    *my_ADCSRA |= 0b01000000; //set bit 6 to 1 - starts conversion
    while ((*my_ADCSRA & 0x40) != 0); //wait for conversion to complete

    return *my_ADC_DATA; //return result in ADC data register
}

//DHT reading
void dhtsense()
{
    humid = dht.readHumidity(); //reads humidity
    temp = dht.readTemperature(true); //reads temp in F
    if(isnan(humid) || isnan(temp)) //if the DHT doesn't receive readings, will send error
    {
        Serial.println("no reading from DHT");
        return;
    }
}

//water sensor reading
void watersense()
{
    adc_reading = adc_read(adc_id); //for now gets reading from ADC
    if(((Historyvalue>=adc_reading) && ((Historyvalue - adc_reading) > 10)) || ((Historyvalue<adc_reading) && ((adc_reading - Historyvalue) > 10)))
    {
        sprintf(printBuffer,"ADC%d level is %d\n",adc_id, adc_reading);
        Serial.print(printBuffer);
        Historyvalue = adc_reading;
    }
}

//LCD screen, prints temp and humidity
void lcdth()
{
    lcd.setCursor (0,0); //row 1
    lcd.print("Temp: ");
    lcd.print(temp); //prints temperature
    lcd.print((char)223);
    lcd.print("F");
    
    lcd.setCursor (0,1); //row 2
    lcd.print("Humidity: ");
    lcd.print(humid); //prints humidity
    lcd.print("%");
}

//prints time when fan turned on
void timeon()
{
    fanon = true;
    DateTime now = myRTC.now();
    
    Serial.print("Fan on at Date: ");
    Serial.print(now.year()); //prints year
    Serial.print('/');
    Serial.print(now.month()); //prints month
    Serial.print('/');
    Serial.print(now.day()); //prints day
    Serial.print(" Time: ");
    Serial.print(now.hour()); //prints hour
    Serial.print(':');
    Serial.print(now.minute()); //prints minute
    Serial.print(':');
    Serial.print(now.second()); //prints second
    Serial.println();
}

//prints time when fan turned off
void timeoff()
{
    fanon = false;
    DateTime now = myRTC.now();
    
    Serial.print("Fan off at Date: ");
    Serial.print(now.year()); //prints year
    Serial.print('/');
    Serial.print(now.month()); //prints month
    Serial.print('/');
    Serial.print(now.day()); //prints day
    Serial.print(" Time: ");
    Serial.print(now.hour()); //prints hour
    Serial.print(':');
    Serial.print(now.minute()); //prints minute
    Serial.print(':');
    Serial.print(now.second()); //prints second
    Serial.println();
}

//INTERRUPTS
void venton() //ISR function when vent button is pressed
{
    unsigned int lastint = 0; //starting point
    unsigned int newint = millis(); //records new point

    if (newint - lastint > 200) //debounces - checks for noise
    {
        myservo.write(90*bpress);// moves servo by 90 degrees each vent button press
        bpress++;
        if (bpress == 3) //change to needed number later
        {
            bpress = 0; //servo/vent will move back to beginning next vent button press
        }
    }
}

void disabled() //ISR function when disabled button pressed
{
    unsigned int lastint = 0; //starting point
    unsigned int newint = millis(); //records new point

    if (newint - lastint > 200) //debounces - checks for noise
    {
        disable = !disable;
    }
}
