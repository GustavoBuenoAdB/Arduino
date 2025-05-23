
/// Porta Sensor de Linha Preta Esquerdo mais distante do centro
#define PSLP_ESQ_LADO 7
// Porta Sensor de Linha Preta Direito mais distante do centro 
#define PSLP_DIR_LADO 8
// Porta Sensor de Linha Preta Esquerdo no centro
#define PSLP_ESQ_MEIO 9
// Porta Sensor de Linha Preta Direito no centro
#define PSLP_DIR_MEIO 10

// Porta Motor Esquerdo
#define PORTA_MOT_E 1
// Porta Motor Direito
#define PORTA_MOT_D 2

// Definicoes de Valores para traducao real ===

// velocidades
#define VEL_LENTA 1
#define VEL_PADRAO 1
#define VEL_RAPIDA 1

// duracoes
#define DURACAO_CURTA 1
#define DURACAO_PADRAO 1
#define DURACAO_LONGA 1

// duracoes para cada angulo de curva em cada velocidade
#define DURACAO_ANG_CURVA_P_VEL_LENTA 1
#define DURACAO_ANG_CURVA_P_VEL_PADRAO 1
#define DURACAO_ANG_CURVA_P_VEL_RAPIDA 1


// globais para armazenar a leitura dos sensores de linha
int sen_Esq_Lado = 0;
int sen_Dir_Lado = 0;
int sen_Esq_Meio = 0;
int sen_Dir_Meio = 0;

typedef struct Comando
{
    int mov_MotEsq; // define se o motor avança (1) retrocede (-1) ou fica parado (0).
    int mov_MotDir; // define se o motor avança (1) retrocede (-1) ou fica parado (0).
    int mov_duracao; // duracao em ms do movimento dos motores.
    uint8_t mov_velocidade; // velocidade pra aplicar idealmente em ambos os motores.
    int leitura_anterior[4]; // leitura anterior dos 4 sensores de linha.

} Comando;

Comando comando;

#define MOV_LEVE 1
#define MOV_BRUCO 2
#define MOV_DELAY 3.0

AF_DCMotor Motor_esq(PORTA_MOT_E);
AF_DCMotor Motor_dir(PORTA_MOT_D);

uint8_t velocidade = 200;

#define INC_MOTOR_REAL 0.02 // 2%
double porcentagem_motor_Esq_real = 0.9;
double porcentagem_motor_Dir_real = 0.9;

void setup()
{
    // inicializando os sensores de linha preta
    pinMode(PSLP_ESQ_LADO, INPUT);
    pinMode(PSLP_DIR_LADO, INPUT);
    pinMode(PSLP_ESQ_MEIO, INPUT);
    pinMode(PSLP_DIR_MEIO, INPUT);
}

// loop de estados simples para gerar rotina
void loop()
{
    atualiza_Sensores(); // atualiza os sensores 
    define_Comando(); // gera um Comando baseado na Interpretacao dos sensores
    executa_Comando(); // executa o comando gerado anteriormente
}

void atualiza_Sensores()
{
    // Atualizando sensores de Linha Preta
    sen_Esq_Lado = digitalRead(PSLP_ESQ_LADO);
    sen_Dir_Lado = digitalRead(PSLP_DIR_LADO);
    sen_Esq_Meio = digitalRead(PSLP_ESQ_MEIO);
    sen_Dir_Meio = digitalRead(PSLP_DIR_MEIO);
    delay(1); // delay pra ler os sensores
}

