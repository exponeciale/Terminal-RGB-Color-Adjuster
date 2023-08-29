#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <math.h>

#define CIRC_HEIGHT 17
#define CIRC_WIDTH 75
#define CIRC_ASPECT_RATIO 2
#define CIRC_RADIUS 5
#define CIRC_CENTER_X (CIRC_WIDTH / 2.0)
#define CIRC_CENTER_Y (CIRC_HEIGHT / 2.0)

typedef struct {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} ColorRGB;

void printColor(ColorRGB color) {
    printf("RGB(%d, %d, %d)\n", color.red, color.green, color.blue);
}

void clearScreen() {
    printf("\033[H\033[J"); // Código ANSI para limpar a tela
}

void setUnbufferedInput() {
    struct termios term;
    tcgetattr(0, &term);
    term.c_lflag &= ~ICANON;
    term.c_cc[VMIN] = 1;
    term.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &term);
}

char getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    return buf;
}

int clip(int v, int max) {
    return v < max ? v : max - 1;
}

int main() {
    ColorRGB color = {128, 128, 128}; // Cor inicial

    int maxX = CIRC_WIDTH;
    int maxY = CIRC_HEIGHT;

    setUnbufferedInput(); // Configurar o terminal para leitura sem buffer

    while (1) {
        clearScreen(); // Limpar a tela antes de imprimir os círculos

        for (int y = 0; y < CIRC_HEIGHT; y++) {
            for (int x = 0; x < CIRC_WIDTH; x++) {
                float distance1 = sqrt(pow((x - (CIRC_CENTER_X - 27)) / CIRC_ASPECT_RATIO, 2) + pow(y - CIRC_CENTER_Y + 2, 2));
                float distance2 = sqrt(pow((x - (CIRC_CENTER_X - 18)) / CIRC_ASPECT_RATIO, 2) + pow(y - CIRC_CENTER_Y + 2, 2));
                float distance3 = sqrt(pow((x - CIRC_CENTER_X + 22) / CIRC_ASPECT_RATIO, 2) + pow(y - (CIRC_CENTER_Y + 2), 2));
                float distance4 = sqrt(pow((x - CIRC_CENTER_X - 20) / CIRC_ASPECT_RATIO, 2) + pow(y - CIRC_CENTER_Y, 2));

                int printX = CIRC_CENTER_X + x;
                int printY = CIRC_CENTER_Y + y;

                printX = clip(printX, maxX);
                printY = clip(printY, maxY);

                int r = distance1 <= CIRC_RADIUS ? color.red : 0;
                int g = distance2 <= CIRC_RADIUS ? color.green : 0;
                int b = distance3 <= CIRC_RADIUS ? color.blue : 0;

                // Impressão do caractere colorido
                if (distance4 <= CIRC_RADIUS) {
                    printf("\033[38;2;%d;%d;%dmo", color.red, color.green, color.blue);  // Combined circle
                } else {
                    printf("\033[38;2;%d;%d;%dmo", r, g, b);  // Original circles
                }
            }
            printf("\033[0m\n");  // Reset color
        }

        printf("\nControles:\n");
        printf("'a' para diminuir R, 'd' para aumentar R\n");
        printf("'w' para aumentar G, 's' para diminuir G\n");
        printf("'k' para aumentar B, 'j' para diminuir B\n");
        printf("'q' para sair\n");

        printColor(color);
        printf("\033[38;2;%d;%d;%dm###\n", color.red, color.green, color.blue);  // Indicador de cor atual
        printf("\033[0m"); // Reset color

        char input = getch();

        if (input == 'q') {
            break;
        } else if (input == 'a' && color.red > 0) {
            color.red--;
        } else if (input == 'd' && color.red < 255) {
            color.red++;
        } else if (input == 'w' && color.green < 255) {
            color.green++;
        } else if (input == 's' && color.green > 0) {
            color.green--;
        } else if (input == 'k' && color.blue < 255) {
            color.blue++;
        } else if (input == 'j' && color.blue > 0) {
            color.blue--;
        }
    }

    // Restaurar as configurações originais do terminal
    struct termios term;
    tcgetattr(0, &term);
    term.c_lflag |= ICANON;
    tcsetattr(0, TCSANOW, &term);

    return 0;
}
