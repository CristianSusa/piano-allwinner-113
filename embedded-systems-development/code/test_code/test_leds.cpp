#include <gpiod.h>
#include <unistd.h>
#include <iostream>
#include <map>
#include <string>

#define CHIPNAME "gpiochip0"
#define CONSUMER "led-matrix-test"

// Pines columnas
#define PIN_SER_COL   195  // PG3
#define PIN_CLK_COL   197  // PG5
#define PIN_LATCH_COL 196  // PG4

// Pines filas
#define PIN_SER_ROW   38   // PB6
#define PIN_CLK_ROW   37   // PB5
#define PIN_LATCH_ROW 36   // PB4

// Notas organizadas por [columna][fila]
const char* notes[5][5] = {
    {"C4",  "F4",  "A#4", "D#5", "G#5"},
    {"C#4", "F#4", "B4",  "E5",  "A5"},
    {"D4",  "G4",  "C5",  "F5",  "A#5"},
    {"D#4", "G#4", "C#5", "F#5", "B5"},
    {"E4",  "A4",  "D5",  "G5",  "C6"}
};

gpiod_chip *chip;
gpiod_line *serCol, *clkCol, *latchCol;
gpiod_line *serRow, *clkRow, *latchRow;

void pulse(gpiod_line* line) {
    gpiod_line_set_value(line, 1);
    usleep(1);
    gpiod_line_set_value(line, 0);
    usleep(1);
}

bool setup() {
    chip = gpiod_chip_open_by_name(CHIPNAME);
    if (!chip) return false;

    serCol   = gpiod_chip_get_line(chip, PIN_SER_COL);
    clkCol   = gpiod_chip_get_line(chip, PIN_CLK_COL);
    latchCol = gpiod_chip_get_line(chip, PIN_LATCH_COL);

    serRow   = gpiod_chip_get_line(chip, PIN_SER_ROW);
    clkRow   = gpiod_chip_get_line(chip, PIN_CLK_ROW);
    latchRow = gpiod_chip_get_line(chip, PIN_LATCH_ROW);

    if (!serCol || !clkCol || !latchCol || !serRow || !clkRow || !latchRow)
        return false;

    if (gpiod_line_request_output(serCol, CONSUMER, 0) < 0 ||
        gpiod_line_request_output(clkCol, CONSUMER, 0) < 0 ||
        gpiod_line_request_output(latchCol, CONSUMER, 0) < 0 ||
        gpiod_line_request_output(serRow, CONSUMER, 0) < 0 ||
        gpiod_line_request_output(clkRow, CONSUMER, 0) < 0 ||
        gpiod_line_request_output(latchRow, CONSUMER, 0) < 0)
        return false;

    return true;
}

void shiftOut(gpiod_line* data, gpiod_line* clk, gpiod_line* latch, uint8_t val) {
    for (int i = 7; i >= 0; --i) {
        int bit = (val >> i) & 1;
        gpiod_line_set_value(data, bit);
        pulse(clk);
    }
    pulse(latch);
}

// Encender una nota específica
void lightNote(std::string note, const std::map<std::string, std::pair<int, int>>& noteToPosition) {
    auto it = noteToPosition.find(note);
    if (it == noteToPosition.end()) {
        std::cerr << "Nota no encontrada: " << note << std::endl;
        return;
    }

    int col = it->second.first;
    int row = it->second.second;

    uint8_t colMask = 1 << col;
    uint8_t rowMask = ~(1 << row);  // lógica inversa

    shiftOut(serCol, clkCol, latchCol, colMask);
    shiftOut(serRow, clkRow, latchRow, rowMask);
}

int main() {
    if (!setup()) {
        std::cerr << "Error al inicializar GPIOs\n";
        return 1;
    }

    std::map<std::string, std::pair<int, int>> noteToPosition;

    // Generar el mapeo inverso
    for (int col = 0; col < 5; ++col) {
        for (int row = 0; row < 5; ++row) {
            noteToPosition[notes[col][row]] = {col, row};
        }
    }

    // Barrido de todas las notas del piano
    const char* testNotes[] = {
        "C4", "C#4", "D4", "D#4", "E4", "F4", "F#4", "G4", "G#4",
        "A4", "A#4", "B4", "C5", "C#5", "D5", "D#5", "E5", "F5",
        "F#5", "G5", "G#5", "A5", "A#5", "B5", "C6"
    };

    for (const auto& note : testNotes) {
        std::cout << "Encendiendo nota: " << note << std::endl;
        lightNote(note, noteToPosition);
        usleep(300000);  // 300 ms
    }

    // Apagar todo
    shiftOut(serCol, clkCol, latchCol, 0);
    shiftOut(serRow, clkRow, latchRow, 0);
    gpiod_chip_close(chip);

    return 0;
}

