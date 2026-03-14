// SPDX-License-Identifier: MIT
#include "quantum.h"
#include "eeconfig.h"

extern uint16_t actuation_point[MATRIX_COLS][MATRIX_ROWS];

void keyboard_pre_init_kb(void) {
    /* debug_enable=true; */
    gpio_set_pin_input(C4);
    gpio_set_pin_output_push_pull(C6);
    gpio_set_pin_output_push_pull(C7);
    gpio_set_pin_output_push_pull(C8);
    gpio_set_pin_output_push_pull(C9);
    gpio_set_pin_output_push_pull(C14);
    gpio_set_pin_input(A3);
    gpio_set_pin_input(A4);
    gpio_set_pin_input(A5);
    gpio_set_pin_input(A6);
    gpio_set_pin_input(A7);
    gpio_set_pin_input(A10);
    gpio_set_pin_output_push_pull(B0);
    gpio_set_pin_output_push_pull(B1);
    gpio_set_pin_output_push_pull(B2);
    gpio_set_pin_output_push_pull(B6);
    gpio_set_pin_output_push_pull(B10);
    gpio_set_pin_output_push_pull(B11);
    gpio_set_pin_output_push_pull(B12);
    gpio_set_pin_output_push_pull(B13);
    gpio_set_pin_output_push_pull(B14);
    gpio_set_pin_output_push_pull(B15);
    gpio_set_pin_output_push_pull(A8);
    gpio_set_pin_output_push_pull(A9);
    gpio_set_pin_output_push_pull(D2);
    gpio_set_pin_input(B7);
    gpio_set_pin_input(B8);
    gpio_set_pin_input(B9);
    gpio_set_pin_output_push_pull(B3);
    gpio_set_pin_output_push_pull(B4);
    gpio_set_pin_output_push_pull(B5);
    gpio_set_pin_output_push_pull(B6);
    gpio_set_pin_output_push_pull(A15);

    // leds
    gpio_write_pin_low(A8);
    gpio_write_pin_low(A9);

    // bluetooth board?
    gpio_write_pin_high(A15);
    gpio_write_pin_high(B3);
    gpio_write_pin_high(B4);
    gpio_write_pin_high(B5);
    gpio_write_pin_high(B6);
    //
    gpio_write_pin_high(C14);
    gpio_write_pin_high(C7);
    gpio_write_pin_high(B6);

    palSetLineMode(A1, PAL_MODE_INPUT_ANALOG);
    /* rccResetADC1(); */
    /* rccEnableADC1(true); */

    rccEnableADC1(true);

    if (READ_BIT(ADC1->ISR, ADC_ISR_ADRDY)) SET_BIT(ADC1->ISR, ADC_ISR_ADRDY);
    if (READ_BIT(ADC1->CR, ADC_CR_ADEN)) SET_BIT(ADC1->CR, ADC_CR_ADDIS);

    while (READ_BIT(ADC1->CR, ADC_CR_ADEN))
        ;
    MODIFY_REG(ADC1->CR, ADC_CR_ADVREGEN, 0);
    MODIFY_REG(ADC1->CR, ADC_CR_ADVREGEN, ADC_CR_ADVREGEN);
    wait_ms(2);

    /* SET_BIT(ADC->CCR, ADC_CCR_LFMEN); */

    /* MODIFY_REG(ADC1->CFGR2, ADC_CFGR2_CKMODE, ADC_CFGR2_CKMODE_0); */
    MODIFY_REG(ADC1->CFGR2, ADC_CFGR2_CKMODE, ADC_CFGR2_CKMODE_0 | ADC_CFGR2_CKMODE_1);

    SET_BIT(ADC1->CR, ADC_CR_ADCAL);
    while (READ_BIT(ADC1->CR, ADC_CR_ADCAL))
        ;

    SET_BIT(ADC1->ISR, ADC_ISR_ADRDY);
    SET_BIT(ADC1->CR, ADC_CR_ADEN);
    while (!READ_BIT(ADC1->ISR, ADC_ISR_ADRDY))
        ;

    MODIFY_REG(ADC1->CFGR1, ADC_CFGR1_EXTEN, 0);

    CLEAR_BIT(ADC1->CFGR1, ADC_CFGR1_CONT);

    MODIFY_REG(ADC1->SMPR, ADC_SMPR_SMP, 0);

    ADC1->CHSELR = 1UL << 1;

    if (actuation_point[0][0] == 0) {
        const uint16_t *eeprom_calibration0 = (void *)0x08081100;
        const uint16_t *eeprom_calibration1 = (void *)0x08081200;
        for (uint8_t i = 0; i < 0x80; i++) {
            uint8_t col = i / 8;
            uint8_t row = i % 8;
            if (row < MATRIX_ROWS && col < MATRIX_COLS) actuation_point[col][row] = eeprom_calibration0[i] + eeprom_calibration1[i];
        }
    }
    if (actuation_point[0][0] == 0) {
        // fallback if eeprom didn't contain the calibration
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
                actuation_point[col][row] = 1375;
            }
        }
    }

    keyboard_pre_init_user();
}
