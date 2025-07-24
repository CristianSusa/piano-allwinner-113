#include <gpiod.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <csignal>
#include <sys/types.h>
#include <sys/wait.h>

#define CHIPNAME "gpiochip0"
#define CONSUMER "tutor-estrellita"

#define PIN_SER_OUT   193
#define PIN_CLK_OUT   194
#define PIN_LATCH_OUT 192
#define PIN_SER_IN    128
#define PIN_CLK_IN    129
#define PIN_LATCH_IN   39

#define PIN_SER_COL   195
#define PIN_CLK_COL   197
#define PIN_LATCH_COL 196
#define PIN_SER_ROW   38
#define PIN_CLK_ROW   37
#define PIN_LATCH_ROW 36

const char* notes[5][5] = {
    {"C4",  "F4",  "A#4", "D#5", "G#5"},
    {"C#4", "F#4", "B4",  "E5",  "A5"},
    {"D4",  "G4",  "C5",  "F5",  "A#5"},
    {"D#4", "G#4", "C#5", "F#5", "B5"},
    {"E4",  "A4",  "D5",  "G5",  "C6"}
};

const std::vector<std::string> melodia = {
    "C4", "C4", "D4", "C4", "F4", "E4",
    "C4", "C4", "D4", "C4", "G4", "F4",
    "C4", "C4", "C5", "A4", "F4", "E4", "D4",
    "A#4", "A#4", "A4", "F4", "G4", "F4"
};

gpiod_chip *chip;
gpiod_line *serOut, *clkOut, *latchOut;
gpiod_line *serIn, *clkIn, *latchIn;
gpiod_line *serCol, *clkCol, *latchCol;
gpiod_line *serRow, *clkRow, *latchRow;

std::map<std::string, pid_t> procesosActivos;
std::map<std::string, std::pair<int, int>> noteToPosition;

void pulse(gpiod_line* line) {
    gpiod_line_set_value(line, 1);
    usleep(1);
    gpiod_line_set_value(line, 0);
    usleep(1);
}

bool setup() {
    chip = gpiod_chip_open_by_name(CHIPNAME);
    if (!chip) return false;

    serOut = gpiod_chip_get_line(chip, PIN_SER_OUT);
    clkOut = gpiod_chip_get_line(chip, PIN_CLK_OUT);
    latchOut = gpiod_chip_get_line(chip, PIN_LATCH_OUT);
    serIn = gpiod_chip_get_line(chip, PIN_SER_IN);
    clkIn = gpiod_chip_get_line(chip, PIN_CLK_IN);
    latchIn = gpiod_chip_get_line(chip, PIN_LATCH_IN);

    serCol = gpiod_chip_get_line(chip, PIN_SER_COL);
    clkCol = gpiod_chip_get_line(chip, PIN_CLK_COL);
    latchCol = gpiod_chip_get_line(chip, PIN_LATCH_COL);
    serRow = gpiod_chip_get_line(chip, PIN_SER_ROW);
    clkRow = gpiod_chip_get_line(chip, PIN_CLK_ROW);
    latchRow = gpiod_chip_get_line(chip, PIN_LATCH_ROW);

    if (!serOut || !clkOut || !latchOut || !serIn || !clkIn || !latchIn ||
        !serCol || !clkCol || !latchCol || !serRow || !clkRow || !latchRow)
        return false;

    if (gpiod_line_request_output(serOut, CONSUMER, 0) < 0 ||
        gpiod_line_request_output(clkOut, CONSUMER, 0) < 0 ||
        gpiod_line_request_output(latchOut, CONSUMER, 0) < 0 ||
        gpiod_line_request_input(serIn, CONSUMER) < 0 ||
        gpiod_line_request_output(clkIn, CONSUMER, 0) < 0 ||
        gpiod_line_request_output(latchIn, CONSUMER, 0) < 0 ||
        gpiod_line_request_output(serCol, CONSUMER, 0) < 0 ||
        gpiod_line_request_output(clkCol, CONSUMER, 0) < 0 ||
        gpiod_line_request_output(latchCol, CONSUMER, 0) < 0 ||
        gpiod_line_request_output(serRow, CONSUMER, 0) < 0 ||
        gpiod_line_request_output(clkRow, CONSUMER, 0) < 0 ||
        gpiod_line_request_output(latchRow, CONSUMER, 0) < 0)
        return false;

    return true;
}

