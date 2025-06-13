 #include <AFMotor.h>
 
 /* AINDA NÃO IMPLEMENTADO
 // === Configurações do sensor ultrassônico ===
 #define DISTANCIA_MIN 25 // Distancia minima permitida antes de desviar
 #define ECHO 39 // porta ligada ao sensor
 #define TRIG 38 // porta ligada ao sensor
 #define PARAM_DIV_SENSOR 58.2 // parametro do sensor (especificado pelo fabricante)
 //==================================
 */

 // === Portas onde os motores estão conectados ===
 #define PORTA_MOT_E 1 // porta do shield do motor esquerdo
 #define PORTA_MOT_D 2 // porta do shield do motor direito
 //==================================
 
 // === Configurações dos encoders (monitoramento dos motores) ===
 #define PINO_ENC_ESQ 18 // ligado na interrupção 5 (monitoramento do motor esquerdo)
 #define PINO_ENC_DIR 19 // ligado na interrupção 4 (monitoramento do motor direito)
 #define VEL_REAL_INC 0.005 // quanto a velocidade real pode ser ajustada (2% por vez)
 #define VEL_REAL_MIN 0.7 // menor velocidade permitida para os motores (em %)
 #define MAX_GAP_ENC 2 // maior diferença permitida entre os ciclos de encorder antes de corrigir
 //==================================

 // === Configurações dos Sensores de Linha ===
 #define P_SENS_ESQ  // porta do sensor de linha esquerdo
 #define P_SENS_DIR  // porta do sensor de linha direito
 #define MS_POR_ALINHADA 300 // tempo (ms) antes de tentar corrigir o alinhamento da linha preta
 //==================================
 
 //=== Definindo variaveis de escopo global =================================
 AF_DCMotor Motor_esq(PORTA_MOT_E);
 AF_DCMotor Motor_dir(PORTA_MOT_D);
 
 // distancia captada pelo sensor
 int distancia;
 
 // variaveis dos valores lidos pelos sensores de linha
 bool sl_esq;
 bool sl_dir;

 // contador de tiks de concorrencia dos encoders dos motores
 volatile int cont_enc = 0;
 
 // velocidade ideal desejada para ser aplicada no carro
 uint8_t velocidade = 0;
 
 // velocidade real dos motores (em % da velocidade ideal)
 volatile double vel_real_dir = 1;
 volatile double vel_real_esq = 1;
 
 // variavel para poder fazer o teste de N ms que passaram
 unsigned long int ms_ultima_alinhada = 0;

 // flag de dispacho das velocidades
 bool disp = 0;

 // par dispachado das velocidades dos sensores que andam reto
 double disp_vel_dir;
 double disp_vel_esq;
 //==========================================================================

 // inicia as parada certinho
 void setup() 
 {
    /* TODO:
   //definição das portas do ultrassonico.
   pinMode(TRIG, OUTPUT);
   pinMode(ECHO, INPUT);
    */

   //definição das portas dos encoders
   pinMode(PINO_ENC_ESQ, INPUT);
   pinMode(PINO_ENC_DIR, INPUT);
 
   //definição das portas dos sensores de linha
   pinMode(P_SENS_ESQ, INPUT);
   pinMode(P_SENS_DIR, INPUT);

   //iniciando as interrupcoes
   attachInterrupt(5, interrupcao_esq, FALLING);
   attachInterrupt(4, interrupcao_dir, FALLING);
 }
 
 // atualiza os sensores contantemente, e em um intervalo definido ve se tem linha pra tratar o ajuste
 void loop() 
 {
    atualiza_sensores();
    
    // conferindo a cada MS_POR_ALINHADA se tem linha pra corrigir e seguir
    if ((millis() - ms_ultima_alinhada) > MS_POR_ALINHADA)
    {
        if (sl_esq || sl_dir)
        {
            if (!disp)
            {
                detachInterrupt(5);
                detachInterrupt(4);
                disp_vel_esq = vel_real_esq;
                disp_vel_dir = vel_real_dir;
                disp = 1;
            }
            segue_linha();
        }
        else if (disp)
        {
            attachInterrupt(5, interrupcao_esq, FALLING);
            attachInterrupt(4, interrupcao_dir, FALLING);
            vel_real_esq = disp_vel_esq;
            vel_real_dir = disp_vel_dir;
            disp = 0;
        }
        set_motores(velocidade, FORWARD, FORWARD)
        ms_ultima_alinhada = millis();
    }
 }
 
 // atualizam/leem os sensores
 void atualiza_sensores()
 {
    atualiza_sens_dista();
    atualiza_sens_linha();
    atualiza_sens_cores();
 }
 void atualiza_sens_dista()
 {
    digitalWrite(TRIG, HIGH);
    delay(1);
    digitalWrite(TRIG, LOW);
    distancia = ( pulseIn(ECHO, HIGH) / PARAM_DIV_SENSOR );
 }
 void atualiza_sens_linha()
 {
    // TODO:
    // faz o que tem que fazer
 }
 void atualiza_sens_cores()
 {
    // TODO:
    // faz o que tem que fazer
 }
 
 // ajusta os motores baseado em onde ta a linha
 void segue_linha()
 {
    if (sl_esq)
    {
        if (vel_dir_real < 1.0)
            vel_dir_real += VEL_REAL_INC;
        else if (vel_esq_real > VEL_REAL_MIN)
            vel_esq_real -= VEL_REAL_INC;
    }
    else if (sl_dir)
    {
        if (vel_esq_real < 1.0)
            vel_esq_real += VEL_REAL_INC;
        else if (vel_dir_real > VEL_REAL_MIN)
            vel_dir_real -= VEL_REAL_INC;
        cont_enc = 0;
    }
    // TODO:
    // if os 2 tao ligado, qi tem que ver o que qeu vamo fazer.
    // tem que colocar o sensor de cor e num sei que n sei que la.

 }
 
 // atualiza a velocidade e direcao dos 2 motores ja considerando a porcentagem real de cada um internamente
 void set_motores(uint8_t velocidade, int direcao_esq, int direcao_dir)
 {
   Motor_esq.setSpeed((uint8_t) (velocidade * vel_motor_esq_real));
   Motor_dir.setSpeed((uint8_t) (velocidade * vel_motor_dir_real));
   Motor_esq.run(direcao_esq);
   Motor_dir.run(direcao_dir);
 }
 
 // interrupcoes dos encoders que alinham os motores enquanto ele nao tiver lendo linha 
 void interrupcao_esq()
 {
    cont_enc--;
    if (cont_enc < -MAX_GAP_ENC)
    {
        if (vel_dir_real < 1.0)
            vel_dir_real += VEL_REAL_INC;
        else if (vel_esq_real > VEL_REAL_MIN)
            vel_esq_real -= VEL_REAL_INC;
        cont_enc = 0;
    }
 }
 void interrupcao_dir()
 {
    cont_enc++;
    if (cont_enc > MAX_GAP_ENC)
    {
        if (vel_esq_real < 1.0)
            vel_esq_real += VEL_REAL_INC;
        else if (vel_dir_real > VEL_REAL_MIN)
            vel_dir_real -= VEL_REAL_INC;
        cont_enc = 0;
    }   
 }