#include <gpiod.h>
#include <unistd.h>
#include <iostream>

#define CHIPNAME "gpiochip0"
#define CONSUMER "apagado-matriz"

// Pines columnas
#define PIN_SER_COL   195
#define PIN_CLK_COL   197
#define PIN_LATCH_COL 196

// Pines filas
#define PIN_SER_ROW   38
#define PIN_CLK_ROW   37
#define PIN_LATCH_ROW 36

void pulse(gpiod_line* line) {
    gpiod_line_set_value(line, 1);
    usleep(1);
    gpiod_line_set_value(line, 0);
    usleep(1);
}

void shiftOut(gpiod_line* data, gpiod_line* clk, gpiod_line* latch, uint8_t val) {
    for (int i = 7; i >= 0; --i) {
        int bit = (val >> i) & 1;
        gpiod_line_set_value(data, bit);
        pulse(clk);
    }
    pulse(latch);
}

int main() {
    gpiod_chip* chip = gpiod_chip_open_by_name(CHIPNAME);
    if (!chip) {
        std::cerr << "Error al abrir el chip GPIO\n";
        return 1;
    }

    gpiod_line* serCol   = gpiod_chip_get_line(chip, PIN_SER_COL);
    gpiod_line* clkCol   = gpiod_chip_get_line(chip, PIN_CLK_COL);
    gpiod_line* latchCol = gpiod_chip_get_line(chip, PIN_LATCH_COL);
    gpiod_line* serRow   = gpiod_chip_get_line(chip, PIN_SER_ROW);
    gpiod_line* clkRow   = gpiod_chip_get_line(chip, PIN_CLK_ROW);
    gpiod_line* latchRow = gpiod_chip_get_line(chip, PIN_LATCH_ROW);

    if (!serCol || !clkCol || !latchCol || !serRow || !clkRow || !latchRow) {
        std::cerr << "Error al obtener las líneas GPIO\n";
        return 1;
    }

    gpiod_line_request_output(serCol, CONSUMER, 0);
    gpiod_line_request_output(clkCol, CONSUMER, 0);
    gpiod_line_request_output(latchCol, CONSUMER, 0);
    gpiod_line_request_output(serRow, CONSUMER, 0);
    gpiod_line_request_output(clkRow, CONSUMER, 0);
    gpiod_line_request_output(latchRow, CONSUMER, 0);

    // Apagar LEDs
    shiftOut(serCol, clkCol, latchCol, 0);
    shiftOut(serRow, clkRow, latchRow, 0);

    gpiod_chip_close(chip);
    return 0;
}

