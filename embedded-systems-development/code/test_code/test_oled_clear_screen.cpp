#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>

#define WIDTH 128
#define HEIGHT 64
#define FB_SIZE (WIDTH * HEIGHT / 8)

int main(int argc, char **argv) {
    const char *fb_path = "/dev/fb1";
    int fb = open(fb_path, O_RDWR);
    if (fb < 0) {
        perror("No se pudo abrir el framebuffer");
        return 1;
    }

    uint8_t buffer[FB_SIZE];
    
    // Por defecto: pintar todo blanco
    uint8_t color = 0xFF;
    if (argc > 1 && strcmp(argv[1], "black") == 0)
        color = 0x00;

    memset(buffer, color, FB_SIZE);
    write(fb, buffer, FB_SIZE);
    close(fb);

    std::cout << "Pantalla pintada completamente de " << ((color == 0xFF) ? "blanco" : "negro") << "\n";
    return 0;
}

