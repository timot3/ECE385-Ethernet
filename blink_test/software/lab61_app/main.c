//// Main.c - makes LEDG0 on DE2-115 board blink if NIOS II is set up correctly
//// for ECE 385 - University of Illinois - Electrical and Computer Engineering

//
//int main() {
//    volatile unsigned int *LED_PIO = (unsigned int*)0x60; //make a pointer to access the PIO block
//    volatile unsigned int *SWITCHES = (unsigned int*)0x40; //make a pointer to access the PIO block
//    volatile unsigned int *KEY = (unsigned int*)0x30; //make a pointer to access the PIO block
//
//    volatile int counter = 0;
//    int isPressed = 0;
//
//    *LED_PIO = 0; //clear all LEDs
//    while(1) {
//    	// key_0 and key_1 have different values, active low:
//    	// key_0 is our reset, has value 2.
//    	// key_1 is our accumulate, has value 1.
//    	// when neither of them are pressed, the value is 3
//    	volatile int keySum = KEY[0] + KEY[1];
//    	if(keySum == 1 && !isPressed) {
//    		isPressed = 1;
//    		counter += *SWITCHES;
//			counter %= 256;
//    	} else if(keySum == 0) {
//    		counter = 0;
//    		isPressed = 0;
//    	} else if(keySum == 3) {
//    		isPressed = 0;
//    	}
//        *LED_PIO = counter;
//    }
//    return 1; //never gets here
//}
//
// Main.c - makes LEDG0 on DE2-115 board blink if NIOS II is set up correctly
// for ECE 385 - University of Illinois - Electrical and Computer Engineering
// Author: Zuofu Cheng

int main()
{
    int i = 0;
    volatile unsigned int *LED_PIO = (unsigned int*)0x60; //make a pointer to access the PIO block

    *LED_PIO = 0; //clear all LEDs
    while ( (1+1) != 3) //infinite loop
    {
        for (i = 0; i < 100000; i++); //software delay
        *LED_PIO |= 0x1; //set LSB
        for (i = 0; i < 100000; i++); //software delay
        *LED_PIO &= ~0x1; //clear LSB
    }
    return 1; //never gets here
}
