#include <ti/driverlib/driverlib.h>
#include "GraphicsLib/graphics.h"
#include "GraphicsLib/display.h"

#define PRESSED 0

void Init_ButtonLED();
uint16_t colormix(int r, int g, int b);
void TurnOn_LL1();
void TurnOff_LL1();
char SwitchStatus_LB2();

int main(void) {
    Graphics_Init();
    Init_ButtonLED();

    while (1) {
        if (SwitchStatus_LB2() != PRESSED) TurnOff_LL1();
        else TurnOn_LL1();

        for (int j = 0; j < 128; j++)
            for (int i = 0; i < 128; i++)
                Graphics_DrawLineH(0, 127, (i + j) % 127, colormix(25, i * 2, 256 - i * 2));
    }
}

void Init_ButtonLED() {
    DL_GPIO_initDigitalOutput(IOMUX_PINCM1);
    DL_GPIO_enableOutput(GPIOA, DL_GPIO_PIN_0);

    DL_GPIO_initDigitalInputFeatures(IOMUX_PINCM49,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
}

uint16_t colormix(int r, int g, int b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3);
}

void TurnOn_LL1()  { DL_GPIO_setPins(GPIOA, DL_GPIO_PIN_0); }
void TurnOff_LL1() { DL_GPIO_clearPins(GPIOA, DL_GPIO_PIN_0); }

char SwitchStatus_LB2() {
    return DL_GPIO_readPins(GPIOB, DL_GPIO_PIN_21) != 0;
}