//Button test Code
//Susu Pelger

#include <Servo.h> //servo library
Servo myservo; //sets up servo
unsigned int bpress = 0;

//pointers for port b to light up LEDs and for motor - 
volatile unsigned char *port_b = (unsigned char*) 0x25;
volatile unsigned char *ddr_b = (unsigned char*) 0x24;
volatile unsigned char *pin_b = (unsigned char*) 0x23;

//pointers for port e - for disabled button
volatile unsigned char *port_e = (unsigned char*) 0x2E;
volatile unsigned char *ddr_e = (unsigned char*) 0x2D;
volatile unsigned char *pin_e = (unsigned char*) 0x2C;

unsigned int ventpin = 2; //pin vent button
unsigned int dispin = 3; //pin disable button

void setup() {
    // put your setup code here, to run once:
    myservo.attach(51); //servo connected to pin 51
    myservo.write(0); //servo set at 0°
    
    *ddr_b &= 0xFD; //sets PB1 (enable button) and PB3 (vent button) to input
    *port_b &= 0xF5; //sets PB1 and PB3 to low
    *ddr_b |= 0xF2; //sets LEDs as outputs
    *port_b &= 0x0F; //sets LEDs to low
    *ddr_b &= 0xFD; //sets PE5 as input
    *port_e |= 0x10; //sets PE5 high

    attachInterrupt(digitalPinToInterrupt(ventpin), venton, RISING); //pull-down resistor
    //attachInterrupt(digitalPinToInterrupt(dispin), disabled, LOW); //pull-up resistor
}

void loop() {
    // put your main code here, to run repeatedly:
    
    while (!(*pin_e & 0x10))
    {
        *port_b |= 0x20; //lights up yellow LED
        *port_b &= 0xBF; //green LED off
        *port_b &= 0xEF; //red LED off
    }
    
    *port_b |= 0x10; //lights up red LED
    *port_b &= 0xDF; //yellow LED off
    /*
    if((*pin_b & 0x02)) //if enable button pressed
    {
        *port_b |= 0x40; //lights up green LED
        *port_b &= 0xEF; //red LED off
        *port_b &= 0xDF; //yellow LED off
    }
    else //buttons not pressed
    {
        *port_b |= 0x10; //lights up red LED
        *port_b &= 0xBF; //green LED off
        *port_b &= 0xDF; //yellow LED off
    }*/

}

void venton() //ISR function when vent button is pressed
{
    //for(volatile unsigned int i = 0; i < 1000; i++); //eliminates noise
    //if (*pin_b & 0x08)
    //{
        myservo.write(90*bpress);// moves servo by 90 degrees each vent button press
        bpress++;
        if (bpress == 3) //change to needed number later
        {
            bpress = 0; //servo/vent will move back to beginning next vent button press
        }
    //}
}

void disabled() //ISR function when disabled button pressed
{
    *port_b |= 0x20; //lights up yellow LED
    *port_b &= 0xBF; //green LED off
    *port_b &= 0xEF; //red LED off
    
    while (!(*pin_e & 0x10)); //stays in interrupt until disabled button not pressed
    //need to fix this, unable to move vent in this state
    //maybe change this to a while loop in the main?
}
