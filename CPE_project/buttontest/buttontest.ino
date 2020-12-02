//Button test Code
//Susu Pelger

//pointers for port b to light up LEDs and for motor - 
volatile unsigned char *port_b = (unsigned char*) 0x25;
volatile unsigned char *ddr_b = (unsigned char*) 0x24;
volatile unsigned char *pin_b = (unsigned char*) 0x23;

void setup() {
    // put your setup code here, to run once:
    *ddr_b &= 0xFD; //sets PB1 (enable button) to input 0b1111 1101
    *ddr_b |= 0xF0; //sets LEDs as outputs
    *port_b &= 0x0F; //sets LEDs to low
    *port_b &= 0xFD; //sets PB1 to low    
}

void loop() {
    // put your main code here, to run repeatedly:
    if((*pin_b & 0x02)) //if button pressed
    {
        *port_b |= 0x40; //lights up green LED
    }
    else //button is not pressed
    {
        *port_b |= 0x10; //lights up red LED
    }

}
