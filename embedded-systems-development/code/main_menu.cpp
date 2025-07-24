#include <gpiod.h>
#include <unistd.h>
#include <iostream>
#include <csignal>
#include <sys/wait.h>

#define CHIPNAME "gpiochip0"
#define PIN_IZQ 132
#define PIN_ENT 133
#define PIN_DER 134

enum EstadoMenu { RAIZ, SELECCION_MODO, MODO_TUTOR, MODO_NORMAL };
enum OpcionRaiz { NORMAL, TUTOR };
enum CancionTutor { ESTRELLITA, HBD, PIRATAS, POLLITOS };

gpiod_chip *chip;
gpiod_line *btnIzq, *btnEnt, *btnDer;

bool setup() {
    chip = gpiod_chip_open_by_name(CHIPNAME);
    if (!chip) return false;

    btnIzq = gpiod_chip_get_line(chip, PIN_IZQ);
    btnEnt = gpiod_chip_get_line(chip, PIN_ENT);
    btnDer = gpiod_chip_get_line(chip, PIN_DER);

    if (!btnIzq || !btnEnt || !btnDer) return false;

    if (gpiod_line_request_input(btnIzq, "menu") < 0 ||
        gpiod_line_request_input(btnEnt, "menu") < 0 ||
        gpiod_line_request_input(btnDer, "menu") < 0)
        return false;

    return true;
}

bool presionado(gpiod_line* btn) {
    return gpiod_line_get_value(btn) == 1;
}

void esperar_liberacion() {
    while (presionado(btnIzq) || presionado(btnEnt) || presionado(btnDer)) {
        usleep(50000);
    }
    usleep(150000); // debounce
}

void mostrar(const std::string& imagen) {
    if (fork() == 0) {
        execl(("./" + imagen).c_str(), imagen.c_str(), nullptr);
        _exit(1);
    } else {
        int status;
        wait(&status);
    }
}

void ejecutar_con_escape(const std::string& binario) {
    pid_t pid = fork();
    if (pid == 0) {
        execl(("./" + binario).c_str(), binario.c_str(), nullptr);
        _exit(1);
    }

    // Proceso padre: monitorea botones mientras ejecuta el hijo
    while (true) {
        if (presionado(btnEnt) && presionado(btnDer)) {
            kill(pid, SIGTERM);
            waitpid(pid, nullptr, 0);
            break;
        }

        int status;
        pid_t result = waitpid(pid, &status, WNOHANG);
        if (result == pid) break; // terminó naturalmente

        usleep(100000); // 100ms
    }

    esperar_liberacion();
}

void ejecutar(const std::string& binario) {
    if (fork() == 0) {
        execl(("./" + binario).c_str(), binario.c_str(), nullptr);
        _exit(1);
    } else {
        int status;
        wait(&status);
    }
}

int main() {

    ejecutar("apagar_leds");

    if (!setup()) {
        std::cerr << "Error al inicializar botones GPIO\n";
        return 1;
    }

    EstadoMenu estado = RAIZ;
    OpcionRaiz opcion = NORMAL;
    CancionTutor cancion = ESTRELLITA;

    mostrar("o_menu_normal");

    while (true) {
        bool izq = presionado(btnIzq);
        bool der = presionado(btnDer);
        bool ent = presionado(btnEnt);

        // Volver atrás si se presionan ENTER + DERECHA
        if (ent && der) {
            if (estado == MODO_TUTOR || estado == MODO_NORMAL) {
                estado = RAIZ;
                mostrar("o_menu_normal");
                esperar_liberacion();
                continue;
            }
        }

        if (estado == RAIZ) {
            if (izq || der) {
                opcion = (opcion == NORMAL) ? TUTOR : NORMAL;
                mostrar(opcion == NORMAL ? "o_menu_normal" : "o_menu_tutor");
                esperar_liberacion();
            } else if (ent) {
                if (opcion == NORMAL) {
                    mostrar("o_normal");
                    estado = MODO_NORMAL;
                    ejecutar_con_escape("read_notes");
                    mostrar("o_menu_normal");
                    estado = RAIZ;
                } else {
                    estado = MODO_TUTOR;
                    cancion = ESTRELLITA;
                    mostrar("o_t_estrellita");
                    esperar_liberacion();
                }
            }
        }

        else if (estado == MODO_TUTOR) {
            if (izq) {
                cancion = static_cast<CancionTutor>((cancion + 3) % 4);
                switch (cancion) {
                    case ESTRELLITA: mostrar("o_t_estrellita"); break;
                    case HBD:         mostrar("o_t_hbd"); break;
                    case PIRATAS:     mostrar("o_t_piratas"); break;
                    case POLLITOS:    mostrar("o_t_pollitos"); break;
                }
                esperar_liberacion();
            } else if (der) {
                cancion = static_cast<CancionTutor>((cancion + 1) % 4);
                switch (cancion) {
                    case ESTRELLITA: mostrar("o_t_estrellita"); break;
                    case HBD:         mostrar("o_t_hbd"); break;
                    case PIRATAS:     mostrar("o_t_piratas"); break;
                    case POLLITOS:    mostrar("o_t_pollitos"); break;
                }
                esperar_liberacion();
            } else if (ent) {
                switch (cancion) {
                    case ESTRELLITA: ejecutar("t_estrellita"); break;
                    case HBD:         ejecutar("t_hbd"); break;
                    case PIRATAS:     ejecutar("t_piratas"); break;
                    case POLLITOS:    ejecutar("t_pollitos"); break;
                }
                mostrar("o_menu_normal");
                estado = RAIZ;
                esperar_liberacion();
            }
        }

        usleep(100000); // 100ms
    }

    gpiod_chip_close(chip);
    return 0;
}
