/*
 * This is a simple program to introduce UART.
 * Pushing S2 on the Launchpad toggles the LED2 on the Launchpad at any time.
 * The program gets characters sent from your Laptop and decides whether it is Number, Letter or Other. Then,
 * it sends back N, L or O to the laptop in response.
 * Sending lower case c from laptop changes the baudrate.
 * The system clock for this project is 3MHz and all the UART calcuations are based on this number.
 */
#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>
#include "HAL/pins.h"

typedef enum {baud9600, baud19200, baud38400, baud57600} UARTBaudRate_t;

void TurnOn_Launchpad_LED2();
void TurnOff_Launchpad_LED2();
char S2isPressed();
void InitializeHardware();

int main(void) {
    InitializeHardware();

    // Setting up variables to use the clock and UART
    DL_UART_Main_ClockConfig uartClock;
    DL_UART_Main_Config uartConfig;

    // Related to baudrate generation and sampling
    uartClock.clockSel = DL_UART_MAIN_CLOCK_BUSCLK;
    uartClock.divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1;
    uartConfig.mode = DL_UART_MAIN_MODE_NORMAL;
    uartConfig.direction = DL_UART_MAIN_DIRECTION_TX_RX;
    uartConfig.flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE;

    // Related to other configurations
    uartConfig.parity = DL_UART_MAIN_PARITY_NONE;
    uartConfig.stopBits = DL_UART_MAIN_STOP_BITS_ONE;
    uartConfig.wordLength = DL_UART_MAIN_WORD_LENGTH_8_BITS;

    DL_UART_Main_setClockConfig(UART0, &uartClock);
    DL_UART_Main_init(UART0, &uartConfig);
    DL_UART_Main_setOversampling(UART0, DL_UART_OVERSAMPLING_RATE_16X);
    DL_UART_Main_setBaudRateDivisor(UART0, 208, 21);
    DL_UART_Main_enable(UART0);

    UARTBaudRate_t baudRate = baud9600;
    char rChar, tChar;

    while (1) {
        // To avoid blocking, we check to see if there is a character to process. In that case, we proceed to receiving it.
        if (!DL_UART_Main_isRXFIFOEmpty(UART0)) {
            //beginning of "got new char"

            // calling this function without the above check results in a blocking code.
            // this function waits until it receives a character from UART
            rChar = DL_UART_Main_receiveData(UART0);

            // Depending on if the received char is a Number, a Letter, or Otherwise, the transmit char is N, L or O
            if (('0' <= rChar) && ('9' >= rChar))
                tChar = 'N';
            else if ((('a'<=rChar) && (rChar <= 'z')) ||
                    (('A'<=rChar) && (rChar <= 'Z')))
                tChar = 'L';
            else
                tChar = 'O';
            
            if (DL_UART_Main_isTXFIFOEmpty(UART0))
                DL_UART_Main_transmitData(UART0, tChar);

            // If the character is 'c', it also means to change the baudrate
            if (rChar == 'c') {
                switch (baudRate) {
                    case baud9600:
                        while (DL_UART_Main_isBusy(UART0)) {}            // let the last character finish sending
                        DL_UART_Main_disable(UART0);
                        DL_UART_Main_setBaudRateDivisor(UART0, 104, 11); // 19200 baud at 32 MHz
                        DL_UART_Main_enable(UART0);
                        baudRate = baud19200;
                        break;

                    case baud19200:
                        while (DL_UART_Main_isBusy(UART0)) {}
                        DL_UART_Main_disable(UART0);
                        DL_UART_Main_setBaudRateDivisor(UART0, 52, 5);   // 38400 baud
                        DL_UART_Main_enable(UART0);
                        baudRate = baud38400;
                        break;

                    case baud38400:
                        while (DL_UART_Main_isBusy(UART0)) {}
                        DL_UART_Main_disable(UART0);
                        DL_UART_Main_setBaudRateDivisor(UART0, 34, 46);  // 57600 baud
                        DL_UART_Main_enable(UART0);
                        baudRate = baud57600;
                        break;

                    case baud57600:
                        while (DL_UART_Main_isBusy(UART0)) {}
                        DL_UART_Main_disable(UART0);
                        DL_UART_Main_setBaudRateDivisor(UART0, 208, 21); // 9600 baud
                        DL_UART_Main_enable(UART0);
                        baudRate = baud9600;
                        break;
                }
            }
        }

        // The non-blocking test. This part of code should always work smoothly if other parts of code are not blocking it.
        // If the button is not pressed, keep the LED off
        if (!S2isPressed())
            TurnOff_Launchpad_LED2();
        else
            TurnOn_Launchpad_LED2();
    }
}


//// Simple GPIO HAL for non-blocking test ///////////////////////////////////

// When a button is pressed, it is grounded (logic 0)
#define PRESSED 0

void InitializeHardware() {
    DL_GPIO_reset(GPIOB);
    DL_UART_Main_reset(UART0);
    DL_GPIO_enablePower(GPIOB); // This is what enables software to write to pins in port B
    DL_UART_enablePower(UART0);
    delay_cycles(16); // Good practice to wait for the powered modules to start up before configuring them

    // make sure Tx and Rx pins work for UART and not a regular GPIO pin
    // PA10: Controls transmitting to launchpad's UART
    // PA11: Controls receiving from launchpad's UART
    // (the second argument picks the UART function from that pin's list of possible functions)
    DL_GPIO_initPeripheralOutputFunction(PA10_IOMUX, PA10_UART0_TX);
    DL_GPIO_initPeripheralInputFunction(PA11_IOMUX, PA11_UART0_RX);

    // PB21: Controls Launchpad Button 2 (LS2)
    DL_GPIO_initDigitalInputFeatures(PB21_IOMUX,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    // PB26: Controls Launchpad LED2 (LL2)
    DL_GPIO_initDigitalOutput(PB26_IOMUX);
    DL_GPIO_clearPins(PB26_PORT, PB26_PIN);
    DL_GPIO_enableOutput(PB26_PORT, PB26_PIN);
}

void TurnOn_Launchpad_LED2() {
    DL_GPIO_setPins(PB26_PORT, PB26_PIN);
}

void TurnOff_Launchpad_LED2() {
    DL_GPIO_clearPins(PB26_PORT, PB26_PIN);
}

char S2isPressed() {
    return DL_GPIO_readPins(PB21_PORT, PB21_PIN) == PRESSED;
}