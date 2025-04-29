#include <AFMotor.h>
#include "robo_desviador.h"

//=== Definindo variaveis de escopo global =================================
AF_DCMotor Motor_esq(PORTA_MOT_E);
AF_DCMotor Motor_dir(PORTA_MOT_D);
//valor alterado a cada troca de marcha
uint8_t incremento_marcha = (uint8_t) (255 - MIN_VEL) / N_MARCHAS;
// distancia captada pelo sensor
int distancia;
uint8_t marcha = 0;
//contador de tiks de cada motor
volatile int cont_dir = 0;
volatile int cont_esq = 0;
//velocidade ideal aplicada nos motores
uint8_t velocidade = 0;
//velocidade real dos motores (em %)
double vel_motor_dir_real = 1.0;
double vel_motor_esq_real = 1.0;
//controle para testes de tempo
long int ms_ultima_alinhada = 0;
//==========================================================================

void setup() 
{
  //definindo as portas de entrada e saida.
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(PINO_ENC_ESQ, INPUT);
  pinMode(PINO_ENC_DIR, INPUT);

  //iniciando as interrupcoes
  atattchinterrupt(5, interrupcao_esq, FALLING);
  atattchinterrupt(4, interrupcao_dir, FALLING);

  //iniciando os motores
  da_partida();
  alinhando_motores_antes();
}

void loop() 
{
  //atualiza a distancia
  confere_distancia();
  
  if(distancia < DISTANCIA_MIN)
    curva(testa_curva(), 90);

  anda(FORWARD);

  // alinhando os motores com encoders
  if ((miliseg() - ms_ultima_alinhada) > MS_POR_ALINHADA)
    alinha_motores();
}

alinhando_motores_antes()
{
  double esq_real = 0;
  double dir_real = 0;
  while (esq_real != vel_motor_esq_real && dir_real != vel_motor_dir_real)
  {
    double esq_real = vel_motor_esq_real;
    double dir_real = vel_motor_dir_real;
      
    anda(FORWARD);
    delay(MS_POR_ALINHADA);
    alinha_motores();

    anda(BACKWARD);
    delay(MS_POR_ALINHADA);
    alinha_motores();
  }
}

int testa_curva()
{
  curva(ESQUERDA, 60);
  int esq = confere_distancia();
  
  curva(DIREITA, 120);
  int dir = confere_distancia();
  
  curva(ESQUERDA, 60);
  
  
  if (esq > dir && esq > DISTANCIA_MIN)
    return ESQUERDA;  
  else if (esq < dir && dir > DISTANCIA_MIN)
    return DIREITA
  else
  {
    curva(DIREITA, 90);
    return DIREITA;
  }
}

void alinha_motores()
{
  double rpt_esq = cont_esq / MS_POR_ALINHADA;
  double rpt_dir = cont_dir / MS_POR_ALINHADA;
  cont_esq = 0;
  cont_dir = 0;

  if (rpt_esq > rpt_dir)
  {
    if (vel_motor_dir_real < 1.0)
      vel_motor_dir_real += VEL_REAL_INC;
    else if (vel_motor_esq_real > VEL_REAL_INC)
      vel_motor_esq_real -= VEL_REAL_INC;
  }
  else if (rpt_esq < rpt_dir) 
  {
    if (vel_motor_esq_real < 1.0)
      vel_motor_esq_real += VEL_REAL_INC;
    else if (vel_motor_dir_real > VEL_REAL_INC)
      vel_motor_dir_real -= VEL_REAL_INC;

  }
  ms_ultima_alinhada = miliseg();
}

int confere_distancia()
{
  digitalWrite(TRIG, HIGH);
  delay(1);
  digitalWrite(TRIG, LOW);
  distancia = ( pulseIn(ECHO, HIGH) / PARAM_DIV_SENSOR );
  return distancia; 
}

void da_partida()
{
  atual = 1;
  velocidade = MIN_VEL;
  set_motores(velocidade, FORWARD, FORWARD);
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

void set_motores(uint8_t velocidade, int direcao_esq, int direcao_dir)
{
  Motor_esq.setSpeed(velocidade * vel_motor_esq_real);
  Motor_dir.setSpeed(velocidade * vel_motor_dir_real);
  Motor_esq.run(direcao_esq);
  Motor_dir.run(direcao_dir);
}

void curva(int direcao, double angulo)
{
  if (SEGURANCA)
    troca_marcha(1);
   
  if (direcao == ESQUERDA)
    set_motores(velocidade, FORWARD, BACKWARD);
  else if (direcao == DIREITA)
    set_motores(velocidade, BACKWARD, FORWARD);
}

void anda(int direcao)
{
  set_motores(velocidade, direcao, direcao);
}

void interrupcao_esq()
{
  cont_esq++;
}

void interrupcao_dir()
{
  cont_dir++;
}
