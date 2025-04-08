#include "funcoes.h"

#define NOTE_A4  440
#define NOTE_A4S 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_D5  587
#define NOTE_E5  659
#define NOTE_F4  349
#define NOTE_F5  698
#define NOTE_G4  392
#define NOTE_G5  784

// --- Definições de variáveis globais ---
const int LED_PINS[NUM_LEDS] = {13, 12, 11, 10};
const int BUTTON_PINS[NUM_LEDS] = {7, 6, 5, 4};
const int buzzerPin = 2;
const int notas[NUM_LEDS] = {NOTE_A4, NOTE_A4, NOTE_F4, NOTE_C5}; 

const int tempoFeedback = 300;
const int tempoEntreLeds = 200;

EstadoJogo estadoJogo = INICIO;

int pontuacaoAtual = 0;
int maiorPontuacao = 0;
int sequenciaLed = 1;
const int enderecoEEPROM = 0;

String resposta = "";
String recebido = "";
bool modoMisterioso = false;

// --- Funções ---
void tocarMarchaImperial() {
  int melodia[] = {
    NOTE_A4, NOTE_A4, NOTE_A4, NOTE_F4, NOTE_C5, NOTE_A4,
    NOTE_F4, NOTE_C5, NOTE_A4,
    NOTE_E5, NOTE_E5, NOTE_E5, NOTE_F5, NOTE_C5, NOTE_G4,
    NOTE_F4, NOTE_C5, NOTE_A4
  };
  int duracoes[] = {
    500, 500, 500, 350, 150, 500,
    350, 150, 650,
    500, 500, 500, 350, 150, 500,
    350, 150, 650
  };

  for (int i = 0; i < 18; i++) {
    tone(buzzerPin, melodia[i], duracoes[i]);
    digitalWrite(LED_PINS[i % NUM_LEDS], HIGH);
    delay(duracoes[i] * 1.3);
    digitalWrite(LED_PINS[i % NUM_LEDS], LOW);
  }
  noTone(buzzerPin);
}

void iniciarJogo() {
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    pinMode(BUTTON_PINS[i], INPUT_PULLUP);
  }
  pinMode(buzzerPin, OUTPUT);

  maiorPontuacao = EEPROM.read(enderecoEEPROM);
  if (maiorPontuacao == 255) maiorPontuacao = 0;
  Serial.println("Bem-vindo ao Genius!");
  Serial.print("Maior pontuacao registrada: ");
  Serial.println(maiorPontuacao);
  Serial.println("Digite '1' para comecar ou '2' para o modo misterioso.");
}

void atualizarEstadoJogo() {
  if (Serial.available()) {
    char entrada = Serial.read();
    if (entrada == '1') {
      estadoJogo = PLAY_GAME;
      pontuacaoAtual = 0;
      sequenciaLed = 1;
      modoMisterioso = false;

      Serial.println("\nPrepare-se! A batalha vai comecar!");
      tocarMarchaImperial();
    } else if (entrada == '2') {
      estadoJogo = MODO_MISTERIOSO;
      pontuacaoAtual = 0;
      sequenciaLed = 1;
      modoMisterioso = true;

      Serial.println("\nModo Misterioso ativado. Prepare-se!");
      tocarMarchaImperial();
    }
  }

  switch (estadoJogo) {
    case PLAY_GAME:
    case MODO_MISTERIOSO:
      Serial.print("Nivel: ");
      Serial.println(sequenciaLed);

      if (modoMisterioso && sequenciaLed == 1) {
        apresentarSonsSemLeds();
      }

      resposta = gerarSequenciaAleatoria(sequenciaLed);
      if (modoMisterioso) {
        tocarSequenciaSemLeds(resposta);
      } else {
        mostrarSequencia(resposta);
      }

      recebido = lerSequenciaJogador(sequenciaLed);

      if (recebido == resposta) {
        int pontosGanhos = pow(2, sequenciaLed - 1);
        pontuacaoAtual += pontosGanhos;
        Serial.print("Correto! Pontos ganhos: ");
        Serial.println(pontosGanhos);
        Serial.print("Pontuacao atual: ");
        Serial.println(pontuacaoAtual);

        if (pontuacaoAtual > maiorPontuacao) {
          maiorPontuacao = pontuacaoAtual;
          EEPROM.write(enderecoEEPROM, maiorPontuacao);
          Serial.println("🎉 Novo recorde atingido!");
        }

        sequenciaLed++;
      } else {
        gameOver();
      }
      break;

    case GAME_OVER:
      Serial.println("Jogo encerrado. Digite '1' para jogar novamente ou '2' para o modo misterioso.");
      estadoJogo = INICIO;
      break;

    default:
      break;
  }
}

void tocarSom(int nota) {
  tone(buzzerPin, nota, tempoFeedback);
}

void mostrarSequencia(String sequencia) {
  for (int i = 0; i < sequencia.length(); i++) {
    int indice = sequencia[i] - '1';
    digitalWrite(LED_PINS[indice], HIGH);
    tocarSom(notas[indice]);
    delay(tempoFeedback);
    digitalWrite(LED_PINS[indice], LOW);
    delay(tempoEntreLeds);
  }
}

void tocarSequenciaSemLeds(String sequencia) {
  for (int i = 0; i < sequencia.length(); i++) {
    int indice = sequencia[i] - '1';
    tocarSom(notas[indice]);
    delay(tempoFeedback + 200);
  }
}

void apresentarSonsSemLeds() {
  Serial.println("Modo Misterioso: veja os sons dos LEDs");
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(LED_PINS[i], HIGH);
    tone(buzzerPin, notas[i], 500);
    delay(600);
    digitalWrite(LED_PINS[i], LOW);
    delay(300);
  }
  noTone(buzzerPin);
  Serial.println("A sequência será apenas sonora. Prepare-se!");
  delay(1000);
}

String gerarSequenciaAleatoria(int tamanho) {
  String sequencia = "";
  for (int i = 0; i < tamanho; i++) {
    sequencia += String(random(1, 5));
  }
  return sequencia;
}

String lerSequenciaJogador(int tamanhoSequencia) {
  String respostaJogador = "";
  while (respostaJogador.length() < tamanhoSequencia) {
    for (int i = 0; i < NUM_LEDS; i++) {
      if (digitalRead(BUTTON_PINS[i]) == LOW) {
        while (digitalRead(BUTTON_PINS[i]) == LOW);
        respostaJogador += String(i + 1);
        tocarSom(notas[i]);
        delay(tempoFeedback);
      }
    }
  }
  return respostaJogador;
}

void gameOver() {
  tocarMelodiaErro();
  Serial.println("❌ Sequência incorreta. Fim de jogo.");
  Serial.print("Sua pontuacao final: ");
  Serial.println(pontuacaoAtual);
  Serial.print("Recorde atual: ");
  Serial.println(maiorPontuacao);
  estadoJogo = GAME_OVER;
}

void tocarMelodiaErro() {
  int melodia[] = {
    NOTE_E5, NOTE_D5, NOTE_C5, NOTE_B4, NOTE_A4
  };
  int duracoes[] = {
    150, 150, 150, 150, 300
  };

  for (int i = 0; i < 5; i++) {
    tone(buzzerPin, melodia[i], duracoes[i]);
    delay(duracoes[i] * 1.1); // Pequeno delay entre as notas
  }

  noTone(buzzerPin);
}