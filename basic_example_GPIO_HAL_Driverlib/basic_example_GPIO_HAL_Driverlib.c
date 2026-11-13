// This application uses launchpad button 2 (LB2) and launchpad LED 1 (LL1)
// The operation is such that if you press the right button, as long as you keep the button pressed,
// the LED is on. As soon as you release the button, LED is off and stays off until the button is pressed again.
// This example uses Driverlib functions. It also builds a HAL.
// It is easy to read (due to HAL) and easy to write(due to Driverlib)!!

#include <ti/driverlib/driverlib.h>

// This function initializes all the peripherals
void initializeGPIO();

bool LB2_isPressed();
void LL1_turnOn();
void LL1_turnOff();

#define POWER_STARTUP_DELAY 16
#define PRESSED 0

int main() {
    initializeGPIO(); //We create an initialize function as part of the HAL protocol
    
    while (1) {
        //We define helper functions so its more intuitive for a human user
        //trying to work with launchpad hardware configuration
        if (LB2_isPressed())
            LL1_turnOn();
        else
            LL1_turnOff();
    }
}

// The HAL itself is written using Driverlib, so it is much easier to implement.
void initializeGPIO() {
    //Clear any current instructions from the pins
    DL_GPIO_reset(GPIOA);
    DL_GPIO_reset(GPIOB);

    //Power on ports A and B so they listen to instructions
    DL_GPIO_enablePower(GPIOA);
    DL_GPIO_enablePower(GPIOB);

    //Always delay after enabling power to give the board time to configure
    DL_Common_delayCycles(POWER_STARTUP_DELAY);

    //Initiates the Launchpad LED1 (LL1) as an output
    DL_GPIO_initDigitalOutput(IOMUX_PINCM1); //Connects port A, pin 0 to the GPIO
    DL_GPIO_enableOutput(GPIOA, DL_GPIO_PIN_0); //Turns on port A, pin 0 as an output

    //Defines the button as a pull-up resistor and connects port B, pin 21 to the GPIO
    DL_GPIO_initDigitalInputFeatures(IOMUX_PINCM49,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
}

bool LB2_isPressed() {
    return DL_GPIO_readPins(GPIOB, DL_GPIO_PIN_21) == PRESSED;
}

void LL1_turnOn() {
    DL_GPIO_clearPins(GPIOA, DL_GPIO_PIN_0);
}

void LL1_turnOff() {
    DL_GPIO_setPins(GPIOA, DL_GPIO_PIN_0);
}