/* This application illustrates a simple use of interrupts for GPIO
 * A transition on LB2 turns LL1 on */

#include <ti/driverlib/driverlib.h>

void DL_init();

int main()
{
    DL_init();

    /*
     * Turn OFF LED if SW is open, ON if SW is closed.
     * LED starts OFF by default.
     */

    NVIC_EnableIRQ(GPIOB_INT_IRQn);

    while (1) {
        __WFI();
    }
}

void GROUP1_IRQHandler()
{
    switch (DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1)) {
        case DL_INTERRUPT_GROUP1_IIDX_GPIOB:
            /* If SW is high, turn the LED off */
            if (DL_GPIO_readPins(GPIOB, DL_GPIO_PIN_21))
                DL_GPIO_setPins(GPIOA, DL_GPIO_PIN_0);
            /* Otherwise, turn the LED on */
            else
                DL_GPIO_clearPins(GPIOA, DL_GPIO_PIN_0);
            break;
    }
}

void DL_init() {
    DL_GPIO_reset(GPIOA);
    DL_GPIO_reset(GPIOB);

    DL_GPIO_enablePower(GPIOA);
    DL_GPIO_enablePower(GPIOB);
    delay_cycles(POWER_STARTUP_DELAY);

    DL_GPIO_initDigitalOutput(IOMUX_PINCM1);

    DL_GPIO_initDigitalInputFeatures(IOMUX_PINCM49,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_setPins(GPIOA, DL_GPIO_PIN_0);
    DL_GPIO_enableOutput(GPIOA, DL_GPIO_PIN_0);
    DL_GPIO_setUpperPinsPolarity(GPIOB, GPIO_POLARITY31_16_DIO21_RISE_FALL);
    DL_GPIO_clearInterruptStatus(GPIOB, DL_GPIO_PIN_21);
    DL_GPIO_enableInterrupt(GPIOB, DL_GPIO_PIN_21);
}
