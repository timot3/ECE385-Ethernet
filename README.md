# ECE385-Ethernet
ECE 385 Final Project -- Ethernet on MAX10-DE10 Lite FPGA


## Documentation

### To set pin number n: 

Set GPIO_PIO equal to 2^n to set that bit to high. For example, setting `*GPIO_PIO = 0x8` sets bit #4 to high, which in turn sets Arduino_IO3. 

<p align="center">
  <img src="https://cdn.discordapp.com/attachments/611409693134553126/908588278691790888/unknown.png">
</p>

You also need to add a GPIO module to platform designer with output pins equal to the number of GPIO pins.

In the top level, add the GPIO pins as output, pass them into the SoC, and set pins in pin planner to Arduino pins. The last step should be taken care of if using the Altera-provided pin layouts (make sure to use the right name and have the right number of GPIO pins!).