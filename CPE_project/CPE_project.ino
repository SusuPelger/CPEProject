//CPE 301 - Project
//Uses water, temperature, and humidity sensors to control motor and LCD screen
//(Swamp Cooler Simulation)
//Written by Susu Pelger, Fall 2020

//LCD library
#include <LiquidCrystal.h>
LiquidCrystal lcd(23, 24, 25, 26, 27, 28); //initializes LCD

//DHT library
#include <DHT.h>
#define DHTPIN A1 //digital pin A1
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE); //initializes DHT sensor

//pointers for ADC
volatile unsigned char *my_ADMUX = (unsigned char*) 0x7C;
volatile unsigned char *my_ADCSRB = (unsigned char*) 0x7B;
volatile unsigned char *my_ADCSRA = (unsigned char*) 0x7A;
volatile unsigned int *my_ADC_DATA = (unsigned int*) 0x78;

//pointers for port b to light up LEDs and for motor - 
volatile unsigned char *port_b = (unsigned char*) 0x25;
volatile unsigned char *ddr_b = (unsigned char*) 0x24;
volatile unsigned char *pin_b = (unsigned char*) 0x23;

//adc vars
int adc_id = 0;
int Historyvalue = 0;
char printBuffer[128];

void setup()
{
    Serial.begin(9600);
    adc_init(); //sets up ADC
    lcd.begin(16, 2); //sets up LCD columns and rows
    dht.begin(); //sets up dht sensor
    *ddr_b |= 0xF1; //sets PB0 and 4-7 outputs
    *port_b &= 0x0E; //sets LEDs and motor to low for now
}

void loop()
{
    //interrupts should be used to obtain readings from the sensors
    //and switch between states

    //dht sensor
    float humid = dht.readHumidity(); //reads humidity
    float temp = dht.readTemperature(true); //reads temp in F
    if(isnan(humid) || isnan(temp))
    {
        Serial.println("no reading from DHT");
        return;
    }
    
    //water sensor
    unsigned int adc_reading = adc_read(adc_id); //for now gets reading from ADC
    if(((Historyvalue>=adc_reading) && ((Historyvalue - adc_reading) > 10)) || ((Historyvalue<adc_reading) && ((adc_reading - Historyvalue) > 10)))
    {
      sprintf(printBuffer,"ADC%d level is %d\n",adc_id, adc_reading);
      Serial.print(printBuffer);
      Historyvalue = adc_reading;
    }

    *port_b |= 0x10; //used to cycle through LEDs to high, rn lights up red LED

    //LCD code
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

//functions
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

unsigned int adc_read(unsigned char adc_channel)
{
    *my_ADMUX &= 0b11100000; //clear channel selection bits MUX 4-0
    *my_ADCSRB &= 0b11110111; //clear channel selection bit MUX 5
    
    *my_ADMUX |= adc_channel; //changes based on pin used
    
    *my_ADCSRA |= 0b01000000; //set bit 6 to 1 - starts conversion
    while ((*my_ADCSRA & 0x40) != 0); //wait for conversion to complete

    return *my_ADC_DATA; //return result in ADC data register
}
