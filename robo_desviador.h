//=== Definições para as marchas ===
#define N_MARCHAS 5
#define MIN_VEL 105
//==================================

//=== Definições para o sensor =====
#define DISTANCIA_MIN 25
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
#define MS_POR_ALINHADA 1000 // tempo em milissegundos para esperar antes de tentar alinhar os motores de novo
#define VEL_REAL_INC 0.01 // 1% por ajuste
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
void anda(int direcao);

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