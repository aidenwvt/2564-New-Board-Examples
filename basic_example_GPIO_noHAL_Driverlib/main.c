// This application uses button 2 and LED 1
// The operation is such that if you press the right button, as long as you keep the button pressed,
// the LED is on. As soon as you release the button, LED is off and stays off until the button is pressed again.
// This example uses Driverlib functions, so it is easier than using memory-mapped registers directly.
// However, it does not build a Hardware Abstraction Layer (HAL), and therefore, the code is not easy to read and impossible to port.

#include <ti/driverlib/driverlib.h>

#define POWER_STARTUP_DELAY 16
#define PRESSED 0

int main(void) {
    //Clear any current instructions from the pins
    DL_GPIO_reset(GPIOA);
    DL_GPIO_reset(GPIOB);

    //Power on ports A and B so they listen to instructions
    DL_GPIO_enablePower(GPIOA);
    DL_GPIO_enablePower(GPIOB);

    //Always delay after enabling power to give the board time to configure
    DL_Common_delayCycles(POWER_STARTUP_DELAY);

    //Initiates the Launchpad LED1 (LL1) as an input
    DL_GPIO_initDigitalOutput(IOMUX_PINCM1); //Connects port A, pin 0 to the GPIO
    DL_GPIO_enableOutput(GPIOA, DL_GPIO_PIN_0); //Turns on port A, pin 0 as an output

    //Defines the button as a pull-up resistor and connects port B, pin 21 to the GPIO
    DL_GPIO_initDigitalInputFeatures(IOMUX_PINCM49,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
    
    while (1) {
        //Reads the pin voltage thats connected to Launchpad Right Button (LB2) to see if the button has been pressed
        if (DL_GPIO_readPins(GPIOB, DL_GPIO_PIN_21) == PRESSED)
            DL_GPIO_clearPins(GPIOA, DL_GPIO_PIN_0); //Turns on Launchpad LED1 (LL1) since its active low
        else
            DL_GPIO_setPins(GPIOA, DL_GPIO_PIN_0); //Turns off Launchpad LED1 (LL1) since its active low
    }
}
