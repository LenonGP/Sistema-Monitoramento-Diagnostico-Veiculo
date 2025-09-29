// ===================================================================================
// == Projeto: Sistema de Monitoramento e Diagnóstico Veicular para Arduino         ==
// == Versão: Display LCD Paralelo (Compatível com SimulIDE)                        ==
// ===================================================================================

// 1. INCLUSÃO DE BIBLIOTECAS
// ==========================
#include <LiquidCrystal.h>      // Biblioteca padrão para Displays LCD paralelos

// 2. DEFINIÇÕES E CONSTANTES
// ==========================
// --- Pinos do Display LCD Paralelo ---
// LiquidCrystal(rs, enable, d4, d5, d6, d7)
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

// --- Pinos dos Sensores e Atuadores ---
const int PINO_SENSOR_TEMP = A0;      // Pino analógico para o sensor de temperatura (potenciômetro)
const int PINO_SENSOR_PRESSAO = A1;   // Pino analógico para o sensor de pressão (potenciômetro)
const int PINO_SENSOR_RPM = 2;        // Pino digital para o botão de RPM (usando interrupção)
const int PINO_LED_ALERTA_TEMP = 4;   // Pino para o LED vermelho (superaquecimento)
const int PINO_LED_ALERTA_PRESSAO = 5;// Pino para o LED amarelo (pressão baixa)
const int PINO_BUZZER = 6;            // Pino para o buzzer de alerta

// --- Limites para os Alertas ---
const float TEMP_MAXIMA_ALERTA = 90.0;  // Temperatura em °C para acionar o alerta
const float PRESSAO_MINIMA_ALERTA = 20.0; // Pressão (simulada) para acionar o alerta

// 3. VARIÁVEIS GLOBAIS
// ================================
// --- Variáveis para os Sensores ---
float temperatura = 0.0;
float pressao = 0.0;

// --- Variáveis para o cálculo de RPM ---
// 'volatile' é essencial pois a variável é modificada dentro de uma interrupção
volatile unsigned long contadorPulsosRPM = 0;
unsigned long tempoAnteriorRPM = 0;
int rpm = 0;

// 4. FUNÇÃO DE CONFIGURAÇÃO (SETUP)
// =================================
void setup() {
  // --- Inicialização da Comunicação Serial (UART) ---
  Serial.begin(9600);
  Serial.println("== Sistema de Monitoramento Veicular INICIADO ==");
  Serial.println("Usando Display LCD Paralelo");

  // --- Inicialização do Display LCD Paralelo ---
  lcd.begin(16, 2); // Inicia o display com 16 colunas e 2 linhas
  lcd.print("Iniciando...");
  delay(1000);

  // --- Configuração dos Pinos (pinMode) ---
  pinMode(PINO_LED_ALERTA_TEMP, OUTPUT);
  pinMode(PINO_LED_ALERTA_PRESSAO, OUTPUT);
  pinMode(PINO_BUZZER, OUTPUT);
  pinMode(PINO_SENSOR_RPM, INPUT_PULLUP); // Ativa resistor de pull-up interno para o botão

  // --- Configuração da Interrupção para o RPM ---
  attachInterrupt(digitalPinToInterrupt(PINO_SENSOR_RPM), contarPulso, FALLING);
}

// 5. LOOP PRINCIPAL
// =================
void loop() {
  // --- Leitura e Processamento dos Dados ---
  lerTemperatura();
  lerPressao();
  calcularRPM();

  // --- Lógica de Controle ---
  atualizarDisplay();
  verificarAlertas();
  enviarDadosSerial();

  // --- Delay para não sobrecarregar o loop ---
  delay(500); // Atualiza os dados a cada meio segundo
}

// 6. FUNÇÕES AUXILIARES
// =====================

// --- Função para ler e converter a temperatura ---
void lerTemperatura() {
  int leituraADC = analogRead(PINO_SENSOR_TEMP);
  float tensao = leituraADC * (5.0 / 1023.0);
  temperatura = tensao * 100.0; // Fórmula para sensor LM35. Ajuste se necessário.
}

// --- Função para ler e converter a pressão (simulada) ---
void lerPressao() {
  int leituraADC = analogRead(PINO_SENSOR_PRESSAO);
  pressao = map(leituraADC, 0, 1023, 0, 100); // Mapeia a leitura para 0-100 PSI
}

// --- Função para calcular o RPM ---
void calcularRPM() {
  // A cada 1 segundo, calcula o RPM baseado nos pulsos contados
  if (millis() - tempoAnteriorRPM >= 1000) {
    noInterrupts(); // Pausa as interrupções para ler a variável com segurança
    rpm = contadorPulsosRPM * 60; // (pulsos por segundo) * 60 = pulsos por minuto
    contadorPulsosRPM = 0; // Zera o contador para a próxima medição
    interrupts(); // Reativa as interrupções
    
    tempoAnteriorRPM = millis(); // Salva o tempo da última medição
  }
}

// --- Função chamada pela interrupção a cada pulso do RPM ---
void contarPulso() {
  contadorPulsosRPM++;
}

// --- Função para atualizar as informações no Display LCD ---
void atualizarDisplay() {
  lcd.clear();
  // Linha 1: Temperatura e Pressão
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperatura, 1); // Mostra com 1 casa decimal
  lcd.print("C ");
  lcd.setCursor(8, 0);
  lcd.print("P:");
  lcd.print((int)pressao); // Mostra como inteiro
  lcd.print("PSI");

  // Linha 2: RPM
  lcd.setCursor(0, 1);
  lcd.print("RPM: ");
  lcd.print(rpm);
}

// --- Função para verificar e ativar os alertas ---
void verificarAlertas() {
  bool alertaAtivo = false;

  // Alerta de Superaquecimento
  if (temperatura > TEMP_MAXIMA_ALERTA) {
    digitalWrite(PINO_LED_ALERTA_TEMP, HIGH);
    alertaAtivo = true;
  } else {
    digitalWrite(PINO_LED_ALERTA_TEMP, LOW);
  }

  // Alerta de Pressão Baixa
  if (pressao < PRESSAO_MINIMA_ALERTA) {
    digitalWrite(PINO_LED_ALERTA_PRESSAO, HIGH);
    alertaAtivo = true;
  } else {
    digitalWrite(PINO_LED_ALERTA_PRESSAO, LOW);
  }

  // Ativa o buzzer se qualquer alerta estiver ativo
  if (alertaAtivo) {
    tone(PINO_BUZZER, 1500, 200); // Toca um som de 1500Hz por 200ms
  } else {
    noTone(PINO_BUZZER);
  }
}

// --- Função para enviar os dados via Serial para o computador ---
void enviarDadosSerial() {
  Serial.print("DIAGNOSTICO -> ");
  Serial.print("Temp: ");
  Serial.print(temperatura);
  Serial.print(" C, ");
  Serial.print("Pressao: ");
  Serial.print(pressao);
  Serial.print(" PSI, ");
  Serial.print("RPM: ");
  Serial.println(rpm);
}