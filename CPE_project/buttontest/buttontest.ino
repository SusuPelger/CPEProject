//Button test Code
//Susu Pelger

//pointers for port b to light up LEDs and for motor - 
volatile unsigned char *port_b = (unsigned char*) 0x25;
volatile unsigned char *ddr_b = (unsigned char*) 0x24;
volatile unsigned char *pin_b = (unsigned char*) 0x23;

void setup() {
    // put your setup code here, to run once:
    *ddr_b &= 0xF7; //sets PB3 (enable button) to input 0b1111 0111
    *ddr_b |= 0xF0; //sets LEDs as outputs
    *port_b &= 0x0F; //sets LEDs to low
    *port_b &= 0xF7; //sets PB3 to low    
}

void loop() {
    // put your main code here, to run repeatedly:
    if((*pin_b & 0x08)) //if button pressed 0b0000 1000
    {
        *port_b |= 0x40; //lights up green LED
        *port_b &= 0xEF; //red LED off
    }
    else //button is not pressed
    {
        *port_b |= 0x10; //lights up red LED
        *port_b &= 0xBF; //green LED off
    }

}
