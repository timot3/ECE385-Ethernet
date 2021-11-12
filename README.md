# ECE385-Ethernet
ECE 385 Final Project -- Ethernet on MAX10-DE10 Lite FPGA


To set a pin: 

Here is how to set pin number n: set GPIO_PIO equal to 2^n to set that bit to high. For example, setting `*GPIO_PIO = 0x8` sets bit #4 to high, which in turn sets IO3. [image0.png](https://cdn.discordapp.com/attachments/611409693134553126/908588278691790888/unknown.png)

You also need to add a GPIO module to platform designer with output pins equal to the number of GPIO pins.

In the top level, add the GPIO pins as output, pass them into the SOC, and set pins in pin planner to arduino pins. The last step should be taken care of if using the Altera-provided pin layouts (make sure to use the right name and have the right number of GPIO pins!).