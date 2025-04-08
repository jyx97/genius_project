#include "funcoes.h"

void setup() {
  Serial.begin(9600);
  iniciarJogo();
}

void loop() {
  atualizarEstadoJogo();
}
