//Button test Code
//Susu Pelger

#include <Servo.h> //servo library
Servo myservo; //sets up servo
unsigned int bpress = 1;

//pointers for port b to light up LEDs and for motor - 
volatile unsigned char *port_b = (unsigned char*) 0x25;
volatile unsigned char *ddr_b = (unsigned char*) 0x24;
volatile unsigned char *pin_b = (unsigned char*) 0x23;

unsigned int ventpin = 2; //pin vent button
unsigned int dispin = 3; //pin disable button

bool disable = false;

unsigned int ventlastint = 0; //starting point for vent button
unsigned int dislastint = 0; //starting point for disable button

void setup() {
    // put your setup code here, to run once:
    myservo.attach(51); //servo connected to pin 51
    myservo.write(0); //servo set at 0°
    
    *ddr_b |= 0xF2; //sets LEDs as outputs
    *port_b &= 0x0F; //sets LEDs to low

    attachInterrupt(digitalPinToInterrupt(ventpin), venton, RISING); //pull-down resistor
    attachInterrupt(digitalPinToInterrupt(dispin), toggle, RISING); //pull-down resistor
}

void loop() {
    // put your main code here, to run repeatedly:

    if (disable == true)
    {
        *port_b |= 0x20; //lights up yellow LED
        *port_b |= 0x80; //lights up blue LED
        *port_b &= 0x2F; //!yellow LED off 0b0010 1111
    }
    else
    {
        *port_b |= 0x40; //lights up green LED
        *port_b |= 0x10; //RED
        //*port_b &= 0x4F; //!green LED off 0b0100 1111
        *port_b &= 0x5F; //0b0101
    }
}

void venton() //ISR function when vent button is pressed
{
    unsigned int newint = millis(); //records new point

    if (newint - ventlastint > 200) //debounces!
    {
        myservo.write(90*bpress);// moves servo by 90 degrees each vent button press
        bpress++;
        if (bpress == 3) //change to needed number later
        {
            bpress = 0; //servo/vent will move back to beginning next vent button press
        }
        ventlastint = newint;
    }
}

void toggle() //ISR function when disabled button pressed
{
    unsigned int newint = millis(); //records new point

    if (newint - dislastint > 200)
    {
        disable = !disable;
    }
    dislastint = newint;
}
