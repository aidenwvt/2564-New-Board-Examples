/* This application uses launchpad button 2 (LB2) and launchpad LED 1 (LL1)
 * The operation is such that if you press the right button, as long as you keep the button pressed,
 * the LED is on. As soon as you release the button, LED is off and stays off until the button is pressed again.
 * This example use memory-mapped registers to communicate with GPIO peripherals.
 * This example doesn't use any Driverlib function. However, it does build a Hardware Abstraction Layer (HAL).
 * The main function is easy to read, but the HAL functions themselves are hard to write since we do not use Driverlib. */

#include "stdbool.h" // Called to pass through the bool type when DriverLib isn't used

// This function initializes all the peripherals
void initializeGPIO();

bool LB2_isPressed();
void LL1_turnOn();
void LL1_turnOff();

#define POWER_STARTUP_DELAY 16
#define PRESSED 0

int main() {
	initializeGPIO();
    
    while (1) {
        /* Read the pin voltage thats connected to Launchpad Right Button (LB2) to see if the button has been pressed
         * Clearing the LED turns it on here since LL1 is active low */
        if (LB2_isPressed())
            LL1_turnOn(); // Turn on Launchpad LED1 (LL1)
        else
            LL1_turnOff(); // Turn off Launchpad LED1 (LL1)
    }
}

void initializeGPIO() {
    unsigned int *GPIOA_RSTCTL_address;
    unsigned int *GPIOB_RSTCTL_address;
    unsigned int *GPIOA_PWREN_address;
    unsigned int *GPIOB_PWREN_address;
    unsigned int *PINCM1_address;
    unsigned int *GPIOA_DOESET_address;
    unsigned int *PINCM49_address;

    /* Refer to the Luachpad MSPM0G350x Datasheet on page 58 for register base addresses
	 * Refer to the MSPM0 TRM on pages 663, 671 for register offsets */
    GPIOA_RSTCTL_address  = (unsigned int *)0x400A0804;
    GPIOB_RSTCTL_address  = (unsigned int *)0x400A2804;
    GPIOA_PWREN_address   = (unsigned int *)0x400A0800;
    GPIOB_PWREN_address   = (unsigned int *)0x400A2800;
    PINCM1_address        = (unsigned int *)0x40428004;
    GPIOA_DOESET_address  = (unsigned int *)0x400A12D0;
    PINCM49_address       = (unsigned int *)0x404280C4;

    // Clear any current instructions from the pins
    *GPIOA_RSTCTL_address = 0xB1000003;
    *GPIOB_RSTCTL_address = 0xB1000003;

    // Power on ports A and B so they listen to instructions
    *GPIOA_PWREN_address = 0x26000001;
    *GPIOB_PWREN_address = 0x26000001;

    // Always add delay after enabling power to give the board time to configure
    for (volatile int i = 0; i < POWER_STARTUP_DELAY; i++);

    // Initiate the Launchpad LED1 (LL1) as an output
    *PINCM1_address = 0x81;         //Connect port A, pin 0 to the GPIO
    *GPIOA_DOESET_address = 1 << 0; //Define port A, pin 0 as an output

    /* Define the button as a pull-up resistor and connect port B, pin 21 to the GPIO
     * GPIO functionality is always a pin function of 0x00000001 */
    *PINCM49_address = 0x00060081;
}

bool LB2_isPressed() {
    unsigned int *GPIOB_DIN_address;
    GPIOB_DIN_address = (unsigned int *)0x400A3380;

    return (*GPIOB_DIN_address & (1 << 21)) == PRESSED;
}

void LL1_turnOn() {
    unsigned int *GPIOA_DOUTCLR_address;
    GPIOA_DOUTCLR_address = (unsigned int *)0x400A12A0;

    *GPIOA_DOUTCLR_address = 1 << 0;
}

void LL1_turnOff() {
    unsigned int *GPIOA_DOUTSET_address;
    GPIOA_DOUTSET_address = (unsigned int *)0x400A1290;

	*GPIOA_DOUTSET_address = 1 << 0;
}