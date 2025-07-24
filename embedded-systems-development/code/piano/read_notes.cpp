#include <gpiod.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <csignal>   // para kill
#include <sys/types.h>
#include <sys/wait.h>

#define CHIPNAME "gpiochip0"
#define CONSUMER "piano"

#define PIN_SER_OUT   193
#define PIN_CLK_OUT   194
#define PIN_LATCH_OUT 192
#define PIN_SER_IN    128
#define PIN_CLK_IN    129
#define PIN_LATCH_IN   39

const char* notes[5][5] = {
    {"C4",  "F4",  "A#4", "D#5", "G#5"},
    {"C#4", "F#4", "B4",  "E5",  "A5"},
    {"D4",  "G4",  "C5",  "F5",  "A#5"},
    {"D#4", "G#4", "C#5", "F#5", "B5"},
    {"E4",  "A4",  "D5",  "G5",  "C6"}
};

// GPIO
gpiod_chip *chip;
gpiod_line *serOut, *clkOut, *latchOut;
gpiod_line *serIn, *clkIn, *latchIn;

std::map<std::string, pid_t> procesosActivos;

void pulse(gpiod_line *line) {
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

    if (!serOut || !clkOut || !latchOut || !serIn || !clkIn || !latchIn) return false;

    if (gpiod_line_request_output(serOut, CONSUMER, 0) < 0 ||
        gpiod_line_request_output(clkOut, CONSUMER, 0) < 0 ||
        gpiod_line_request_output(latchOut, CONSUMER, 0) < 0 ||
        gpiod_line_request_input(serIn, CONSUMER) < 0 ||
        gpiod_line_request_output(clkIn, CONSUMER, 0) < 0 ||
        gpiod_line_request_output(latchIn, CONSUMER, 0) < 0)
        return false;

    return true;
}

void shiftOut(uint8_t val) {
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

void tocarNota(const std::string& nota) {
    if (procesosActivos.count(nota) == 0) {
        pid_t pid = fork();
        if (pid == 0) {
            execl(("./bin/" + nota).c_str(), nota.c_str(), nullptr);
            _exit(1); // por si falla
        }
        procesosActivos[nota] = pid;
        std::cout << "Tocando: " << nota << " (PID: " << pid << ")\n";
    }
}

void apagarNota(const std::string& nota) {
    if (procesosActivos.count(nota)) {
        pid_t pid = procesosActivos[nota];
        kill(pid, SIGTERM);
        waitpid(pid, nullptr, 0); // limpia proceso zombie
        procesosActivos.erase(nota);
        std::cout << "Nota parada: " << nota << "\n";
    }
}

void apagarTodas() {
    for (auto& [nota, pid] : procesosActivos) {
        kill(pid, SIGTERM);
        waitpid(pid, nullptr, 0);
    }
    procesosActivos.clear();
}

int main() {
    if (!setup()) {
        std::cerr << "Error al inicializar GPIO\n";
        return 1;
    }

    std::map<std::string, bool> estadoAnterior;

    while (true) {
        std::map<std::string, bool> estadoActual;

        for (int col = 0; col < 5; ++col) {
            uint8_t colMask = 1 << col;
            shiftOut(colMask);
            usleep(100);

            uint8_t rawRowState = shiftIn();
            uint8_t rowState = rawRowState >> 1;

            for (int row = 0; row < 5; ++row) {
                std::string nota = notes[col][row];
                bool presionada = (rowState & (1 << row));
                estadoActual[nota] = presionada;
            }
        }

        // Comparar estados
        for (auto &[nota, presionada] : estadoActual) {
            bool antes = estadoAnterior[nota];
            if (presionada && !antes) {
                tocarNota(nota);
            } else if (!presionada && antes) {
                apagarNota(nota);
            }
        }

        estadoAnterior = estadoActual;
        usleep(30000); // 30 ms
    }

    apagarTodas();
    gpiod_chip_close(chip);
    return 0;
}
