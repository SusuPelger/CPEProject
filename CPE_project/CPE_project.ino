//CPE 301 - Project
//Uses water, temperature, and humidity sensors to control motor and LCD screen
//(Swamp Cooler Simulation)
//Written By Theresa Belleza, Michael Chavez, and Susu Pelger, Fall 2020

//pointers for ADC
volatile unsigned char *my_ADMUX = (unsigned char*) 0x7C;
volatile unsigned char *my_ADCSRB =  (unsigned char*) 0x7B;
volatile unsigned char *my_ADCSRA = (unsigned char*) 0x7A;
volatile unsigned int *my_ADC_DATA = (unsigned int*) 0x78;

//pointers for port b to light up LEDs and for motor
volatile unsigned char *port_b = (unsigned char*) 0x25;
volatile unsigned char *ddr_b = (unsigned char*) 0x24;
volatile unsigned char *pin_b = (unsigned char*) 0x23;

//pointers for port a for LCD screen
volatile unsigned char *port_a = (unsigned char*) 0x22;
volatile unsigned char *ddr_a = (unsigned char*) 0x21;
volatile unsigned char *pin_a = (unsigned char*) 0x20;

//pointers for the EEPROM registers to save data
volatile unsigned int* EEPROM_ADDR_REG = (unsigned int*) 0x41; //eeprom address register
volatile unsigned char* EEPROM_DATA_REG = (unsigned char*) 0x40; //eeprom data register
volatile unsigned char* EEPROM_CNTRL_REG = (unsigned char*) 0x3F; //eeprom control regiser
unsigned int address = 0x0025;    // random address to save counter to

void setup()
{
    adc_init() //sets up ADC
}

void loop()
{
    //interrupts should be used to obtain readings from the sensors
    //and switch between states

    unsigned int adc_reading = adc_read(0); //for now gets reading from ADC
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
