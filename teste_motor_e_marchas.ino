#include <AFMotor.h>

// numero de marchas definidas
#define N_MARCHAS 5

//definindo as portas usadas
#define PORTA_MOT_E 1
#define PORTA_MOT_D 2

//definindo esquerda e direita pra curvas
#define DIREITA 1
#define ESQUERDA 0

//definindo motores como globais
AF_DCMotor* motor_esq;
AF_DCMotor* motor_dir;

uint8_t incremento_marcha; 

void setup() 
{
  motor_esq = new AF_DCMotor(PORTA_MOT_E);
  motor_dir = new AF_DCMotor(PORTA_MOT_D);

  incremento_marcha = (uint8_t) 255 / N_MARCHAS;
}

void loop() 
{
  uint8_t velocidade = 0;
  uint8_t marcha = 0;

  delay (10000);

  troca_marcha(&marcha, 2, &velocidade);

  delay (5000);

  troca_marcha(&marcha, 0, &velocidade);

  curva(DIREITA, &marcha, &velocidade);
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
      delay(10);
    }
    //atualiza os parametros de referencia
    *velocidade = i;
    (*atual)++;

    // recursao para saltar mais de uma marcha
    delay(1000);
    troca_marcha(atual, destino, velocidade);
  }
  
  else if (*atual > destino)
  {
    //acelera aos pucos até o valor da proxima marcha
    uint8_t i;
    for (i = *velocidade ; i > *velocidade - incremento_marcha ; i--)
    {
      set_motores(i, FORWARD);
      delay(10);
    }
    //atualiza os parametros de referencia
    *velocidade = i;
    (*atual)--;

    // recursao para saltar mais de uma marcha
    delay(1000);
    troca_marcha(atual, destino, velocidade);
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
    motor_esq -> setSpeed(velocidade);
    motor_dir -> setSpeed(velocidade);
    motor_esq -> run(RELEASE);
    motor_dir -> run(RELEASE);
  }
  else
  {
    motor_esq -> setSpeed(velocidade);
    motor_dir -> setSpeed(velocidade);
    motor_esq -> run(direcao);
    motor_dir -> run(direcao);
  }
}

void curva(uint8_t direcao, uint8_t* marcha, uint8_t* velocidade)
{
    troca_marcha(marcha, 1, velocidade);
    if (direcao == ESQUERDA)
    {
        motor_esq -> run(BACKWARD);
        motor_dir -> run(FORWARD);
    }
    else if (direcao == DIREITA)
    {
        motor_dir -> run(BACKWARD);
        motor_esq -> run(FORWARD);
    }
    delay(200);
   troca_marcha (marcha, 0, velocidade);  
}
