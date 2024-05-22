#include "keyboard.h"
#include "screen.h"
#include "timer.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MIN_X 1
#define MAX_X 79
#define MIN_Y 1
#define MAX_Y 23
#define RAQUETE_SIZE 10
#define NUMERO_DE_TIJOLOS 45 // 15 tijolos por fileira * 3 fileiras
#define FPS 60
#define FRAME_DELAY (1000 / FPS)

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

Objeto raquete, bola;
Tijolo *tijolos = NULL;
int placar = 0, highScore = 0;

void wait_ms(int ms);
void inicializarTijolos();
void liberarTijolos();
void moverRaquete();
void moverBola();
void printarPlacar();
void salvarHighScore();
void carregarHighScore();
void verificarHighScore();
void drawGame();

void inicializar() {
  screenInit(1);
  keyboardInit();
  carregarHighScore();
  inicializarTijolos();
  raquete.x = (MAX_X / 2) - (RAQUETE_SIZE / 2);
  raquete.y = MAX_Y - 2;
  bola.x = raquete.x + (RAQUETE_SIZE / 2);
  bola.y = raquete.y - 1;
  bola.velX = 0.28;
  bola.velY = -0.28;
  placar = 0;
  screenClear();
}

void finalizar() {
  liberarTijolos();
  keyboardDestroy();
  screenDestroy();
  salvarHighScore();
}

void loopJogo() {
  bool jogoAtivo = true;
  timerInit(FRAME_DELAY);
  while (jogoAtivo) {
    if (timerTimeOver()) {
      moverRaquete();
      moverBola();
      drawGame();
      if (tijolos == NULL || bola.y >= MAX_Y) {
        jogoAtivo = false;
      }
      timerUpdateTimer(FRAME_DELAY);
    }
  }
}

void drawGame() {
  screenClear();
  printarPlacar();
  printf("\033[%d;%dH", (int)raquete.y, (int)raquete.x);
  for (int i = 0; i < RAQUETE_SIZE; i++) {
    printf("=");
  }
  printf("\033[%d;%dHo", (int)bola.y, (int)bola.x);

  Tijolo *current = tijolos;
  while (current) {
    if (current->durabilidade > 0) {
      printf("\033[%d;%dH[]", (int)current->y, (int)current->x);
    }
    current = current->prox;
  }
  screenUpdate();
}

void moverBola() {
  printf("\033[%d;%dH ", (int)bola.y, (int)bola.x);
  bola.x += bola.velX;
  bola.y += bola.velY;

  if (bola.x <= MIN_X || bola.x >= MAX_X) {
    bola.velX = -bola.velX;
  }
  if (bola.y <= MIN_Y) {
    bola.velY = -bola.velY;
  }
  if (bola.y == raquete.y - 1 && bola.x >= raquete.x &&
      bola.x <= raquete.x + RAQUETE_SIZE) {
    bola.velY = -bola.velY;
  }
  Tijolo *prev = NULL;
  Tijolo *current = tijolos;
  while (current) {
    if (bola.x >= current->x && bola.x <= current->x + 4 &&
        bola.y >= current->y && bola.y <= current->y + 1) {
      current->durabilidade--;
      bola.velY = -bola.velY;
      if (current->durabilidade == 0) {
        if (prev) {
          prev->prox = current->prox;
        } else {
          tijolos = current->prox;
        }
        free(current);
        placar += 5;
      }
      break;
    }
    prev = current;
    current = current->prox;
  }
}

void moverRaquete() {
  if (keyhit()) {
    int ch = readch();
    if (ch == '\033') {
      readch();
      switch (readch()) {
      case 'C':
        if (raquete.x < MAX_X - RAQUETE_SIZE)
          raquete.x += 2;
        break;
      case 'D':
        if (raquete.x > MIN_X)
          raquete.x -= 2;
        break;
      }
    }
  }
}

void inicializarTijolos() {
  int linhas = 3;
  int tijolos_por_linha = (MAX_X - 10) / 5;
  for (int linha = 0; linha < linhas; linha++) {
    for (int i = 0; i < tijolos_por_linha; i++) {
      Tijolo *novo = malloc(sizeof(Tijolo));
      if (novo) {
        novo->x = 5 + i * 5;
        novo->y = 2 + linha * 2;
        novo->durabilidade = 1;
        novo->prox = tijolos;
        tijolos = novo;
      }
    }
  }
}

void liberarTijolos() {
  while (tijolos) {
    Tijolo *temp = tijolos;
    tijolos = tijolos->prox;
    free(temp);
  }
}

void printarPlacar() { printf("\033[%d;%dHPlacar: %d", 0, 0, placar); }

void carregarHighScore() {
  FILE *file = fopen("highscore.txt", "r");
  if (file) {
    fscanf(file, "%d", &highScore);
    fclose(file);
  } else {
    highScore = 0;
  }
}

void salvarHighScore() {
  FILE *file = fopen("highscore.txt", "w");
  if (file) {
    if (placar > highScore) {
      highScore = placar;
    }
    fprintf(file, "%d", highScore);
    fclose(file);
  }
}

int main(void) {
  srand(time(NULL));
  inicializar();
  loopJogo();
  finalizar();
  if (tijolos == NULL || bola.y >= MAX_Y) {
    printf("Game Over. Placar final: %d\nHighscore: %d\n", placar, highScore);
  } else {
    printf("Parabéns, você venceu!\nPlacar final: %d\nHighscore: %d\n", placar, highScore);
  }
  return 0;
}
