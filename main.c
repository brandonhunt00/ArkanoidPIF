#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#define MIN_X 1
#define MAX_X 79
#define MIN_Y 1
#define MAX_Y 23
#define RAQUETE_SIZE 10
#define NUMERO_DE_TIJOLOS 20

typedef struct {
    double x;
    double y;
    double velX;
    double velY;
} Objeto;

typedef struct Tijolo {
    double x;
    double y;
    int durabilidade;
    struct Tijolo *prox;
} Tijolo;

Objeto raquete;
Objeto bola;
Tijolo *tijolos = NULL;

int placar = 0; 

int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

void inicializarTijolos() {
    for (int i = 0; i < NUMERO_DE_TIJOLOS; i++) {
        Tijolo *novo = (Tijolo*)malloc(sizeof(Tijolo));
        novo->x = 5 + i % 15 * 5;
        novo->y = 5 + i / 15 * 2;
        novo->durabilidade = 1;
        novo->prox = tijolos;
        tijolos = novo;
    }
}
void liberarTijolos() {
    while (tijolos != NULL) {
        Tijolo *temp = tijolos;
        tijolos = tijolos->prox;
        free(temp);
    }
}
void updateTela() {
    system("clear");
}
void timerEspera() {
    usleep(30000); 
}

void moverRaquete() {
    if (kbhit()) {
        int ch = getchar();
        if (ch == '\033') { 
            getchar(); 
            switch(getchar()) { 
                case 'C':
                    if (raquete.x < MAX_X - RAQUETE_SIZE - 1) {
                        printf("\033[%d;%dH          ", (int)raquete.y, (int)raquete.x); 
                        raquete.x += 4;
                    }
                    break;
                case 'D': 
                    if (raquete.x > MIN_X + 2) {
                        printf("\033[%d;%dH          ", (int)raquete.y, (int)raquete.x); 
                        raquete.x -= 4; 
                    }
                    break;
            }
        }
    }
}

void moverBola() {
    printf("\033[%d;%dH ", (int)bola.y, (int)bola.x);

    bola.x += bola.velX;
    bola.y += bola.velY;

    if (bola.x <= MIN_X || bola.x >= MAX_X) {
        bola.velX *= -1;
    }
    if (bola.y <= MIN_Y || bola.y >= MAX_Y) {
        bola.velY *= -1;
    }
    if (bola.y == raquete.y - 1 && bola.x >= raquete.x && bola.x <= raquete.x + RAQUETE_SIZE - 1) {
        bola.velY *= -1;
        if (bola.x < raquete.x || bola.x > raquete.x + RAQUETE_SIZE - 1) {
            bola.y--;
        }
    }

    Tijolo *atual = tijolos;
    Tijolo *anterior = NULL;
    while (atual != NULL) {
        if (bola.x >= atual->x && bola.x <= atual->x + 1 && bola.y >= atual->y && bola.y <= atual->y + 1) {
            atual->durabilidade--;
            if (atual->durabilidade == 0) {
                placar += 5; 

                if (anterior == NULL) {
                    Tijolo *temp = atual;
                    atual = atual->prox;
                    tijolos = atual;
                    free(temp);
                } else {
                    anterior->prox = atual->prox;
                    free(atual);
                    atual = anterior->prox;
                }
                bola.velY *= -1;
                break;
            }
        }
        anterior = atual;
        atual = atual->prox;
    }
    printf("\033[%d;%dHo", (int)bola.y, (int)bola.x);
}

int main(void) {
    srand(time(NULL));
    raquete.x = MAX_X / 2 - RAQUETE_SIZE / 2;
    raquete.y = MAX_Y - 1;

    bola.x = MAX_X / 2;
    bola.y = MAX_Y - 2;
    bola.velX = 0.6; 
    bola.velY = -0.3;

    inicializarTijolos();
    updateTela();

    while (1) {

        moverRaquete();
        moverBola();
        printf("\033[%d;%dH==========", (int)raquete.y, (int)raquete.x);

        Tijolo *atual = tijolos;
        while (atual != NULL) {
            if (atual->durabilidade > 0) {
                printf("\033[%d;%dH[]", (int)atual->y, (int)atual->x);
            }
            atual = atual->prox;
        }
        if (tijolos == NULL) {
            printf("\033[%d;%dHVocê venceu! Placar: %d\n", MAX_Y / 2, MAX_X / 2 - 15, placar);
            break;
        }
        if (bola.y >= MAX_Y - 1) {
            printf("\033[%d;%dHGAME OVER Placar: %d\n", MAX_Y / 2, MAX_X / 2 - 15, placar);
            break;
        }
        timerEspera();
    }
    liberarTijolos();
    return 0;
}
