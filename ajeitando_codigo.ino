#include <AFMotor.h>

//=== Definições para as marchas ===
#define N_MARCHAS 5
#define MIN_VEL 105
//==================================

//=== Definições para o sensor =====
#define DISTANCIA_MIN 30
// portas
#define ECHO 39
#define TRIG 38
#define PARAM_DIV_SENSOR 58.2
//==================================

//=== Definições para curvas =======
#define TEMPO_CURVA_ANG_M1 3.17
#define ESQUERDA 0
#define DIREITA 1
//==================================

//=== Definições dos motores =======
#define PORTA_MOT_E 2
#define PORTA_MOT_D 1
//==================================

// === Definindo as funções usadas =========================================

/**
 * @brief Incrementa ou decrementa a marcha atual.
 *
 * Vai da marcha atual até a marcha destino de um em um recursivamente
 * cada troca de marcha faz a velocidade variar em ((255 - VEL_MIN) / N_MARCHAS) para
 * cima ou para baixo dependendo se incrementa ou decrementa a marcha.
 *
 * @param destino Valor da marcha que se quer chegar (entre [0 - N_MARCHAS])
*/
void troca_marcha(uint8_t destino);

/**
 * @brief Atualiza a velocidade e a direcao de ambos os motores
 *
 * @warning set_motores é uma função usada para implementar outras funções, usa-la diretamente no codigo de loop() pode gerar problemas.
 *  
 * @param velocidade a nova velocidade dos motores
 * @param direcao a direcao dos motores (FORWARD, BACKWARD ou RELEASE) (ignorada se velocidade = 0)
 *
*/
void set_motores(uint8_t velocidade, int direcao);

/**
 * @brief Faz uma curva em um angulo qualquer
 *
 * faz uma curva em um angulo e direcao passados por parametros
 * como o depende da potencia no motor, o calculo é aproximado e
 * supoe que ele esta em uma marcha 1. 
 *
 * @param direcao a direcao da curva (DIREITA ou ESQUERDA).
 * @param angulo o angulo de rotacao da curva. (depende da macro TEMPO_CURVA_ANG_M1)
*/
void curva(int direcao, int angulo);

/**
 * @brief Retorna a distancia lida pelo sensor.
 *
 * Com o sensor de profundidade calcula e ja divide pelo PARAM_DIV_SENSOR
 * retornando a distancia aproximada em centimetros do objeto mais proximo
 * na linha do sensor 
 *
 * @return um valor entre [5 - 250] estimativamente ou um valor negativo, caso esteja fora do alcançe.
*/
int confere_distancia();
void da_partida();
void desliga();
// =========================================================================

//=== Definindo variaveis de escopo global =================================
AF_DCMotor motor_esq(PORTA_MOT_E);
AF_DCMotor motor_dir(PORTA_MOT_D);
uint8_t incremento_marcha = (uint8_t) (255 - MIN_VEL) / N_MARCHAS;
int distancia;
uint8_t velocidade = 0;
uint8_t marcha = 0;
//==========================================================================

void setup() 
{
  //definindo as portas de entrada e saida.
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
}

void loop() 
{

  da_partida();

  int dist = confere_distancia();
  if (dist < DISTANCIA_MIN)
  {
    curva(ESQUERDA, 60);
    int esq = confere_distancia();
    delay(30);
    curva(DIREITA, 120);
    int dir = confere_distancia();
    delay(30);
    curva(ESQUERDA, 60);
    if (esq > dir && esq > DISTANCIA_MIN)
      curva(DIREITA, 90);  
    else if (esq < dir && dir > DISTANCIA_MIN)
      curva(ESQUERDA, 90);
    else
      curva(DIREITA, 180);
  }
  
}

int confere_distancia()
{
  digitalWrite(TRIG, HIGH);
  delay(1);
  digitalWrite(TRIG, LOW);
  return ( pulseIn(ECHO, HIGH) / PARAM_DIV_SENSOR ); 
}

void da_partida()
{
  atual = 1;
  velocidade = MIN_VEL;
  set_motores(velocidade, FORWARD);
}

void desliga()
{
  troca_marcha(1);
  atual = 0;
  velocidade = 0;
  set_motores(velocidade, RELEASE);
}

void troca_marcha(uint8_t destino)
{
  if (atual < destino && destino <= N_MARCHAS)
  {
    //acelera aos pucos até o valor da proxima marcha
    uint8_t i;
    for (i = velocidade ; i < velocidade + incremento_marcha ; i++)
    {
      set_motores(i, FORWARD);
      delay(2);
    }
    //atualiza os parametros de referencia
    velocidade = i;
    atual++;

    // recursao para saltar mais de uma marcha
    troca_marcha(atual, destino, velocidade);
  }
  
  else if (atual > destino && destino >= 1)
  {
    //desacelera aos pucos até o valor da marcha anterior
    uint8_t i;
    for (i = velocidade ; i > velocidade - incremento_marcha ; i--)
    {
      set_motores(i, FORWARD);
      delay(2);
    }
    //atualiza os parametros de referencia
    velocidade = i;
    atual--;

    // recursao para saltar mais de uma marcha
    troca_marcha(atual, destino, velocidade);
  }
}

void set_motores(uint8_t velocidade, int direcao)
{
  motor_esq.setSpeed(velocidade);
  motor_dir.setSpeed(velocidade);
  motor_esq.run(direcao);
  motor_dir.run(direcao);
}

void curva(int direcao, int angulo, uint8_t* atual, uint8_t* velocidade)
{
   troca_marcha(1);
   
   if (direcao == ESQUERDA)
   {
     motor_esq.run(FORWARD);
     motor_dir.run(BACKWARD);
   }
   else if (direcao == DIREITA)
   {
     motor_esq.run(BACKWARD);
     motor_dir.run(FORWARD);
   }
   delay(angulo * TEMPO_CURVA_ANG_M1);
}
