//ADC code for CPE 301 project - this is for the water sensor so far, still need to implement DHT
//Susu Pelger

//pointers for ADC
volatile unsigned char *my_ADMUX = (unsigned char*) 0x7C;
volatile unsigned char *my_ADCSRB = (unsigned char*) 0x7B;
volatile unsigned char *my_ADCSRA = (unsigned char*) 0x7A;
volatile unsigned int *my_ADC_DATA = (unsigned int*) 0x78;

int adc_id = 1;
int HistoryValue = 0;
char printBuffer[128];

void setup()
{
    adc_init(); //sets up ADC
    Serial.begin(9600);
}

void loop()
{
    //unsigned int adc_reading_w = adc_read(0); //gets reading for water sensor
    //Serial.print(adc_reading_w);

    int value = analogRead(adc_id); // get adc value

    if(((HistoryValue>=value) && ((HistoryValue - value) > 10)) || ((HistoryValue<value) && ((value - HistoryValue) > 10)))
    {
      sprintf(printBuffer,"ADC%d level is %d\n",adc_id, value);
      Serial.print(printBuffer);
      HistoryValue = value;
    }
}

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