void shiftOutKeys(uint8_t val) {
    for (int i = 7; i >= 0; --i) {
        int bit = (val >> i) & 1;
        gpiod_line_set_value(serOut, bit);
        pulse(clkOut);
    }
    pulse(latchOut);
}

uint8_t shiftIn() {
    gpiod_line_set_value(latchIn, 0);
    usleep(1);
    gpiod_line_set_value(latchIn, 1);
    usleep(1);

    uint8_t value = 0;
    for (int i = 0; i < 8; ++i) {
        pulse(clkIn);
        int bit = gpiod_line_get_value(serIn);
        value = (value << 1) | (bit & 1);
    }
    return value;
}

void shiftOut(gpiod_line* data, gpiod_line* clk, gpiod_line* latch, uint8_t val) {
    for (int i = 7; i >= 0; --i) {
        int bit = (val >> i) & 1;
        gpiod_line_set_value(data, bit);
        pulse(clk);
    }
    pulse(latch);
}

void tocarNota(const std::string& nota) {
    if (procesosActivos.count(nota) == 0) {
        pid_t pid = fork();
        if (pid == 0) {
            execl(("./bin/" + nota).c_str(), nota.c_str(), nullptr);
            _exit(1);
        }
        procesosActivos[nota] = pid;
    }
}

void apagarNota(const std::string& nota) {
    if (procesosActivos.count(nota)) {
        pid_t pid = procesosActivos[nota];
        kill(pid, SIGTERM);
        waitpid(pid, nullptr, 0);
        procesosActivos.erase(nota);
    }
}

void apagarTodas() {
    for (auto& [nota, pid] : procesosActivos) {
        kill(pid, SIGTERM);
        waitpid(pid, nullptr, 0);
    }
    procesosActivos.clear();
}

void lightNote(std::string nota) {
    auto it = noteToPosition.find(nota);
    if (it == noteToPosition.end()) return;
    int col = it->second.first;
    int row = it->second.second;
    uint8_t colMask = 1 << col;
    uint8_t rowMask = ~(1 << row);
    shiftOut(serCol, clkCol, latchCol, colMask);
    shiftOut(serRow, clkRow, latchRow, rowMask);
}

int main() {
    if (!setup()) {
        std::cerr << "Error al inicializar GPIO\n";
        return 1;
    }

    for (int col = 0; col < 5; ++col)
        for (int row = 0; row < 5; ++row)
            noteToPosition[notes[col][row]] = {col, row};

    for (const auto& nota : melodia) {
        std::cout << "Toca la nota: " << nota << std::endl;
        lightNote(nota);

        bool notaPresionada = false;
        std::map<std::string, bool> estadoAnterior;

        while (!notaPresionada) {
            std::map<std::string, bool> estadoActual;

            for (int col = 0; col < 5; ++col) {
                uint8_t colMask = 1 << col;
                shiftOutKeys(colMask);
                usleep(100);
                uint8_t rawRowState = shiftIn();
                uint8_t rowState = rawRowState >> 1;

                for (int row = 0; row < 5; ++row) {
                    std::string notaLeida = notes[col][row];
                    bool presionada = (rowState & (1 << row));
                    estadoActual[notaLeida] = presionada;

                    if (presionada && !estadoAnterior[notaLeida]) {
                        tocarNota(notaLeida);
                    } else if (!presionada && estadoAnterior[notaLeida]) {
                        apagarNota(notaLeida);
                    }

                    if (notaLeida == nota && presionada) {
                        notaPresionada = true;
                    }
                }
            }
            estadoAnterior = estadoActual;
            usleep(20000);
        }

        usleep(1000000);
        apagarNota(nota);
    }

    apagarTodas();
    shiftOut(serCol, clkCol, latchCol, 0);
    shiftOut(serRow, clkRow, latchRow, 0);
    gpiod_chip_close(chip);
    return 0;
}

