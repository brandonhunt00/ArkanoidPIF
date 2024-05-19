#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/select.h>  

#define MIN_X 1
#define MAX_X 79
#define MIN_Y 1
#define MAX_Y 23
#define NUMERO_DE_TIJOLOS 20

typedef struct {
    double x;
    double y;
    double velX;
    double velY;
} Objeto;

typedef struct {
    double x;
    double y;
    int durabilidade;
} Tijolo;

Objeto raquete;
Objeto bola;
Tijolo Tijolos[NUMERO_DE_TIJOLOS];

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#define CLEAR_SCREEN() system("cls")
#else
#include <unistd.h>
#define CLEAR_SCREEN() printf("\033[H\033[J")
#endif

int kbhit() {
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
}

void updateTela() {
    CLEAR_SCREEN();
}

void timerEspera() {
#ifdef _WIN32
    Sleep(100); 
#else
    usleep(100000); 
#endif
}

void moverRaquete() {
    if (kbhit()) {
#ifdef _WIN32
        char ch = _getch();
#else
        char ch = getchar();
#endif
        if (ch == 'a' && raquete.x > MIN_X + 2) {
            raquete.x -= 2;
        } else if (ch == 'd' && raquete.x < MAX_X - 5) {
            raquete.x += 2;
        }
    }
}

void moverBola() {
    bola.x += bola.velX;
    bola.y += bola.velY;

    if (bola.x <= MIN_X || bola.x >= MAX_X) {
        bola.velX *= -1;
    }
    if (bola.y <= MIN_Y || bola.y >= MAX_Y) {
        bola.velY *= -1;
    }
    if (bola.y == raquete.y - 1 && bola.x >= raquete.x && bola.x <= raquete.x + 5) {
        bola.velY *= -1;
    }

    for (int i = 0; i < NUMERO_DE_TIJOLOS; i++) {
        Tijolo *Tijolo = &Tijolos[i];
        if (Tijolo->durabilidade > 0 && bola.x >= Tijolo->x && bola.x <= Tijolo->x + 1 && bola.y >= Tijolo->y && bola.y <= Tijolo->y + 1) {
            Tijolo->durabilidade--;
            if (Tijolo->durabilidade == 0) {
                bola.velY *= -1;
            }
        }
    }
}

void printarObjeto(double nextX, double nextY, char *objeto) {
    printf("\033[%d;%dH%s", (int)nextY, (int)nextX, objeto);
}

int main(void) {
    srand(time(NULL));
    raquete.x = MAX_X / 2;
    raquete.y = MAX_Y - 1;

    bola.x = MAX_X / 2;
    bola.y = MAX_Y - 2;
    bola.velX = 0.2;
    bola.velY = -0.1;

    for (int i = 0; i < NUMERO_DE_TIJOLOS; i++) {
        Tijolos[i].x = 5 + i % 15 * 5;
        Tijolos[i].y = 5 + i / 15 * 2;
        Tijolos[i].durabilidade = 1;
    }
    updateTela();

    while (1) {
        moverRaquete();
        moverBola();

        printarObjeto(raquete.x, raquete.y, "=====");
        printarObjeto(bola.x, bola.y, "o");

        for (int i = 0; i < NUMERO_DE_TIJOLOS; i++) {
            Tijolo *Tijolo = &Tijolos[i];
            if (Tijolo->durabilidade > 0) {
                printarObjeto(Tijolo->x, Tijolo->y, "[]");
            }
        }

        int tijolosEsquerda = 0;
        for (int i = 0; i < NUMERO_DE_TIJOLOS; i++) {
            if (Tijolos[i].durabilidade > 0) {
                tijolosEsquerda++;
            }
        }
        if (tijolosEsquerda == 0) {
            printarObjeto(MAX_X / 2 - 5, MAX_Y / 2, "Você venceu!");
            break;
        }

        if (bola.y >= MAX_Y - 1) {
            printarObjeto(MAX_X / 2 - 5, MAX_Y / 2, "GAME OVER");
            break;
        }
        updateTela();
        timerEspera();
    }
    return 0;
}
