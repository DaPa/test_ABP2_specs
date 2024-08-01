/*
 * Testing pressure formula from teh ABP2 specs
 */

#pragma once

#include <Arduino.h>

#define TESTS_NR 15U
static uint8_t data[TESTS_NR][4] = {
    { 0, 0x1A, 0x7F, 0x7F },
    { 0, 0x1A, 0x7F, 0x80 },
    { 0, 0x1A, 0x80, 0x7F },
    { 0, 0x1A, 0x80, 0x80 },
    { 0, 0x80, 0x7F, 0x7F },
    { 0, 0x80, 0x7F, 0x80 },
    { 0, 0x80, 0x80, 0x7F },
    { 0, 0x80, 0x80, 0x80 },

    // see TABLE 6. SENSOR OUTPUT AT SIGNIFICANT PERCENTAGES, on page 6 of
    //  sps-siot-abp2-series-datasheet-32350268-en.pdf
    //  (32350268-F-EN | F | 11/23, ©2023 Honeywell International Inc.)
    { 0, 0x00, 0x00, 0x00 },  // expect 0x000000 = 0        (  0%)
    { 0, 0x19, 0x99, 0x9A },  // expect 0x19999A = 1677722  ( 10%)
    { 0, 0x4C, 0xCC, 0xCC },  // expect 0x4CCCCC = 5033164  ( 30%)
    { 0, 0x80, 0x00, 0x00 },  // expect 0x800000 = 8388608  ( 50%)
    { 0, 0xB3, 0x33, 0x33 },  // expect 0xB33333 = 11744051 ( 70%)
    { 0, 0xE6, 0x66, 0x66 },  // expect 0xE66666 = 15099494 ( 90%)
    { 0, 0xFF, 0xFF, 0xFF },  // expect 0xFFFFFF = 16777215 (100%)
};
static uint8_t i = 0;

// the setup function runs once when you press reset or power the board
void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    while (!Serial) delay(10);
    Serial.println("test starting!");
    delay(2000);
}

// the loop function runs over and over again forever
void loop() {

    for (;;) {

        const uint8_t *p = data[i++];

        // code as per sps-siot-abp2-series-datasheet-32350268-en.pdf, page 32, where
        //  if p[2] = 128 to 255 will result into a negative (p[2] * 256) term!
        double press_counts = p[3] + p[2] * 256 + p[1] * 65536; // calculate digital pressure counts

        // corrected code, which correctly restore pressure counts from the received 3 bytes
        //  as per TABLE 6. SENSOR OUTPUT AT SIGNIFICANT PERCENTAGES
        uint32_t press_counts_u = ((uint32_t)p[1] << 16U) | ((uint32_t)p[2] << 8U) | ((uint32_t)p[3]);


        char printBuffer[300], cBuff[20];
        dtostrf(press_counts, 4, 0, cBuff);
        sprintf(printBuffer, " %u: %2x %2x %2x\t press counts: original=%s(%8lx) \t vs \t corrected=%lu(%8lx)\t diff=%lu\n",
                i, p[1], p[2], p[3],
                cBuff, (uint32_t)press_counts,
                press_counts_u, press_counts_u,
                press_counts_u - (uint32_t)press_counts);
        Serial.print(printBuffer);

        if (i >= TESTS_NR) {
            i = 0;
            Serial.println("\n");
            delay(3000);
        }

        delay(100);
    }
}