void define_Comando()
{

    // Situacao Ideal.
    // if (B - P - P - B) {...}
    if ( !(sen_Esq_Lado) && (sen_Esq_Meio) && (sen_Dir_Meio) && !(sen_Dir_Lado) )
    {
        comando.mov_MotEsq = 1;
        comando.mov_MotDir = 1;
        comando.mov_velocidade = VEL_PADRAO;
        comando.mov_duracao = DURACAO_PADRAO;
    }

    // if (B - B - P - B) {...}
    else if ( !(sen_Esq_Lado) && !(sen_Esq_Meio) && (sen_Dir_Meio) && !(sen_Dir_Lado) )
    {
        comando.mov_MotEsq = 0;
        comando.mov_MotDir = -1;
        comando.mov_velocidade = VEL_PADRAO;
        comando.mov_duracao = DURACAO_CURTA;
    }

    // if (B - P - B - B) {...}
    else if ( !(sen_Esq_Lado) && (sen_Esq_Meio) && !(sen_Dir_Meio) && !(sen_Dir_Lado) )
    {
        comando.mov_MotEsq = -1;
        comando.mov_MotDir = 0;
        comando.mov_velocidade = VEL_PADRAO;
        comando.mov_duracao = DURACAO_CURTA;
    }

    // if (P - P - P - P) {...}
    else if ( (sen_Esq_Lado) && (sen_Esq_Meio) && (sen_Dir_Meio) && (sen_Dir_Lado) )
    {
        comando.mov_MotEsq = 0;
        comando.mov_MotDir = 0;
    }

    // Situacao Problema.
    // if (B - B - B - B) {...}
    else if ( !(sen_Esq_Lado) && !(sen_Esq_Meio) && !(sen_Dir_Meio) && !(sen_Dir_Lado) )
    {
        // supondo um gap
        comando.mov_MotEsq = 1;
        comando.mov_MotDir = 1;
        // diminui a velocidade pra tentar evitar de ignorar uma linha em s1 e s4
        comando.mov_velocidade = VEL_LENTA;
        comando.mov_duracao = DURACAO_CURTA;
    }

    // if (B - P - P - P) ou (B - B - B - P) {...}
    else if ( !(sen_Esq_Lado) && ( ( (sen_Esq_Meio) && (sen_Dir_Meio) ) || ( !(sen_Esq_Meio) && !(sen_Dir_Meio) ) ) && (sen_Dir_Lado) )
    {
        comando.mov_MotEsq = 1;
        comando.mov_MotDir = -1;
        // diminui a velocidade pra tentar evitar de ignorar uma linha em s1 e s4
        comando.mov_velocidade = VEL_LENTA;
        comando.mov_duracao = DURACAO_ANG_CURVA_P_VEL_LENTA * 90;
    }

    // if (P - P - P - B) ou (P - B - B - B) {...}
    else if ( (sen_Esq_Lado) && ( ( (sen_Esq_Meio) && (sen_Dir_Meio) ) || ( !(sen_Esq_Meio) && !(sen_Dir_Meio) ) ) && !(sen_Dir_Lado) )
    {
        comando.mov_MotEsq = -1;
        comando.mov_MotDir = 1;
        // diminui a velocidade pra tentar evitar de ignorar uma linha em s1 e s4
        comando.mov_velocidade = VEL_LENTA;
        comando.mov_duracao = DURACAO_ANG_CURVA_P_VEL_LENTA * 90;
    }
}


void executa_Comando()
{
    int dir_Esq, dir_Dir;

    if (comando.mov_MotEsq > 0)
        dir_Esq = FORWARD;
    else if (comando.mov_MotEsq < 0)
        dir_Esq = BACKWARD;
    else
        dir_Esq = RELEASE;
    
    if (comando.mov_MotDir > 0)
        dir_Esq = FORWARD;
    else if (comando.mov_MotDir < 0)
        dir_Esq = BACKWARD;
    else
        dir_Esq = RELEASE;

    set_Motores(comando.mov_velocidade, dir_Esq, dir_Dir);

    delay(comando.mov_duracao);
}


void set_motores(uint8_t velocidade, int direcao_esq, int direcao_dir)
{
  Motor_esq.setSpeed((uint8_t) (velocidade * porcentagem_motor_Esq_real));
  Motor_dir.setSpeed((uint8_t) (velocidade * porcentagem_motor_Dir_real));
  Motor_esq.run(direcao_esq);
  Motor_dir.run(direcao_dir);
}