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

//=== Definições dos encoders ======
#define PINO_ENC_ESQ 18 // interrupcao 5
#define PINO_ENC_DIR 19 // interrupcao 4
#define N_ENC_POR_VOLTA 20 //20 pra falling
#define DIST_POR_ENC 1.078
#define ANG_POR_ENC 9.5071
#define VAL_ERRO_ENC_1 0.0894
//==================================

//=== Definições do comportamento ==
#define SEGURANCA 1 // faz curva só na marcha 1
//==================================


// === Definindo as funções usadas =========================================

/**
 * @brief Inicia os 2 motores em marcha 1.
 *
 * inicia os valores base de velocidade e de marcha da maneira correta.
*/
void da_partida();

/**
 * @brief Desliga os 2 motores.
 *
 * desliga e reinicia os valores bases da maneira correta para a proxima partida.
*/
void desliga();

/**
 * @brief Anda pra frente ou para traz uma distancia X
 *
 * Anda pra frente ou para traz uma distancia de X cm passada por parametro
 * a distancia realmente percorrida é aproximada para valores multiplos de DIST_POR_ENC
 * 
 * @note a distancia independe da marcha atual. 
 *
 * @param direcao A direcao para qual andar (FORWARD ou BACKWARD).
 * @param centimetros A distancia a ser percorrida em cm. (essa distancia sera arredondado para um multiplo de DIST_POR_ENC).
*/
void anda(int direcao, double centimetros);

/**
 * @brief Faz uma curva em um angulo escolhido
 *
 * faz uma curva em um angulo (nao negativo) e direcao passados por parametros
 * o angulo realmente virado é aproximado para valores multiplos de ANG_POR_ENC
 * 
 * @note com SEGURANCA = 1 a funcao troca pra marcha 1 antes de virar. 
 *
 * @param direcao a direcao da curva (DIREITA ou ESQUERDA).
 * @param angulo o angulo de rotacao da curva. (esse angulo sera arredondado para um multiplo de ANG_POR_ENC)
*/
void curva(int direcao, double angulo);

/**
 * @brief Incrementa ou decrementa a marcha atual.
 *
 * Vai da marcha atual até a marcha destino de um em um recursivamente
 * cada troca de marcha faz a velocidade variar em ((255 - VEL_MIN) / N_MARCHAS) para
 * cima ou para baixo dependendo se incrementa ou decrementa a marcha.
 *
 * @param destino Valor da marcha que se quer chegar (entre [0 - N_MARCHAS])
*/
void set_marcha(uint8_t destino);

/**
 * @brief Atualiza a velocidade e a direcao de ambos os motores
 *
 * @warning set_motores é uma função usada para implementar outras funções, usa-la diretamente no codigo de loop() pode gerar problemas.
 *  
 * @param velocidade a nova velocidade dos motores
 * @param direcao_esq a direcao do motor esquerdo (FORWARD, BACKWARD ou RELEASE)
 * @param direcao_dir a direcao do motor direito (FORWARD, BACKWARD ou RELEASE)
*/
void set_motores(uint8_t velocidade, int direcao_esq, int direcao_dir);

/**
 * @brief Atualiza a os contadores dos encoders de cada motor
 *
 * @warning set_contadores é uma função usada para implementar outras funções, usa-la diretamente no codigo de loop() pode gerar problemas.
 *  
 * @param esq O valor do contador do motor esquerdo.
 * @param dir O valor do contador do motor direito.
*/
void set_contadores(int esq, int dir);

/**
 * @brief Função chamada pela interrupção do encoder esquerdo.
 *
 * atualiza o contador do encoder esquerdo e tenta tratar o
 * erro de sincronia caso ele seja detectado.
 * 
 * @warning interrupcao_esq é uma função usada para implementar outras funções, usa-la diretamente no codigo de loop() pode gerar problemas.
*/
void interrupcao_esq();

/**
 * @brief Função chamada pela interrupção do encoder direito.
 *
 * atualiza o contador do encoder direito e tenta tratar o 
 * erro de sincronia caso ele seja detectado.
 * 
 * @warning interrupcao_dir é uma função usada para implementar outras funções, usa-la diretamente no codigo de loop() pode gerar problemas.
*/
void interrupcao_dir();

/**
 * @brief Retorna a distancia lida pelo sensor.
 *
 * Com o sensor de profundidade calcula e ja divide pelo PARAM_DIV_SENSOR
 * retornando a distancia aproximada em centimetros do objeto mais proximo
 * na linha do sensor e atualizando a variavel global distancia.
 *
 * @return um valor entre [5 - 250] estimativamente ou um valor negativo, caso esteja fora do alcançe.
*/
int confere_distancia();
// =========================================================================

//=== Definindo variaveis de escopo global =================================
AF_DCMotor Motor_esq(PORTA_MOT_E);
AF_DCMotor Motor_dir(PORTA_MOT_D);
//valor alterado a cada troca de marcha
uint8_t incremento_marcha = (uint8_t) (255 - MIN_VEL) / N_MARCHAS;
// distancia captada pelo sensor
int distancia;
uint8_t velocidade = 0;
uint8_t marcha = 0;
//contador de sinais que cada encoder de motor deve dar antes de parar
volatile int cont_dir = 0;
volatile int cont_esq = 0;
volatile double erro_acumulado = 0;
//==========================================================================

void setup() 
{
  //definindo as portas de entrada e saida.
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(PINO_ENC_ESQ, INPUT);
  pinMode(PINO_ENC_DIR, INPUT);
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
  Motor_esq.setSpeed(velocidade);
  Motor_dir.setSpeed(velocidade);
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

  set_contadores( (int) angulo/ANG_POR_ENC , (int) angulo/ANG_POR_ENC );
}

void anda(int direcao, double centimetros)
{
  set_motores(velocidade, direcao, direcao);
  set_contadores((int) centimetros / DIST_POR_ENC , (int) centimetros / DIST_POR_ENC);
}

void set_contadores(int esq, int dir)
{
  cont_dir = dir;
  cont_esq = esq;
}

void interrupcao_esq()
{
  if (cont_esq > 0)
    cont_esq--;
  else
  {
    Motor_esq.run(RELEASE);
    if (cont_dir > 0)
      // Quase impossivel rodar isso pq VAL_ERRO_ENC_1 maximo é muito baixo
      erro_acumulado += (cont_dir * VAL_ERRO_ENC_1);
      if (erro_acumulado > DIST_POR_ENC)
        set_contadores(0 , (int) erro_acumulado / DIST_POR_ENC);
  }
}

void interrupcao_dir()
{
  if (cont_dir > 0)
    cont_dir--;
  else
  {
    Motor_dir.run(RELEASE);
    if (cont_esq > 0)
      // Quase impossivel rodar isso pq VAL_ERRO_ENC_1 maximo é muito baixo
      erro_acumulado -= (cont_esq * VAL_ERRO_ENC_1);
      if (erro_acumulado < -DIST_POR_ENC)
        set_contadores((int) -erro_acumulado / DIST_POR_ENC, 0);
  }
}
