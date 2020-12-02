//Button test Code
//Susu Pelger

//pointers for port b to light up LEDs and for motor - 
volatile unsigned char *port_b = (unsigned char*) 0x25;
volatile unsigned char *ddr_b = (unsigned char*) 0x24;
volatile unsigned char *pin_b = (unsigned char*) 0x23;

void setup() {
    // put your setup code here, to run once:
    *ddr_b &= 0xFD; //sets PB1 (enable button) and PB3 (vent button) to input
    *ddr_b |= 0xF0; //sets LEDs as outputs
    *port_b &= 0x0F; //sets LEDs to low
    *port_b &= 0xF5; //sets PB1 and PB3 to low    
}

void loop() {
    // put your main code here, to run repeatedly:
    if((*pin_b & 0x02)) //if enable button pressed
    {
        *port_b |= 0x40; //lights up green LED
        *port_b &= 0xEF; //red LED off
        *port_b &= 0xDF; //yellow LED off
    }
    else if((*pin_b & 0x08)) //if vent button pressed
    {
        *port_b |= 0x20; //lights up yellow LED
        *port_b &= 0xEF; //red LED off
        *port_b &= 0xBF; //green LED off
    }
    else //buttons not pressed
    {
        *port_b |= 0x10; //lights up red LED
        *port_b &= 0xBF; //green LED off
        *port_b &= 0xDF; //yellow LED off
    }

}
