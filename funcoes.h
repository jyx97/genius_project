#ifndef FUNCOES_H
#define FUNCOES_H

#include <EEPROM.h>
#include <math.h>  // Necessário para pow()

// Constantes
#define NUM_LEDS 4

// Pinos
extern const int LED_PINS[NUM_LEDS];
extern const int BUTTON_PINS[NUM_LEDS];
extern const int buzzerPin;

// Notas musicais
extern const int notas[NUM_LEDS];

// Tempos
extern const int tempoFeedback;
extern const int tempoEntreLeds;

// Estados do jogo
enum EstadoJogo {
  INICIO,
  PLAY_GAME,
  GAME_OVER,
  MODO_MISTERIOSO
};

extern EstadoJogo estadoJogo;

// Variáveis de jogo
extern int pontuacaoAtual;
extern int maiorPontuacao;
extern int sequenciaLed;
extern const int enderecoEEPROM;
extern String resposta;
extern String recebido;
extern bool modoMisterioso;

// Funções
void iniciarJogo();
void atualizarEstadoJogo();
void tocarSom(int nota);
void mostrarSequencia(String sequencia); // Corrigido
void tocarSequenciaSemLeds(String sequencia); // Adicionado
void apresentarSonsSemLeds(); // Adicionado
String gerarSequenciaAleatoria(int tamanho);
String lerSequenciaJogador(int tamanho);
void gameOver();
void tocarMelodiaErro();

#endif
