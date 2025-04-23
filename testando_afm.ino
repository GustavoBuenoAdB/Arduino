#include <AFMotor.h>

// definicoes para as marchas
#define N_MARCHAS 5
#define MIN_VEL 105

// definicoes para o sensor
#define DISTANCIA_MIN 30
#define ECHO 39
#define TRIG 38
#define PARAM_DIV_SENSOR 58.2

// definicoes do botao
#define PORTA_BOTAO 30

//definicoes para curvas
#define TEMPO_CURVA_ANG_M1 3.17
#define ESQUERDA 0
#define DIREITA 1

//definindo as portas usadas
#define PORTA_MOT_E 2
#define PORTA_MOT_D 1

//definindo motores como globais
AF_DCMotor motor_esq(PORTA_MOT_E);
AF_DCMotor motor_dir(PORTA_MOT_D);


// variaveis globais.

uint8_t incremento_marcha; 
int distancia;

void troca_marcha(uint8_t* atual, uint8_t destino, uint8_t* velocidade);
void set_motores(uint8_t velocidade, int direcao);
void curva(int direcao, int angulo);
int confere_distancia();

void setup() 
{
  incremento_marcha = (uint8_t) (255 - MIN_VEL) / N_MARCHAS;
  //pinMode(PORTA_BOTAO, INPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
}

void loop() 
{
  uint8_t velocidade = MIN_VEL;
  uint8_t marcha = 1;

  set_motores(velocidade, FORWARD);

  //curva(DIREITA, 360);

  int dist = confere_distancia();
  if (dist < DISTANCIA_MIN)
  {
    curva(ESQUERDA, 60, &marcha, &velocidade);
    int esq = confere_distancia();
    delay(30);
    curva(DIREITA, 120, &marcha, &velocidade);
    int dir = confere_distancia();
    delay(30);
    curva(ESQUERDA, 60, &marcha, &velocidade);
    if (esq > dir && esq > DISTANCIA_MIN)
      curva(DIREITA, 90, &marcha, &velocidade);  
    else if (esq < dir && dir > DISTANCIA_MIN)
      curva(ESQUERDA, 90, &marcha, &velocidade);
    else
      curva(DIREITA, 180, &marcha, &velocidade);
  }

  //set_motores(0,FORWARD);

  //delay (10000);
  
 
  troca_marcha(&marcha, 4, &velocidade);

  
}

int confere_distancia()
{
  
  digitalWrite(TRIG, HIGH);
  delay(1);
  digitalWrite(TRIG, LOW);
  return ( pulseIn(ECHO, HIGH) / PARAM_DIV_SENSOR ); 
}

/**
 * @brief Troca de marcha, para cima ou para baixo.
 *
 * vai da marcha atual até a marcha destino de um em um recursivamente
 * cada troca de marcha faz a velocidade variar em (255 / N_MARCHAS) para
 * cima ou para baixo dependendo se incrementa ou decrementa a marcha.
 *
 * @param atual o endereço da variavel da marcha atual do robô
 * @param destino o valor da marcha que se quer chegar (entre [0 - N_MARCHAS])
 * @param velocidade o endereço da variavel velocidade para manter ela atualizada.
 *
*/
void troca_marcha(uint8_t* atual, uint8_t destino, uint8_t* velocidade)
{
  if (*atual < destino && destino <= N_MARCHAS)
  {
    //acelera aos pucos até o valor da proxima marcha
    uint8_t i;
    for (i = *velocidade ; i < *velocidade + incremento_marcha ; i++)
    {
      set_motores(i, FORWARD);
      delay(2);
    }
    //atualiza os parametros de referencia
    *velocidade = i;
    (*atual)++;

    // recursao para saltar mais de uma marcha
    //delay(1000);
    troca_marcha(atual, destino, velocidade);
  }
  
  else if (*atual > destino && destino >= 1)
  {
    //desacelera aos pucos até o valor da marcha anterior
    uint8_t i;
    for (i = *velocidade ; i > *velocidade - incremento_marcha ; i--)
    {
      set_motores(i, FORWARD);
      delay(2);
    }
    //atualiza os parametros de referencia
    *velocidade = i;
    (*atual)--;

    // recursao para saltar mais de uma marcha
    //delay(1000);
    troca_marcha(atual, 3, velocidade);
  }
}

/**
 * @brief atualiza a velocidade e a direcao de ambos os motores
 *
 * @param velocidade a nova velocidade dos motores
 * @param direcao a direcao dos motores (FORWARD, BACKWARD ou RELEASE) (ignorada se velocidade = 0)
 *
*/
void set_motores(uint8_t velocidade, int direcao)
{
  if (velocidade == 0)
  {
    motor_esq.setSpeed(velocidade);
    motor_dir.setSpeed(velocidade);
    motor_esq.run(RELEASE);
    motor_dir.run(RELEASE);
  }
  else
  {
    motor_esq.setSpeed(velocidade);
    motor_dir.setSpeed(velocidade);
    motor_esq.run(direcao);
    motor_dir.run(direcao);
  }
}


/**
 * @brief faz uma curva em um angulo qualquer
 *
 * faz uma curva em um angulo e direcao passados por parametros
 * @warning o calculo do angulo supoe que ele esta em uma marcha 1. 
 *
 * @param direcao a direcao da curva (DIREITA ou ESQUERDA).
 * @param angulo o angulo de rotacao da curva. (depende da macro TEMPO_CURVA_ANG_M1)
*/
void curva(int direcao, int angulo, uint8_t* atual, uint8_t* velocidade)
{
   troca_marcha(atual, 1, velocidade);
   motor_esq.setSpeed((*velocidade));
   motor_dir.setSpeed((*velocidade));
   
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
