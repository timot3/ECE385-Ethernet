//int main() {
//    int i = 0;
//    volatile unsigned int *LED_PIO = (unsigned int*)0x60; //make a pointer to access the PIO block
//    volatile unsigned int *GPIO_PIO = (unsigned int*)0x40;
//
//    *LED_PIO = 0; //clear all LEDs
//    while ( (1+1) != 3) //infinite loop
//    {
//        for (i = 0; i < 100000; i++); //software delay
//        *LED_PIO |= 0x1; //set LSB
//        *GPIO_PIO = 1;
//        printf("SET\n");
//        for (i = 0; i < 100000; i++); //software delay
//        *LED_PIO &= ~0x1; //clear LSB
//        *GPIO_PIO = 0;
//        printf("CLEAR\n");
//    }
//    return 1; //never gets here
//}
