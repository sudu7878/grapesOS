INFORMATION ABOUT THE PROJECT

    Thank you for having a look! As you might've guessed, I've used Assembly and C for this project (though I plan to use C++ afterwards).

    So, First things first, Raspberry Pi has mainly GPIO pins, upon which many's functions are reserved or predefined, for miniUART or UART, for 
    exapmple, we use GPIO 14 and GPIO 15 for TX and RX respectively. To initialize the interface, we need to use some control registers, which are
    virtually mapped in the pi's memory. So, thats the reason why the actual memory addresses differ from that the physical ones. This hierarchy is
    pretty much same for all the systems in the pi. 

    I've also tried to explain the functions of the code in the code itself, so you can check them out as well!

    Also this project is just for a hobby and it may/may not fail! (hope so not tho).

    Thanks!